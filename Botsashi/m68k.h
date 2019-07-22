// Botsashi
// (C) Buenia 2019
// Licensed under the GNU GPL v3
// See LICENSE for full license text

// File: m68k.h
// Date: March 7, 2019
// Description: Emulates the Motorola 68000 in software

#ifndef GEN_M68K
#define GEN_M68K

#include "utils.h"
#include <cstdint>
#include <functional>
using namespace std;

namespace m68k
{
    using m68kreadb = function<uint8_t(uint32_t)>;
    using m68kwriteb = function<void(uint32_t, uint8_t)>;
    using m68kreadw = function<uint16_t(uint32_t)>;
    using m68kwritew = function<void(uint32_t, uint16_t)>;
    using m68kreadl = function<uint32_t(uint32_t)>;
    using m68kwritel = function<void(uint32_t, uint32_t)>;
    
    struct M68K
    {    
        void reset(uint32_t val);
	void shutdown();
            
        struct m68kregisters
        {
            uint32_t datareg[8] = {0};
            uint32_t addrreg[8] = {0};
            uint32_t ssp;
            uint32_t usp;
            uint32_t pc;
            uint16_t sr;
            uint32_t vbr;
            uint32_t getsp();
            void setsp(uint32_t value);
        };
            
        struct conditioncodes
        {
            int carry = 0;
            int overflow = 1;
            int zero = 2;
            int negative = 3;
            int extended = 4;
        };
            
        m68kregisters m68kreg;
        conditioncodes condition;
        uint32_t tempaddr = 0;
        int mcycles = 0;

	inline int getmovescycletimings(int opmode, int opreg, bool dword)
	{
	    int temp = 0;	    

	    switch (opmode)
	    {
		case 0: temp = 4; break;
		case 1: temp = 4; break;
		case 2: temp = (dword) ? 12 : 8; break;
		case 3: temp = (dword) ? 12 : 8; break;
		case 4: temp = (dword) ? 14 : 10; break;
		case 5: temp = 0; break;
		case 6: temp = 0; break;
		case 7:
		{
		    switch (opreg)
		    {
			case 0: temp = (dword) ? 16 : 12; break;
			case 1: temp = (dword) ? 20 : 16; break;
			case 2: temp = 0; break;
			case 3: temp = 0; break;
			case 4: temp = (dword) ? 12 : 8; break;
		    }
		}
		break;
	    }

	    return temp;
	}

	inline int getmovedcycletimings(int opmode, int opreg, bool dword)
	{
	    int temp = 0;	    

	    switch (opmode)
	    {
		case 0: temp = 0; break;
		case 1: temp = 0; break;
		case 2: temp = (dword) ? 8 : 4; break;
		case 3: temp = (dword) ? 8 : 4; break;
		case 4: temp = (dword) ? 8 : 4; break;
		case 5: temp = 0; break;
		case 6: temp = 0; break;
		case 7:
		{
		    switch (opreg)
		    {
			case 0: temp = (dword) ? 12 : 8; break;
			case 1: temp = (dword) ? 16 : 12; break;
		    }
		}
		break;
	    }

	    return temp;
	}

