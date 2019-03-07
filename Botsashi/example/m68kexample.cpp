#include "../m68k.h"
#include <iostream>
using namespace m68k;
using namespace std;

M68K m68kcpu;

uint8_t memorymap[0x1000000];

uint8_t M68K::readByte(uint32_t address)
{
    address = (address % 0x1000000);

    return memorymap[address];
}

void M68K::writeByte(uint32_t address, uint8_t value)
{
    address = (address % 0x1000000);    
 
    memorymap[address] = value;
}

uint16_t M68K::readWord(uint32_t address)
{
    address = (address % 0x1000000);
    
    return (readByte(address) << 8) | readByte(address + 1);
}
    
void M68K::writeWord(uint32_t address, uint16_t value)
{
    address = (address % 0x1000000);
        
    writeByte((address + 1), (value & 0xFF));
    writeByte(address, ((value >> 8) & 0xFF));
}
    
uint32_t M68K::readLong(uint32_t address)
{
    address = (address % 0x1000000);
        
    return (readWord(address) << 16) | readWord(address + 2);
}
    
void M68K::writeLong(uint32_t address, uint32_t value)
{
    address = (address % 0x1000000);
        
    writeWord(address, (value >> 16) & 0xFFFF);
    writeWord(address + 2, (value & 0xFFFF));
}

int main()
{
    m68kcpu.writeLong(0x000000, 0x0000FFFE);
    m68kcpu.writeLong(0x000004, 0x00000100);
    m68kcpu.writeWord(0x000100, 0x1039);
    m68kcpu.writeLong(0x000102, 0xA1000001);
    m68kcpu.writeWord(0x000106, 0x60F8);

    m68kcpu.reset();

    bool quit = false;

    while (!quit)
    {
	m68kcpu.executenextm68kopcode();

	if (m68kcpu.mcycles >= 1024)
	{
	    cout << "Total cycles taken: " << m68kcpu.mcycles << endl;
	    quit = true;
	}
    }

    return 0;
}
