#include "utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

bool TestBit(uint8_t reg, int bit)
{
    return (reg & (1 << bit)) ? true : false;
}

uint8_t BitSet(uint8_t reg, int bit)
{
    return (reg | (1 << bit));
}

uint8_t BitReset(uint8_t reg, int bit)
{
    return (reg & ~(1 << bit));
}

int BitGetVal(uint8_t reg, int bit)
{
    return (reg & (1 << bit)) ? 1 : 0;
}

uint8_t BitSetTo(uint8_t reg, int bit, bool set)
{
    return set ? (reg | (1 << bit)) : (reg & ~(1 << bit));
}

uint16_t opcodeencode(uint16_t opcode)
{
    return (opcode & 0xF000);
}
            
uint16_t opcodecondition(uint16_t opcode)
{
    return ((opcode & 0x0F00) >> 8);
}
            
uint16_t opcodedestregister(uint16_t opcode)
{
    return ((opcode & 0x0E00) >> 9);
}
            
uint16_t opcodedestmode(uint16_t opcode)
{
    return ((opcode & 0x01C0) >> 6);
}
            
uint16_t opcodebit8(uint16_t opcode)
{
    return ((opcode & 0x0100) >> 8);
}
            
uint16_t opcodesize(uint16_t opcode)
{
    return ((opcode & 0xC0) >> 6);
}
            
uint16_t opcodesourcemode(uint16_t opcode)
{
    return ((opcode & 0x38) >> 3);
}

uint16_t opcodemiscencode(uint16_t opcode)
{
    return ((opcode & 0x30) >> 4);
}
            
uint16_t opcodesourceregister(uint16_t opcode)
{
    return (opcode & 0x7);
}

string tohexstring(uint32_t val)
{
    stringstream temp;
    if (val < 0x10) 
    { 
        temp << "0x0" << hex << uppercase << val; 
    }
    else if ((val < 0x1000) && (val >= 0x100)) 
    { 
        temp << "0x0" << hex << uppercase << val; 
    }
    else if ((val < 0x100000) && (val >= 0x10000)) 
    { 
        temp << "0x0" << hex << uppercase << val; 
    }
    else if ((val < 0x10000000) && (val >= 0x1000000)) 
    { 
        temp << "0x0" << hex << uppercase << val; 
    }
    else 
    { 
        temp << "0x" << hex << uppercase << val; 
    }
    
    return temp.str();
}

string tostring(uint32_t val)
{
    stringstream temp;
    temp << uppercase << val;
    return temp.str();
}