	inline void setdestreg(int opmode, int opreg, uint32_t source, uint16_t opcode, int length)
	{

	    bool byte = false;
	    bool word = false;
	    bool dword = false;

	    switch (length)
	    {
		case 1: byte = true; word = dword = false; break;
		case 2: word = true; byte = dword = false; break;
		case 3: dword = true; byte = word = false; break;
	    }

	    switch (opmode)
	    {
		case 0: 
		{
		    uint32_t temp = m68kreg.datareg[opreg]; 
		    if (byte)
		    {
			m68kreg.datareg[opreg] = ((temp & ~0xFF) | (source & 0xFF));
		    }
		    else if (word)
		    {
			m68kreg.datareg[opreg] = ((temp & ~0xFFFF) | (source & 0xFFFF));
		    }
		    else if (dword)
		    {
			m68kreg.datareg[opreg] = source;
		    }
		}		
		break;
		case 1: 
		{
		    uint32_t temp = m68kreg.addrreg[opreg]; 
		    if (byte)
		    {
			m68kreg.addrreg[opreg] = ((temp & ~0xFF) | (source & 0xFF));
		    }
		    else if (word)
		    {
			m68kreg.addrreg[opreg] = ((temp & ~0xFFFF) | (source & 0xFFFF));
		    }
		    else if (dword)
		    {
			m68kreg.addrreg[opreg] = source;
		    }
		}
		break;
		case 2:
		{
		    if (byte)
		    {
			writeByte(m68kreg.addrreg[opreg], (source & 0xFF));
		    }
		    else if (word)
		    {
			writeWord(m68kreg.addrreg[opreg], (source & 0xFFFF));
		    }
		    else if (dword)
		    {
			writeLong(m68kreg.addrreg[opreg], source);
		    }
		}
		break;
		case 3:
		{
		    if (byte)
		    {
			writeByte(m68kreg.addrreg[opreg], (source & 0xFF));
			m68kreg.addrreg[opreg] += 1;
		    }
		    else if (word)
		    {
			writeWord(m68kreg.addrreg[opreg], (source & 0xFFFF));
			m68kreg.addrreg[opreg] += 2;
		    }
		    else if (dword)
		    {
			writeLong(m68kreg.addrreg[opreg], source);
			m68kreg.addrreg[opreg] += 4;
		    }
		}
		break;
		case 4:
		{
		    if (byte)
		    {
			m68kreg.addrreg[opreg] -= 1;
			writeByte(m68kreg.addrreg[opreg], (source & 0xFF));
		    }
		    else if (word)
		    {
			m68kreg.addrreg[opreg] -= 2;			
			writeWord(m68kreg.addrreg[opreg], (source & 0xFFFF));
		    }
		    else if (dword)
		    {
			m68kreg.addrreg[opreg] -= 4;			
			writeLong(m68kreg.addrreg[opreg], source);
		    }
		}
		break;
		case 5: unimplementedopcode(opcode); break;
		case 6: unimplementedopcode(opcode); break;
		case 7:
		{
		    switch (opreg)
		    {
			case 0:
			{
			    int16_t wordtemp = (int16_t)(readWord(m68kreg.pc));
			    m68kreg.pc += 2;

			    uint32_t readtemp = (uint32_t)(wordtemp);

			    if (byte)
			    {
				writeByte(readtemp, (source & 0xFF));
			    }
			    else if (word)
			    {
				writeWord(readtemp, (source & 0xFFFF));
			    }
			    else if (dword)
			    {
				writeLong(readtemp, source);
			    }
			}
			break;
			case 1:
			{
			    uint32_t readtemp = readLong(m68kreg.pc);
			    m68kreg.pc += 4;

			    if (byte)
			    {
				writeByte(readtemp, (source & 0xFF));
			    }
			    else if (word)
			    {
				writeWord(readtemp, (source & 0xFFFF));
			    }
			    else if (dword)
			    {
				writeLong(readtemp, source);
			    }
			}
			break;
		    }
		}
		break;
	    }
	}

	inline uint32_t getsourcereg(int opmode, int opreg, uint16_t opcode, int length)
	{
	    uint32_t temp = 0;

	    bool byte = false;
	    bool word = false;
	    bool dword = false;

	    switch (length)
	    {
		case 1: byte = true; word = dword = false; break;
		case 2: word = true; byte = dword = false; break;
		case 3: dword = true; byte = word = false; break;
	    }

	    switch (opmode)
	    {
		case 0: temp = m68kreg.datareg[opreg]; break;
		case 1: temp = m68kreg.addrreg[opreg]; break;
		case 2:
		{
		    if (byte)
		    {
			temp = readByte(m68kreg.addrreg[opreg]);
		    }
		    else if (word)
		    {
			temp = readWord(m68kreg.addrreg[opreg]);
		    }
		    else if (dword)
		    {
			temp = readLong(m68kreg.addrreg[opreg]);
		    }
		}
		break;
		case 3:
		{
		    if (byte)
		    {
			temp = readByte(m68kreg.addrreg[opreg]);
			m68kreg.addrreg[opreg] += 1;
		    }
		    else if (word)
		    {
			temp = readWord(m68kreg.addrreg[opreg]);
			m68kreg.addrreg[opreg] += 2;
		    }
		    else if (dword)
		    {
			temp = readLong(m68kreg.addrreg[opreg]);
			m68kreg.addrreg[opreg] += 4;
		    }
		}
		break;
		case 4:
		{
		    if (byte)
		    {
			m68kreg.addrreg[opreg] -= 1;
			temp = readByte(m68kreg.addrreg[opreg]);
		    }
		    else if (word)
		    {
			m68kreg.addrreg[opreg] -= 2;			
			temp = readWord(m68kreg.addrreg[opreg]);
		    }
		    else if (dword)
		    {
			m68kreg.addrreg[opreg] -= 4;			
			temp = readLong(m68kreg.addrreg[opreg]);
		    }
		}
		break;
		case 5: unimplementedopcode(opcode); break;
		case 6: unimplementedopcode(opcode); break;
		case 7:
		{
		    switch (opreg)
		    {
			case 0:
			{
			    int16_t wordtemp = (int16_t)(readWord(m68kreg.pc));
			    m68kreg.pc += 2;

			    uint32_t readtemp = (uint32_t)(wordtemp);

			    if (byte)
			    {
				temp = readByte(readtemp);
			    }
			    else if (word)
			    {
				temp = readWord(readtemp);
			    }
			    else if (dword)
			    {
				temp = readLong(readtemp);
			    }
			}
			break;
			case 1:
			{
			    uint32_t readtemp = readLong(m68kreg.pc);
			    m68kreg.pc += 4;

			    if (byte)
			    {
				temp = readByte(readtemp);
			    }
			    else if (word)
			    {
				temp = readWord(readtemp);
			    }
			    else if (dword)
			    {
				temp = readLong(readtemp);
			    }
			}
			break;
			case 2: unimplementedopcode(opcode); break;
			case 3: unimplementedopcode(opcode); break;
			case 4:
			{
			    if (byte)
			    {
				temp = readByte(m68kreg.pc);
				m68kreg.pc += 1;
			    }
			    else if (word)
			    {
				temp = readWord(m68kreg.pc);
				m68kreg.pc += 2;
			    }
			    else if (dword)
			    {
				temp = readLong(m68kreg.pc);
				m68kreg.pc += 4;
			    }
			}
			break;
		    }
		}
		break;
	    }

	    return temp;
	}

