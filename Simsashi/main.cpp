#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <Botsashi/botsashi.h>
#include "simsashi.h"
using namespace botsashi;
using namespace std;

vector<uint8_t> memory(0x1000000, 0);

class SimsashiInterface : public BotsashiInterface
{
    public:
	SimsashiInterface(bool &is_stopped) : stopped(is_stopped)
	{
	    simIO = new SimsashiIO(stopped);
	}

	~SimsashiInterface()
	{
	    simIO = NULL;
	}

	bool init()
	{
	    return simIO->init();
	}

	void shutdown()
	{
	    simIO->shutdown();
	}

	void mainLoop(bool &quit)
	{
	    simIO->mainLoop(quit);
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

	    uint32_t function_number = getDataReg<uint8_t>(m68k, 0);
	    
	    switch (function_number)
	    {
		case 9: simIO->halt(); break;
		case 13: simIO->textOutCR(form_string(m68k)); break;
		case 14: simIO->textOut(form_string(m68k)); break;
		case 33:
		{
		    uint32_t out_val = getDataReg(m68k, 1);

		    switch (out_val)
		    {
			case 0:
			{
			    uint16_t width = 0;
			    uint16_t height = 0;
			    simIO->getWindowSize(width, height);
			    uint32_t ret_val = ((width << 16) | height);
			    setDataReg(m68k, 1, ret_val);
			}
			break;
			case 1: simIO->setFullScreen(false); break;
			case 2: simIO->setFullScreen(true); break;
			default:
			{
			    uint16_t width = min<uint16_t>(640, (out_val >> 16));
			    uint16_t height = min<uint16_t>(480, (out_val & 0xFFFF));
			    simIO->setWindowSize(width, height);
			}
			break;
		    }
		}
		break;
		case 50: simIO->closeAllFiles(); break;
		case 51:
		{
		    uint32_t file_id = 0;
		    uint16_t ret = 0;
		    string filename = form_string(m68k);

		    simIO->openFile(filename, ret, file_id);
		    setDataReg(m68k, 0, ret);

		    if (ret == 0)
		    {
			cout << "File ID of file is " << dec << file_id << endl;
			setDataReg(m68k, 1, file_id);
		    }
		}
		break;
		case 52:
		{
		    uint32_t file_id = 0;
		    uint16_t ret = 0;
		    string filename = form_string(m68k);

		    simIO->newFile(filename, ret, file_id);
		    setDataReg(m68k, 0, ret);

		    if (ret == 0)
		    {
			cout << "File ID of file is " << dec << file_id << endl;
			setDataReg(m68k, 1, file_id);
		    }
		}
		break;
		case 53:
		{
		    uint32_t file_id = getDataReg(m68k, 1);
		    uint32_t buffer_address = getAddrReg(m68k, 1);
		    uint32_t num_bytes = getDataReg(m68k, 2);

		    uint16_t ret = 0;

		    auto file_data = simIO->readFile(file_id, num_bytes, ret);

		    setDataReg(m68k, 0, ret);
		    setDataReg(m68k, 2, file_data.size());

		    if (ret < 2)
		    {
			for (uint32_t addr = 0; addr < file_data.size(); addr++)
			{
			    memory[(buffer_address + addr)] = file_data.at(addr);
			}
		    }
		}
		break;
		case 54:
		{
		    uint32_t file_id = getDataReg(m68k, 1);
		    uint32_t buffer_address = getAddrReg(m68k, 1);
		    uint32_t num_bytes = getDataReg(m68k, 2);

		    auto begin = (memory.begin() + buffer_address);
		    auto end = (begin + num_bytes);
		    vector<uint8_t> file_data = vector<uint8_t>(begin, end);

		    uint16_t ret = 0;

		    simIO->writeFile(file_id, file_data, num_bytes, ret);
		    setDataReg(m68k, 0, ret);
		}
		break;
		case 55:
		{
		    uint32_t file_id = getDataReg(m68k, 1);
		    uint32_t file_pos = getDataReg(m68k, 2);

		    uint16_t ret = 0;
		    simIO->seekFile(file_id, file_pos, ret);
		    setDataReg(m68k, 0, ret);
		}
		break;
		case 57:
		{
		    string filename = form_string(m68k);
		    uint16_t ret = 0;
		    simIO->deleteFile(filename, ret);
		    setDataReg(m68k, 0, ret);
		}
		break;
		case 70:
		{
		    string filename = form_string(m68k);
		    uint16_t ret = 0;
		    simIO->playWAV(filename, ret);
		    setDataReg(m68k, 0, ret);
		}
		break;
		case 71:
		{
		    string filename = form_string(m68k);
		    uint8_t ref_num = getDataReg<uint8_t>(m68k, 1);
		    simIO->loadWAV(filename, ref_num);
		}
		break;
		case 72:
		{
		    uint8_t ref_num = getDataReg<uint8_t>(m68k, 1);
		    uint16_t ret = 0;
		    simIO->playSound(ref_num, ret);
		    setDataReg(m68k, 0, ret);
		}
		break;
		case 80: simIO->setPenColor(getDataReg(m68k, 1)); break;
		case 81: simIO->setFillColor(getDataReg(m68k, 1)); break;
		case 82:
		{
		    uint32_t xpos = getDataReg<uint16_t>(m68k, 1);
		    uint32_t ypos = getDataReg<uint16_t>(m68k, 2);
		    simIO->setPixel(xpos, ypos);
		}
		break;
		case 83:
		{
		    uint32_t xpos = getDataReg<uint16_t>(m68k, 1);
		    uint32_t ypos = getDataReg<uint16_t>(m68k, 2);

		    uint32_t color = simIO->getPixel(xpos, ypos);
		    setDataReg(m68k, 0, color);
		}
		break;
		case 84:
		{
		    uint32_t x1 = getDataReg<uint16_t>(m68k, 1);
		    uint32_t y1 = getDataReg<uint16_t>(m68k, 2);
		    uint32_t x2 = getDataReg<uint16_t>(m68k, 3);
		    uint32_t y2 = getDataReg<uint16_t>(m68k, 4);

		    simIO->line(x1, y1, x2, y2);
		}
		break;
		case 85:
		{
		    uint32_t xpos = getDataReg<uint16_t>(m68k, 1);
		    uint32_t ypos = getDataReg<uint16_t>(m68k, 2);

		    simIO->lineTo(xpos, ypos);
		}
		break;
		case 86:
		{
		    uint32_t xpos = getDataReg<uint16_t>(m68k, 1);
		    uint32_t ypos = getDataReg<uint16_t>(m68k, 2);

		    simIO->moveTo(xpos, ypos);
		}
		break;
		case 87:
		{
		    uint32_t leftx = getDataReg<uint16_t>(m68k, 1);
		    uint32_t uppery = getDataReg<uint16_t>(m68k, 2);
		    uint32_t rightx = getDataReg<uint16_t>(m68k, 3);
		    uint32_t lowery = getDataReg<uint16_t>(m68k, 4);

		    simIO->rectangle(leftx, uppery, rightx, lowery);
		}
		break;
		case 88:
		{
		    uint32_t leftx = getDataReg<uint16_t>(m68k, 1);
		    uint32_t uppery = getDataReg<uint16_t>(m68k, 2);
		    uint32_t rightx = getDataReg<uint16_t>(m68k, 3);
		    uint32_t lowery = getDataReg<uint16_t>(m68k, 4);

		    simIO->ellipse(leftx, uppery, rightx, lowery);
		}
		break;
		case 89:
		{
		    uint32_t xpos = getDataReg<uint16_t>(m68k, 1);
		    uint32_t ypos = getDataReg<uint16_t>(m68k, 2);

		    simIO->floodFill(xpos, ypos);
		}
		break;
		case 90:
		{
		    uint32_t leftx = getDataReg<uint16_t>(m68k, 1);
		    uint32_t uppery = getDataReg<uint16_t>(m68k, 2);
		    uint32_t rightx = getDataReg<uint16_t>(m68k, 3);
		    uint32_t lowery = getDataReg<uint16_t>(m68k, 4);

		    simIO->unfilledRectangle(leftx, uppery, rightx, lowery);
		}
		break;
		case 91:
		{
		    uint32_t leftx = getDataReg<uint16_t>(m68k, 1);
		    uint32_t uppery = getDataReg<uint16_t>(m68k, 2);
		    uint32_t rightx = getDataReg<uint16_t>(m68k, 3);
		    uint32_t lowery = getDataReg<uint16_t>(m68k, 4);

		    simIO->unfilledEllipse(leftx, uppery, rightx, lowery);
		}
		break;
		case 92: simIO->setDrawingMode(getDataReg<uint8_t>(m68k, 1)); break;
		case 93: simIO->setPenWidth(getDataReg<uint8_t>(m68k, 1)); break;
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

	void take_screenshot()
	{
	    simIO->take_screenshot();
	}

    private:
	bool &stopped;

	SimsashiIO *simIO = NULL;

	string form_string(Botsashi &m68k, int addr_reg = 1)
	{
	    uint32_t str_base = getAddrReg(m68k, addr_reg);
	    stringstream text_str;

	    for (uint32_t addr = str_base; (memory[addr] != 0); addr++)
	    {
		text_str.put(memory[addr]);
	    }

	    return text_str.str();
	}

	template<typename T = uint32_t>
	uint32_t getDataReg(Botsashi &m68k, int reg)
	{
	    switch (sizeof(T))
	    {
		case 1: return m68k.getDataReg<Botsashi::Byte>(reg); break;
		case 2: return m68k.getDataReg<Botsashi::Word>(reg); break;
		default: return m68k.getDataReg<Botsashi::Long>(reg); break;
	    }
	}

	template<typename T>
	void setDataReg(Botsashi &m68k, int reg, T val)
	{
	    switch (sizeof(T))
	    {
		case 1: m68k.setDataReg<Botsashi::Byte>(reg, val); break;
		case 2: m68k.setDataReg<Botsashi::Word>(reg, val); break;
		default: m68k.setDataReg<Botsashi::Long>(reg, val); break;
	    }
	}

	template<typename T = uint32_t>
	uint32_t getAddrReg(Botsashi &m68k, int reg)
	{
	    switch (sizeof(T))
	    {
		case 1: return m68k.getAddrReg<Botsashi::Byte>(reg); break;
		case 2: return m68k.getAddrReg<Botsashi::Word>(reg); break;
		default: return m68k.getAddrReg<Botsashi::Long>(reg); break;
	    }
	}
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

bool is_ctrl_pressed(SDL_Event event)
{
    return (event.key.keysym.mod & KMOD_CTRL) ? true : false;
}

void display_debug(Botsashi &m68k, uint64_t &total_cycles)
{
    uint64_t prev_cycles = total_cycles;
    // Uncomment the below line to enable debug output
    // m68k.debugoutput();
    stringstream instr;
    size_t offs = m68k.disassembleinstr(instr, m68k.getPC());
    cout << "Instruction: " << instr.str() << endl;
    cout << "Offset: " << dec << int(offs) << endl;
    total_cycles += m68k.executenextinstr();
    uint64_t cycles = (total_cycles - prev_cycles);
    cout << "Cycles taken: " << dec << cycles << endl;
    cout << endl;
}

void display_total_cycles(uint64_t total_cycles)
{
    cout << "Program execution finished." << endl;
    cout << "Total cycles: " << dec << total_cycles << endl;
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

    bool stopped = false;
    SimsashiInterface inter(stopped);

    if (!inter.init())
    {
	return 1;
    }

    copyprogram(bin_file);

    Botsashi m68k;
    m68k.setinterface(inter);
    m68k.init();

    bool quit = false;

    uint64_t total_cycles = 0;

    while (!quit)
    {
	inter.mainLoop(quit);

	if (!stopped)
	{
	    display_debug(m68k, total_cycles);
	}
    }

    display_total_cycles(total_cycles);
    m68k.shutdown();
    inter.shutdown();
    return 0;
}