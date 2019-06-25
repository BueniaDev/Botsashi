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
