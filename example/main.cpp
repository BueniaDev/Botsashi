#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <functional>
#include <chrono>
#include <Botsashi/botsashi.h>
using namespace botsashi;
using namespace std;
using namespace std::chrono;
using namespace std::placeholders;

vector<uint16_t> memory(0x800000, 0);

array<uint16_t, 12> homework1 = 
{
  0x303C, 0x0123, // move.w #$0123,d0
  0x1200, // move.b d0, d1
  0x3401, // move.w d1, d2
  0x263C, 0xFFFF, 0x0000, // move.l #$FFFF0000, d3
  0x3600, // move.w d0, d3
  0x2003, // move.l d3, d0
  0x3002, // move.w d2, d0
  0x4E72, 0x2700 // stop #2700
};

array<uint16_t, 21> homework2 = 
{
    0x303C, 0x0010, // move.w #$0010, d0
    0x31C0, 0x0040, // move.w d0, $00000040
    0x3200, // move.w d0, d1
    0xD241, // add.w d1, d1
    0xD240, // add.w d0, d1
    0x9278, 0x0040, // sub.w $00000040, d1
    0x4841, // swap d1
    0x3200, // move.w d0, d1
    0x287C, 0x0000, 0x0040, // movea.l #$00000040, a4
    0xD254, // add.w (a4), d1
    0x3881, // move.w d1, (a4)
    0xC340, // exg.l d1, d0
    0x4840, // swap d0
    0x4240, // clr.w d0
    0x4E72, 0x2700, // stop #$2700
};

vector<uint16_t> helloworld =
{
    0x43F9, 0x0000, 0x1012, // lea message, a1
    0x103C, 0x000D, // move.b #13, d0
    0x4E4F, // trap #15
    0x103C, 0x0009, // move.b #9, d0
    0x4E4F, // trap #15
    0x4865, 0x6C6C, 0x6F2C, 0x2077, 0x6F72, 0x6C64, 0x2100 // message dc.b "Hello world!",0
};

vector<uint16_t> timeprog = 
{
    0x103C, 0x0008, // move.b #8, d0
    0x4E4F, // trap #15
    0x103C, 0x009, // move.b #9, d0
    0x4E4F, // trap #15
};

vector<uint16_t> fileprog = 
{
    0x43F9, 0x0000, 0x0018, // lea filename, a1
    0x303C, 0x0034, // move #52, d0
    0x4E4F, // trap #15
    0x303C, 0x0032, // move #50, d0
    0x4E4F, // trap #15
    0x303C, 0x0009, // move #9, d0
    0x4E4F, // trap #15
    0x6B75, 0x6A6F, 0x7465, 0x7374, 0x2E74, 0x7874, 0x0000 // filename dc.b 'kujotest.txt', 0
};

vector<uint16_t> fileprog2 = 
{
    0x43F9, 0x0000, 0x0028, // lea filename, a1
    0x303C, 0x0034, // move #52, d0
    0x4E4F, // trap #15
    0x43F9, 0x0000, 0x0036, // lea data, a1
    0x343C, 0x0019, // move #25, d2 ; Data size is 25 bytes
    0x303C, 0x0036, // move #54, d0
    0x4E4F, // trap #15
    0x303C, 0x0032, // move #50, d0
    0x4E4F, // trap #15
    0x303C, 0x0009, // move #9, d0
    0x4E4F, // trap #15
    0x6B75, 0x6A6F, 0x7465, 0x7374, 0x322E, 0x7478, 0x7400, // filename dc.b 'kujotest2.txt', 0
    0x4B61, 0x7265, 0x6E20, 0x4B75, 0x6A6F, 0x2069, 0x7320, 0x6265, 0x7374, 0x2077,
    0x6169, 0x6675, 0x2100, // data dc.b 'Karen Kujo is best waifu!' ; Yeah, she truly is...
};

class SimsashiInterface : public BotsashiInterface
{
    public:
	SimsashiInterface(bool &cb) : stopped(cb)
	{

	}

	~SimsashiInterface()
	{

	}

	uint16_t readWord(bool upper, bool lower, uint32_t addr)
	{
	    return memory[(addr >> 1)];
	}

	void writeWord(bool upper, bool lower, uint32_t addr, uint16_t val)
	{
	    memory[(addr >> 1)] = val;
	}

	bool istrapOverride(int val)
	{
	    return (val == 15);
	}

