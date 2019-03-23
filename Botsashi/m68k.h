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
using namespace std;

namespace m68k
{
    class M68K
    {
        public:
            M68K();
            ~M68K();
            
            void reset();
	    void shutdown();
            
            struct m68kregisters
            {
                uint32_t datareg[8] = {0};
                uint32_t addrreg[8] = {0};
                uint32_t ssp;
                uint32_t usp;
                uint32_t pc;
                uint16_t sr;
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

            uint8_t readByte(uint32_t address);
            void writeByte(uint32_t address, uint8_t value);
            uint16_t readWord(uint32_t address);
            void writeWord(uint32_t address, uint16_t value);
            uint32_t readLong(uint32_t address);
            void writeLong(uint32_t address, uint32_t value);
    };
}


#endif // GEN_M68K
