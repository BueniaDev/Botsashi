// Botsashi
// (C) Buenia 2019
// Licensed under the GNU GPL v3
// See LICENSE for full license text

// File: m68k.cpp
// Date: March 7, 2019
// Description: Emulates the Motorola 68000 in software

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

    }
    
    void M68K::reset()
    {
        for (int i = 0; i < 8; i++)
        {
            m68kreg.datareg[i] = 0;
        }
        
        for (int i = 0; i < 7; i++)
        {
            m68kreg.addrreg[i] = 0;
        }
        
        m68kreg.ssp = m68kreg.addrreg[7] = readLong(0x000000);
        m68kreg.pc = readLong(0x000004);
        m68kreg.sr = 0x7FFF;
        mcycles = 0;
        
        cout << "M68K::Initialized" << endl;
    }

    void M68K::shutdown()
    {
        cout << "M68K::Shutting down..." << endl;
    }
    
    uint32_t M68K::m68kregisters::getsp()
    {
        return addrreg[7];
    }
    
    void M68K::m68kregisters::setsp(uint32_t value)
    {
        if (TestBit(sr, 13))
        {
            ssp = value;
	    addrreg[7] = ssp;
        }
        else
        {
            usp = value;
	    addrreg[7] = usp;
        }
    }

    int M68K::execute(int cycles)
    {
	int executecycles = cycles;

	while (executecycles > 0)
	{
	    int corecycles = mcycles;
	    executenextm68kopcode();
	    int remcycles = mcycles - corecycles;
	    executecycles -= remcycles;
	}

	return 0;
    }
    
    void M68K::executenextm68kopcode()
    {
        uint16_t opcode = readWord(m68kreg.pc);
        m68kreg.pc += 2;
        executem68kopcode(opcode);
    }
    
    bool M68K::interruptsanitycheck(int value)
    {
        if ((value < 0) && (value > 7))
        {
	       cout << "Error: Unrecognized interrupt mask value" << endl;
	       return false;
        }

        return true;
    }

    int M68K::getinterruptmask()
    {
        return ((m68kreg.sr & 0x0700) >> 8);
    }

    void M68K::setinterruptmask(int value)
    {
        if (!interruptsanitycheck(value))
        {
	       return;
        }

        uint16_t temp = m68kreg.sr;
        temp = ((temp & 0xF800) | (value << 8) | (m68kreg.sr & 0xFF));
        m68kreg.sr = temp;
    }
    
    void M68K::requestinterrupt(int interruptlevel)
    {
        if (!interruptsanitycheck((interruptlevel - 1)))
        {
            return;
        }

        int oldlevel = getinterruptmask();
    
        if ((oldlevel != 7) && (interruptlevel == 7))
        {
            exceptioninterrupt(7);
        }
        else
        {
            checkinterrupts(interruptlevel);
        }
        
        setinterruptmask(interruptlevel);
    }

    void M68K::checkinterrupts(int interruptlevel)
    {
        if (interruptlevel >= getinterruptmask())
        {
            exceptioninterrupt(interruptlevel);
        }
    }

    void M68K::exceptioninterrupt(int interruptlevel)
    {
	m68kreg.setsp(m68kreg.getsp() - 4);
        writeWord(m68kreg.getsp(), m68kreg.pc);
	m68kreg.setsp(m68kreg.getsp() - 2);
        writeWord(m68kreg.getsp(), m68kreg.sr);

        m68kreg.pc = readLong((0x64 + ((interruptlevel - 1) * 4)));
        mcycles += 44;
    }

    void M68K::unimplementedopcode(uint16_t opcode)
    {
        cout << "M68K::Unrecognized opcode at " << tohexstring(opcode) << endl;
        exit(1);
    }
}