	void displaystring(uint32_t stringaddr, bool newline = false)
	{
	    for (uint32_t addr = stringaddr; readByte(addr) != 0; addr++)
	    {
		displaychar(readByte(addr));
	    }

	    if (newline)
	    {
		cout << endl;
	    }
	}

	string fetchstring(uint32_t stringaddr)
	{
	    string fetchedstr;

	    for (uint32_t addr = stringaddr; readByte(addr) != 0; addr++)
	    {
		appendchar(fetchedstr, readByte(addr));
	    }

	    return fetchedstr;
	}

	void appendchar(string &str, uint8_t val)
	{
	    if (val < 0x20)
	    {
		return;
	    }
	    
	    str.push_back(val);
	}

	void displaychar(uint8_t val)
	{
	    // TODO: Make this more robust
	    cout.put(val);
	}

	typedef duration<int64_t, centi> centiseconds;

	auto fetchTimeSinceMidnight() -> uint32_t
	{
	    auto current = system_clock::now();
	    auto now_timet = system_clock::to_time_t(current);

	    auto midnight_local = localtime(&now_timet);
	    midnight_local->tm_hour = 0;
	    midnight_local->tm_min = 0;
	    midnight_local->tm_sec = 0;

	    auto midnight = system_clock::from_time_t(mktime(midnight_local));
	    auto diff_time = duration_cast<centiseconds>(current - midnight);

	    return static_cast<uint32_t>(diff_time.count());
	}

	uint8_t readByte(uint32_t addr)
	{
	    if (addr & 1)
	    {
	        return readWord(false, true, (addr & ~1));
	    }
	    else
	    {
		return (readWord(true, false, (addr & ~1)) >> 8);
	    }
	}

	void trapException(int val, Botsashi *m68k)
	{
	    if (val != 15)
	    {
		return;
	    }

	    uint32_t tasknum = m68k->m68kreg.datareg[0];

	    switch (tasknum)
	    {
		// Fetches time since midnight in centiseconds
		case 8:
		{
		    uint32_t timeSinceMidnight = fetchTimeSinceMidnight();
		    m68k->m68kreg.datareg[1] = timeSinceMidnight;
		}
		break;
		// Halts the simulator
		case 9:
		{
		    stopFunction();
		}
		break;
		// Display NULL-terminated string (with EOF markings)
		case 13:
		{
		    uint32_t stringaddr = m68k->m68kreg.addrreg[1];
		    displaystring(stringaddr, true);
		}
		break;
		// Display NULL-terminated string (without EOF markings)
		case 14:
		{
		    uint32_t stringaddr = m68k->m68kreg.addrreg[1];
		    displaystring(stringaddr);
		}
		break;
		case 50:
		{
		    cout << "Closing all files..." << endl;
		    for (int i = 0; i < 8; i++)
		    {
			files[i].stream.close();
			files[i].id = -1;
		    }

		    num_files_open = 0;
		}
		break;
		case 51:
		{
		    uint32_t stringaddr = m68k->m68kreg.addrreg[1];
		    string filename = fetchstring(stringaddr);
		    cout << "Number of files open: " << dec << num_files_open << endl;

		    if (num_files_open >= 8)
		    {
			cout << "Error: cannot open any more files" << endl;
			m68k->setDataReg<Botsashi::Word>(0, 2);
			m68k->setDataReg<Botsashi::Long>(1, 0xFFFFFFFF);
			return;
		    }

		    cout << "Opening file named " << filename << "..." << endl;

		    files[num_files_open].id = num_files_open;
		    files[num_files_open].stream.open(filename.c_str(), ios::in | ios::out | ios::binary);

		    if (!files[num_files_open].stream.is_open())
		    {
			cout << "Error: could not load file" << endl;
			m68k->setDataReg<Botsashi::Word>(0, 2);
			m68k->setDataReg<Botsashi::Long>(1, 0xFFFFFFFF);
			return;
		    }

		    m68k->setDataReg<Botsashi::Word>(0, 0);
		    m68k->setDataReg<Botsashi::Long>(1, files[num_files_open].id);
		    num_files_open += 1;
		}
		break;
		case 52:
		{
		    uint32_t stringaddr = m68k->m68kreg.addrreg[1];
		    string filename = fetchstring(stringaddr);
		    cout << "Number of files open: " << dec << num_files_open << endl;

		    if (num_files_open >= 8)
		    {
			cout << "Error: cannot open any more files" << endl;
			m68k->setDataReg<Botsashi::Word>(0, 2);
			m68k->setDataReg<Botsashi::Long>(1, 0xFFFFFFFF);
			return;
		    }

		    cout << "Opening file named " << filename << "..." << endl;

		    files[num_files_open].id = num_files_open;
		    files[num_files_open].stream.open(filename.c_str(), ios::in | ios::out | ios::trunc | ios::binary);

		    m68k->setDataReg<Botsashi::Word>(0, 0);
		    m68k->setDataReg<Botsashi::Long>(1, files[num_files_open].id);
		    num_files_open += 1;
		}
		break;
		case 54:
		{
		    uint32_t file_id = m68k->m68kreg.datareg[1];
		    uint32_t buffer_addr = m68k->m68kreg.addrreg[1];
		    uint32_t num_bytes = m68k->m68kreg.datareg[2];

		    cout << "Writing " << dec << num_bytes << " bytes of data to file " << dec << file_id << "..." << endl;

		    if (file_id >= 8)
		    {
			cout << "Error: Invalid file ID" << endl;
			m68k->setDataReg<Botsashi::Word>(0, 2);
			m68k->setDataReg<Botsashi::Long>(1, 0xFFFFFFFF);
			return;
		    }

		    vector<uint8_t> file_data;

		    for (uint32_t index = 0; index < num_bytes; index++)
		    {
			file_data.push_back(readByte((buffer_addr + index)));
		    }

		    files[file_id].stream.write((char*)file_data.data(), file_data.size());
		    m68k->setDataReg<Botsashi::Word>(0, 0);
		}
		break;
		case 55:
		{
		    cout << "Unimplemented: Fetching file position" << endl;
		    for (int i = 0; i < 8; i++)
		    {
			files[i].stream.close();
			files[i].id = -1;
		    }

		    num_files_open = 0;
		}
		break;
		default: cout << "Unrecognized trap function of " << dec << tasknum << endl; exit(1); break;
	    }
	}