	inline bool getcond(int cond)
	{
	    bool temp = false;	    

	    switch (cond)
	    {
		case 0: temp = true; break; // T
		case 1: temp = false; break; // F
		case 2: temp = (!iscarry() && !iszero()); break; // HI
		case 3: temp = (iscarry() || iszero()); break; // LS
		case 4: temp = !iscarry(); break; // CC
		case 5: temp = iscarry(); break; // CS
		case 6: temp = !iszero(); break; // NE
		case 7: temp = iszero(); break; // EQ
		case 8: temp = !isoverflow(); break; // VC
		case 9: temp = isoverflow(); break; // VS
		case 10: temp = !isnegative(); break; // PL
		case 11: temp = isnegative(); break; // MI
		case 12: temp = (isnegative() == isoverflow()); break; // GE
		case 13: temp = (isnegative() != isoverflow()); break; // LT
		case 14: temp = (!iszero() && (isnegative() == isoverflow())); break; // GT
		case 15: temp = (!iszero() || (isnegative() != isoverflow())); break; // LE
	    }

	    return temp;
	}

	inline int getmsb(int length)
	{
	    return ((2 << (1 + length)) - 1);
	}

	inline bool sign(uint32_t operand, int length)
	{
	    return TestBit(operand, getmsb(length));
	}

	inline bool overflow(uint32_t op1, uint32_t op2, uint32_t op3, int length)
	{
	    int bit = getmsb(length);
	    if (!TestBit(op1, bit) && !TestBit(op2, bit) && TestBit(op3, bit))
	    {
		return true;
	    }
	    else if (TestBit(op1, bit) && TestBit(op2, bit) && !TestBit(op3, bit))
	    {
		return true;
	    }
	    else
	    {
		return false;
	    }
	}

	inline bool isextended()
	{
	    return TestBit(m68kreg.sr, 4);
	}

	inline void setextended(bool val)
	{
	    m68kreg.sr = (val) ? BitSet(m68kreg.sr, 4) : BitReset(m68kreg.sr, 4);
	}

	inline bool isnegative()
	{
	    return TestBit(m68kreg.sr, 3);
	}

	inline void setnegative(bool val)
	{
	    m68kreg.sr = (val) ? BitSet(m68kreg.sr, 3) : BitReset(m68kreg.sr, 3);
	}

	inline bool iszero()
	{
	    return TestBit(m68kreg.sr, 2);
	}

	inline void setzero(bool val)
	{
	    m68kreg.sr = (val) ? BitSet(m68kreg.sr, 2) : BitReset(m68kreg.sr, 2);
	}

	inline bool isoverflow()
	{
	    return TestBit(m68kreg.sr, 1);
	}

	inline void setoverflow(bool val)
	{
	    m68kreg.sr = (val) ? BitSet(m68kreg.sr, 1) : BitReset(m68kreg.sr, 1);
	}

	inline bool iscarry()
	{
	    return TestBit(m68kreg.sr, 0);
	}

	inline void setcarry(bool val)
	{
	    m68kreg.sr = (val) ? BitSet(m68kreg.sr, 0) : BitReset(m68kreg.sr, 0);
	}
            
	int execute(int cycles);
        void executenextm68kopcode();
        void executem68kopcode(uint16_t opcode);
        void unimplementedopcode(uint16_t opcode);
        string getm68kmnemonic(uint32_t addr);
            
        bool interruptsanitycheck(int value);
        int getinterruptmask();
        void setinterruptmask(int value);
        void requestinterrupt(int interruptlevel);
        void checkinterrupts(int interruptlevel);
        void exceptioninterrupt(int interruptlevel);
            
        string mnemonic = "";
        string opsource = "";
        string opdest = "";

        m68kreadb readByte;
        m68kwriteb writeByte;
        m68kreadw readWord;
        m68kwritew writeWord;
        m68kreadl readLong;
        m68kwritel writeLong;
        
        void setrwbcallback(m68kreadb rb, m68kwriteb wb)
        {
            readByte = rb;
            writeByte = wb;
        }
        
        void setrwwcallback(m68kreadw rw, m68kwritew ww)
        {
            readWord = rw;
            writeWord = ww;
        }
        
        void setrwlcallback(m68kreadl rl, m68kwritel wl)
        {
            readLong = rl;
            writeLong = wl;
        }
    };
}

#endif // GEN_M68K
