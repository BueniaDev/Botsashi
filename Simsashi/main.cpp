#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <functional>
#include <chrono>
#include <SDL2/SDL.h>
#include <Botsashi/botsashi.h>
using namespace botsashi;
using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

vector<uint8_t> memory(0x1000000, 0);

void sdl_error(string message)
{
    cout << message << " SDL_Error: " << SDL_GetError() << endl;
}

class SimsashiInterface : public BotsashiInterface
{
    public:
	SimsashiInterface(bool &cb) : stopped(cb)
	{

	}

	~SimsashiInterface()
	{

	}

	bool init_sdl2()
	{
	    window = SDL_CreateWindow("Simsashi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

	    if (window == NULL)
	    {
		sdl_error("Window could not be created!");
		return false;
	    }

	    render = SDL_CreateRenderer(window, -1, 0);

	    if (render == NULL)
	    {
		sdl_error("Renderer could not be created!");
		return false;
	    }

	    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	    SDL_RenderClear(render);

	    return true;
	}

	void stop_sdl2()
	{
	    SDL_DestroyRenderer(render);
	    SDL_DestroyWindow(window);
	}

	void render_sdl2()
	{
	    SDL_RenderPresent(render);
	}

	uint16_t readWord(bool upper, bool lower, uint32_t addr)
	{
	    uint16_t value = 0;
	    if (upper)
	    {
		value |= (memory[addr] << 8);
	    }

	    if (lower)
	    {
		value |= memory[(addr + 1)];
	    }

	    return value;
	}

	void writeWord(bool upper, bool lower, uint32_t addr, uint16_t val)
	{
	    if (upper)
	    {
		memory[addr] = (val >> 8);
	    }

	    if (lower)
	    {
		memory[(addr + 1)] = (val & 0xFF);
	    }
	}

	bool istrapOverride(int val)
	{
	    return (val == 15);
	}

	void trapException(int val, Botsashi &m68k)
	{
	    if (val != 15)
	    {
		return;
	    }

	    uint32_t function_number = m68k.getDataReg<Botsashi::Long>(0);
	    
	    switch (function_number)
	    {
		case 8: m68k.setDataReg<Botsashi::Long>(1, fetchTimeSinceMidnight()); break;
		case 9: stopSimsashi(); break;
		case 13: display_string(m68k, true); break;
		case 14: display_string(m68k); break;
		case 33:
		{
		    uint32_t screen_reg = m68k.getDataReg<Botsashi::Long>(1);

		    switch (screen_reg)
		    {
			case 0:
			{
			    int width = 0;
			    int height = 0;

			    SDL_GetRendererOutputSize(render, &width, &height);

			    cout << "Screen resolution is " << dec << width << " pixels wide by " << height << " pixels high" << endl;
			    uint16_t screen_width = static_cast<uint16_t>(width);
			    uint16_t screen_height = static_cast<uint16_t>(height);

			    uint32_t screen_res = ((screen_width << 16) | screen_height);
			    m68k.setDataReg<Botsashi::Long>(1, screen_res);
			}
			break;
			case 1: cout << "Unimplemented: Setting windowed mode" << endl; break;
			case 2: cout << "Unimplemented: Setting full-screen mode" << endl; break;
			default: cout << "Unimplemented: Setting screen resolution" << endl; break;
		    }
		}
		break;
		case 50:
		{
		    cout << "Closing all files..." << endl;
		    close_all_files();
		}
		break;
		case 51:
		{
		    string filename = fetch_filename(m68k);

		    int file_id = open_file(filename, false);

		    if (file_id < 0)
		    {
			m68k.setDataReg<Botsashi::Word>(0, 2);
			m68k.setDataReg<Botsashi::Long>(1, -1);
			return;
		    }

		    m68k.setDataReg<Botsashi::Word>(0, 0);
		    m68k.setDataReg<Botsashi::Word>(1, file_id);
		}
		break;
		case 52:
		{
		    string filename = fetch_filename(m68k);

		    int file_id = open_file(filename);

		    if (file_id < 0)
		    {
			m68k.setDataReg<Botsashi::Word>(0, 2);
			m68k.setDataReg<Botsashi::Long>(1, -1);
			return;
		    }

		    m68k.setDataReg<Botsashi::Word>(0, 0);
		    m68k.setDataReg<Botsashi::Word>(1, file_id);
		}
		break;
		case 53:
		{
		    uint32_t file_id = m68k.getDataReg<Botsashi::Long>(1);
		    uint32_t num_bytes = m68k.getDataReg<Botsashi::Long>(2);
		    uint32_t buffer_addr = m68k.getAddrReg<Botsashi::Long>(1);

		    if ((file_id < 0) || (file_id >= 8))
		    {
			cout << "Invalid file ID" << endl;
			m68k.setDataReg<Botsashi::Word>(0, 2);
			return;
		    }

		    cout << "Reading " << dec << num_bytes << " bytes from file " << dec << file_id << " to buffer starting at 0x" << hex << buffer_addr << endl;
		    vector<uint8_t> read_buffer;
		    uint32_t num_bytes_read = read_file(file_id, read_buffer, num_bytes);

		    for (uint32_t index = 0; index < num_bytes_read; index++)
		    {
			m68k.write<Botsashi::Byte>((buffer_addr + index), read_buffer.at(index));
		    }

		    if (num_bytes_read < num_bytes)
		    {
			cout << "EOF reached" << endl;
			m68k.setDataReg<Botsashi::Word>(0, 1);
			return;
		    }

		    m68k.setDataReg<Botsashi::Word>(0, 0);
		}
		break;
		case 54:
		{
		    uint32_t file_id = m68k.getDataReg<Botsashi::Long>(1);
		    uint32_t num_bytes = m68k.getDataReg<Botsashi::Long>(2);
		    uint32_t buffer_addr = m68k.getAddrReg<Botsashi::Long>(1);

		    if ((file_id < 0) || (file_id >= 8))
		    {
			cout << "Invalid file ID" << endl;
			m68k.setDataReg<Botsashi::Word>(0, 2);
			return;
		    }

		    cout << "Writing " << dec << num_bytes << " bytes to file " << dec << file_id << " from buffer starting at 0x" << hex << buffer_addr << endl;

		    vector<uint8_t> write_buffer = create_write_buffer(m68k, buffer_addr, num_bytes);
		    if (!write_file(file_id, write_buffer))
		    {
			m68k.setDataReg<Botsashi::Word>(0, 2);
			return;
		    }

		    m68k.setDataReg<Botsashi::Word>(0, 0);
		}
		break;
		case 55:
		{
		    uint32_t file_id = m68k.getDataReg<Botsashi::Long>(1);
		    uint32_t file_pos = m68k.getDataReg<Botsashi::Long>(2);
		    
		    if ((file_id < 0) || (file_id >= 8))
		    {
			cout << "Invalid file ID" << endl;
			m68k.setDataReg<Botsashi::Word>(0, 2);
			return;
		    }

		    seek_file(file_id, file_pos);
		}
		break;
		case 56:
		{
		    uint32_t file_id = m68k.getDataReg<Botsashi::Long>(1);

		    if ((file_id < 0) || (file_id >= 8))
		    {
			cout << "Invalid file ID" << endl;
			m68k.setDataReg<Botsashi::Word>(0, 2);
			return;
		    }

		    cout << "Closing file " << dec << file_id << endl;
		    close_file(file_id);
		}
		break;
		case 57:
		{
		    cout << "Unimplemented: File deletion" << endl;
		    close_all_files();
		}
		break;
		case 80:
		{
		    uint32_t pen_color = m68k.getDataReg<Botsashi::Long>(1);

		    if ((pen_color >> 24) != 0)
		    {
			cout << "Invalid color" << endl;
			return;
		    }

		    uint8_t red = (pen_color & 0xFF);
		    uint8_t green = ((pen_color >> 8) & 0xFF);
		    uint8_t blue = ((pen_color >> 16) & 0xFF);
		    cout << "Setting pen color to (" << dec << (int)red << ", " << dec << (int)green << ", " << dec << (int)blue << ")" << endl;
		    SDL_SetRenderDrawColor(render, red, green, blue, 0xFF);
		}
		break;
		case 82:
		{
		    uint16_t xpos = m68k.getDataReg<Botsashi::Word>(1);
		    uint16_t ypos = m68k.getDataReg<Botsashi::Word>(2);
		    cout << "Drawing pixel at (" << dec << (int)xpos << ", " << dec << (int)ypos << ")" << endl;
		    SDL_RenderDrawPoint(render, xpos, ypos);
		    SDL_RenderPresent(render);
		}
		break;
		default: unrecognized_trapfunc(function_number); break;
	    }
	}

	// Halts Simsashi
	void stopSimsashi()
	{
	    cout << "Stopping Simsashi..." << endl;
	    stopped = true;
	}

	void stopFunction()
	{
	    stopSimsashi();
	}

	void unrecognized_trapfunc(uint32_t func_num)
	{
	    cout << "Unrecognized I/O function number of " << dec << func_num << endl;
	    exit(0);
	}

	// Fetches the current time since midnight, in centiseconds (hundredths of a second)
	uint32_t fetchTimeSinceMidnight()
	{
	    auto current_time = chrono::system_clock::now();
	    auto current_timet = chrono::system_clock::to_time_t(current_time);

	    auto midnight_tm = localtime(&current_timet);
	    midnight_tm->tm_hour = 0;
	    midnight_tm->tm_min = 0;
	    midnight_tm->tm_sec = 0;

	    auto midnight_time = chrono::system_clock::from_time_t(mktime(midnight_tm));

	    typedef duration<int64_t, centi> centiseconds;
	    auto midnight_diff = chrono::duration_cast<centiseconds>(current_time - midnight_time);
	    return static_cast<uint32_t>(midnight_diff.count());
	}

	void display_cr_lf()
	{
	    cout << endl;
	}

	// Prints character to stdout
	void display_char(uint8_t char_val)
	{
	    cout.put(char_val);
	}

	void append_char(string &str, uint8_t char_val)
	{
	    if (char_val < 0x20)
	    {
		return;
	    }

	    str += static_cast<char>(char_val);
	}

	string fetch_filename(Botsashi &m68k)
	{
	    string filename_str;
	    uint32_t string_addr = m68k.getAddrReg<Botsashi::Long>(1);

	    for (uint32_t addr = string_addr; m68k.read<Botsashi::Byte>(addr) != 0; addr++)
	    {
		append_char(filename_str, m68k.read<Botsashi::Byte>(addr));
	    }

	    return filename_str;
	}

	void display_string(Botsashi &m68k, bool is_cr_lf = false)
	{
	    uint32_t string_addr = m68k.getAddrReg<Botsashi::Long>(1);

	    for (uint32_t addr = string_addr; m68k.read<Botsashi::Byte>(addr) != 0; addr++)
	    {
		display_char(m68k.read<Botsashi::Byte>(addr));
	    }

	    if (is_cr_lf)
	    {
		display_cr_lf();
	    }
	}

	vector<uint8_t> create_write_buffer(Botsashi &m68k, uint32_t buffer_addr, uint32_t num_bytes)
	{
	    vector<uint8_t> result;
	    for (uint32_t index = 0; index < num_bytes; index++)
	    {
		result.push_back(m68k.read<Botsashi::Byte>((buffer_addr + index)));
	    }

	    return result;
	}

	int open_file(string filename, bool is_new_file = true)
	{
	    if (num_files_open >= 8)
	    {
		cout << "Maximum number of files reached" << endl;
		return -1;
	    }

	    auto file_flags = ios::in | ios::out | ios::binary;

	    if (is_new_file)
	    {
		file_flags |= ios::trunc;
	    }

	    cout << "Opening file of " << filename << endl;
	    files[num_files_open].stream.open(filename, file_flags);

	    if (!is_new_file && !files[num_files_open].stream.is_open())
	    {
		cout << "Could not open file of " << filename << endl;
		return -1;
	    }

	    files[num_files_open].file_id = num_files_open;
	    files[num_files_open].is_open = true;

	    int file_id = num_files_open++;
	    return file_id;
	}

	uint32_t read_file(int file_id, vector<uint8_t> &buffer, int num_bytes)
	{
	    if (!files[file_id].is_open)
	    {
		cout << "File is not currently open" << endl;
		return 0;
	    }

	    buffer.resize(num_bytes, 0);
	    files[file_id].stream.read((char*)buffer.data(), num_bytes);
	    return files[file_id].stream.gcount();
	}

	bool write_file(int file_id, vector<uint8_t> buffer)
	{
	    if (!files[file_id].is_open)
	    {
		cout << "File is not currently open" << endl;
		return false;
	    }

	    files[file_id].stream.write((char*)buffer.data(), buffer.size());
	    return true;
	}

	void seek_file(int file_id, int file_pos)
	{
	    if (!files[file_id].is_open)
	    {
		cout << "File is not currently open" << endl;
		return;
	    }

	    files[file_id].stream.seekg(file_pos, ios::beg);
	    files[file_id].stream.seekp(file_pos, ios::beg);
	}

	void close_all_files()
	{
	    for (int index = 0; index < 8; index++)
	    {
		if (files[index].is_open)
		{
		    files[index].stream.close();
		    files[index].is_open = false;
		    files[index].file_id = -1;
		}
	    }

	    num_files_open = 0;
	}

	void close_file(int file_id)
	{
	    if (files[file_id].is_open)
	    {
	        files[file_id].stream.close();
		files[file_id].is_open = false;
		files[file_id].file_id = -1;
		num_files_open -= 1;
	    }
	}

	private:
	    bool &stopped;

	    struct SimsashiFile
	    {
		fstream stream;
		bool is_open = false;
		int file_id = -1;
	    };

	    array<SimsashiFile, 8> files;

	    uint32_t num_files_open = 0;
	    SDL_Window *window = NULL;
	    SDL_Renderer *render = NULL;

	    uint32_t window_width = 640;
	    uint32_t window_height = 480;
};

void copyprogram(vector<uint8_t> program)
{
    auto membegin = memory.begin();
    copy(program.begin(), program.end(), membegin);
}

template<size_t arr_size>
void copyprogram(array<uint8_t, arr_size> program)
{
    vector<uint8_t> prog = vector<uint8_t>(program.begin(), program.end());
    copyprogram(prog);
}

void print_datareg(Botsashi &m68k, int reg)
{
    cout << "Value of d" << dec << reg << ": " << hex << (int)m68k.getDataReg<Botsashi::Long>(reg) << endl;
}

vector<uint8_t> read_file(string filename)
{
    vector<uint8_t> result;
    ifstream file(filename, ios::in | ios::binary | ios::ate);

    if (!file.is_open())
    {
	cout << "Could not load file." << endl;
	return result;
    }

    streampos size = file.tellg();
    file.seekg(0, ios::beg);
    result.resize(size, 0);
    file.read((char*)result.data(), result.size());
    cout << "File succesfully loaded." << endl;
    file.close();
    return result;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
	cout << "Usage: " << argv[0] << " [binary file]" << endl;
	return 1;
    }

