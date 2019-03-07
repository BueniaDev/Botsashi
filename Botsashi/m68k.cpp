#include "m68k.h"
#include <iostream>
using namespace m68k;
using namespace std;

namespace m68k
{
    M68K::M68K()
    {

    }
    
    M68K::~M68K()
    {
        cout << "M68K::Shutting down..." << endl;
    }
    
    void M68K::reset()
    {
        for (int i = 0; i < 8; i++)
        {
            m68kreg.datareg[i] = 0xFFFFFFFF;
        }
        
        for (int i = 0; i < 7; i++)
        {
            m68kreg.addrreg[i] = 0xFFFFFFFF;
        }
        
        m68kreg.sp = readLong(0x000000);
        m68kreg.pc = readLong(0x000004);
        m68kreg.sr = 0x7FFF;
        mcycles = 0;
        
        cout << "M68K::Initialized" << endl;
    }
    
    void M68K::executenextm68kopcode()
    {
        uint16_t opcode = readWord(m68kreg.pc);
        m68kreg.pc += 2;
        executem68kopcode(opcode);
    }
    
    void M68K::unimplementedopcode(uint16_t opcode)
    {
        cout << "M68K::Unrecognized opcode at " << tohexstring(opcode) << endl;
        exit(1);
    }
}
