#include "../m68k.h"
#include <iostream>
#include <string>
#include <fstream>
#include <functional>
using namespace m68k;
using namespace std;
using namespace std::placeholders;

M68K m68kcpu;

uint8_t memorymap[0x1000000];
uint8_t arr1[0x10000];
uint8_t arr2[0x10000];

uint8_t readb(uint32_t address)
{
    address = (address % 0x1000000);

    return memorymap[address];
}

void writeb(uint32_t address, uint8_t value)
{
    address = (address % 0x1000000);    
 
    memorymap[address] = value;
}

uint16_t readw(uint32_t address)
{
    address = (address % 0x1000000);
    return (readb(address) << 8) | readb(address + 1);
}
    
void writew(uint32_t address, uint16_t value)
{
    address = (address % 0x1000000);
        
    writeb((address + 1), (value & 0xFF));
    writeb(address, ((value >> 8) & 0xFF));
}
    
uint32_t readl(uint32_t address)
{
    address = (address % 0x1000000);
        
    return (readw(address) << 16) | readw(address + 2);
}
    
void writel(uint32_t address, uint32_t value)
{
    address = (address % 0x1000000);
        
    writew(address, (value >> 16) & 0xFFFF);
    writew(address + 2, (value & 0xFFFF));
}

void init()
{
    auto rb = bind(readb, _1);
    auto wb = bind(writeb, _1, _2);
    auto rw = bind(readw, _1);
    auto ww = bind(writew, _1, _2);
    auto rl = bind(readl, _1);
    auto wl = bind(writel, _1, _2);

    m68kcpu.setrwbcallback(rb, wb);
    m68kcpu.setrwwcallback(rw, ww);
    m68kcpu.setrwlcallback(rl, wl);
}

bool loadROM(string filename, uint8_t *array)
{
    ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);
    
    if (file.is_open())
    {
        streampos size = file.tellg();
        
        file.seekg(0, ios::beg);
        file.read((char*)&array[0], size);
        
        cout << filename << " succefully loaded." << endl;
        file.close();
        return true;
    }
    else
    {
        cout << "Error: " << filename << " could not be opened." << endl;
        return false;
    }
}

bool loadfiles(string file1, string file2)
{
    
    if (!loadROM(file1, arr1))
    {
        return false;
    }
    
    if (!loadROM(file2, arr2))
    {
        return false;
    }
    
    for (int i = 0; i < 0x10000; i++)
    {
        memorymap[i * 2] = arr1[i];
        memorymap[i * 2 + 1] = arr2[i];
    }
    
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Usage: " << argv[0] << " ROM1 ROM2" << endl;
        return 1;
    }
    
    if (!loadfiles(argv[1], argv[2]))
    {
        return 1;
    }
    
    init();
    
    m68kcpu.reset(0);
    m68kcpu.execute(1000);
    m68kcpu.shutdown();
    
    return 0;
}