    vector<uint8_t> bin_file = read_file(argv[1]);

    if (bin_file.empty())
    {
	return 1;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
	sdl_error("SDL2 could not be initialized!");
	return 1;
    }

    bool stopped = false;
    SimsashiInterface inter(stopped);

    if (!inter.init_sdl2())
    {
	SDL_Quit();
	return 1;
    }

    copyprogram(bin_file);

    Botsashi m68k;
    m68k.setinterface(inter);
    m68k.init();

    bool quit = false;
    SDL_Event event;

    uint64_t total_cycles = 0;
    uint64_t cycles = 0;

    while (!quit)
    {
	while (SDL_PollEvent(&event))
	{
	    if (event.type == SDL_QUIT)
	    {
		quit = true;
	    }
	}

	if (!stopped)
	{
	    uint64_t prev_cycles = total_cycles;
	    // Uncomment the below line to enable debug output
	    // m68k.debugoutput();
	    cout << m68k.disassembleinstr(m68k.getPC()) << endl;
	    total_cycles += m68k.executenextinstr();
	    cycles = (total_cycles - prev_cycles);
	    cout << "Cycles taken: " << dec << cycles << endl;
	    cout << endl;
	}

	inter.render_sdl2();
    }

    cout << "Program execution finished." << endl;
    cout << "Total cycles: " << dec << total_cycles << endl;
    m68k.shutdown();
    inter.stop_sdl2();
    SDL_Quit();
    return 0;
}