	void stopFunction()
	{
	    cout << "Halting Simsashi..." << endl;
	    stopped = true;
	}

	private:
	    bool &stopped;

	    struct SimsashiFile
	    {
		fstream stream;
		int id = -1;
	    };

	    int num_files_open = 0;

	    array<SimsashiFile, 8> files;
};

template<size_t arr_size>
void copyprogram(array<uint16_t, arr_size> program, uint32_t offset = 0)
{
    auto membegin = memory.begin() + (offset >> 1);
    copy(program.begin(), program.end(), membegin);
}

void copyprogram(vector<uint16_t> program, uint32_t offset = 0)
{
    auto membegin = memory.begin() + (offset >> 1);
    copy(program.begin(), program.end(), membegin);
}

void print_datareg(Botsashi &m68k, int reg)
{
    reg &= 7;
    cout << "Value of d" << dec << reg << ": " << hex << (int)m68k.m68kreg.datareg[reg] << endl;
}

vector<uint16_t> read_file(string filename)
{
    vector<uint16_t> binary_file;
    ifstream file(filename, ios::in | ios::binary | ios::ate);

    if (!file.is_open())
    {
	cout << "Could not load file." << endl;
	return binary_file;
    }

    vector<uint8_t> result;
    streampos size = file.tellg();
    file.seekg(0, ios::beg);
    result.resize(size, 0);
    file.read((char*)result.data(), result.size());
    cout << "File succesfully loaded." << endl;
    file.close();

    // Append a NULL byte if the size of the
    // 8-bit binary is an odd number
    if ((result.size() & 1))
    {
	result.push_back(0x00);
    }

    for (size_t index = 0; index < result.size(); index += 2)
    {
	uint16_t value = (result.at(index) << 8) | (result.at((index + 1)));
	binary_file.push_back(value); 
    }

    return binary_file;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
	cout << "Usage: " << argv[0] << " [binary file]" << endl;
	return 1;
    }

    vector<uint16_t> bin_file = read_file(argv[1]);

    if (bin_file.empty())
    {
	return 1;
    }

    Botsashi m68k;
    bool stopped = false;
    SimsashiInterface inter(stopped);
    copyprogram(bin_file);
    
    m68k.setinterface(inter);
    m68k.init();

    while (!stopped)
    {
	// m68k.debugoutput();
	m68k.executenextinstr();
    }

    // print_datareg(m68k, 1);
    cout << "Program execution finished." << endl;
    m68k.shutdown();

    return 0;
}