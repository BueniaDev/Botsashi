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
            
            struct m68kregisters
            {
                uint32_t datareg[8] = {0};
                uint32_t addrreg[7] = {0};
                uint32_t sp;
                uint32_t pc;
                uint16_t sr;
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
            
            void executenextm68kopcode();
            void executem68kopcode(uint16_t opcode);
            void unimplementedopcode(uint16_t opcode);
            string getm68kmnemonic(uint32_t addr);
            
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
