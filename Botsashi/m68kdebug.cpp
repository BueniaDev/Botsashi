// Botsashi
// (C) Buenia 2019
// Licensed under the GNU GPL v3
// See LICENSE for full license text

// File: m68kdebug.cpp
// Date: March 7, 2019
// Description: Provides debug mnemnoics for Motorola 68000 opcodes

#include "m68k.h"
#include <iostream>
using namespace m68k;
using namespace std;

string M68K::getm68kmnemonic(uint32_t addr)
{
    uint16_t opcode = readWord(addr);
    
    string mnemonic = "";
    string opsource = "";
    string opdest = "";
    
    // Bits 15-12 determine the opcode
    switch (opcodeencode(opcode))
    {
        // Bit manipulation / MOVEP / immediate
        case 0x0000:
        {
            switch (opcodebit8(opcode)) // Determines type of opcode
            {
                case 0:
                {
                    switch (opcodedestregister(opcode)) // Determines immediate opcode
                    {
                        case 0: mnemonic = "NAN"; break; // Or immediate opcodes
                        case 1:
                        {
                            switch (opcodesize(opcode)) // Determines size of operand
                            {
                                case 0:
                                {
                                    switch (opcodesourcemode(opcode)) // Determines effective address mode
                                    {
                                        case 0:
                                        {
                                            uint8_t temp = (readWord(addr + 2) & 0xFF);   
                                            mnemonic = "andi.b #$" + tohexstring(temp) + ",d" + tostring(opcodesourceregister(opcode));
                                        }
                                        break; // Data register
                                        case 2: mnemonic = "NAN"; break; // Address
                                        case 3: mnemonic = "NAN"; break; // Address with post-increment
                                        case 4: mnemonic = "NAN"; break; // Address with pre-decrement
                                        case 5: mnemonic = "NAN"; break; // Address with displacement
                                        case 6: mnemonic = "NAN"; break; // Address with index
                                        case 7:
                                        {
                                            switch (opcodedestregister(opcode))
                                            {
                                                case 0: mnemonic = "NAN"; break; // Absolute short
                                                case 1: mnemonic = "NAN"; break; // Absolute long
                                                case 4: mnemonic = "NAN"; break; // And immediate with condition code register
                                            }
                                        }
                                        break; // Addressing mode 7
                                    }
                                }
                                break; // Size is byte
                                case 1: mnemonic = "NAN"; break; // Size is word
                                case 2: mnemonic = "NAN"; break; // Size is long
                            }
                        }
                        break; // And immediate opcodes
                        case 2: mnemonic = "NAN"; break; // Sub immediate
                        case 3: mnemonic = "NAN"; break; // Add immediate
                        case 4: mnemonic = "NAN"; break; // Immediate bit manipulation opcodes
                        case 5: mnemonic = "NAN"; break; // Xor immeidate opcodes
                        case 6: 
                        {
                            switch (opcodesize(opcode))
                            {
                                case 0: mnemonic = "NAN"; break; // Size is byte
                                case 1:
                                {
                                    switch (opcodesourcemode(opcode))
                                    {
                                        case 0:
                                        {
                                            mnemonic = "cmpi.w #$" + tohexstring(readWord(addr + 2)) + ", d" + tostring(opcodesourceregister(opcode));
                                        }
                                        break; // Data register
                                        case 2: mnemonic = "NAN"; break; // Address
                                        case 3: mnemonic = "NAN"; break; // Address with post-increment
                                        case 4: mnemonic = "NAN"; break; // Address with pre-decrement
                                        case 5: mnemonic = "NAN"; break; // Address with displacement
                                        case 6: mnemonic = "NAN"; break; // Address with index
                                        case 7:
                                        {
                                            switch (opcodesourceregister(opcode))
                                            {
                                                case 0: mnemonic = "NAN"; break; // Absolute word
                                                case 1: mnemonic = "NAN"; break; // Absolute long
                                                case 2: mnemonic = "NAN"; break; // PC with displacement
                                                case 3: mnemonic = "NAN"; break; // PC with index
                                            }
                                        }
                                        break; // Addressing mode 7
                                    }
                                }
                                break; // Size is word
                                case 2: mnemonic = "NAN"; break; // Size is long
                            }
                        }
                        break; // Compare immediate
                    }
                }
                break; // Immediate
                case 1: mnemonic = "NAN"; break; // Bit manipulation / MOVEP
            }
        }
        break;
            
        // Move byte
        case 0x1000:
        {
            switch (opcodesourcemode(opcode)) // Determines source mode
            {
                case 0: opsource = "d" + tostring(opcodesourceregister(opcode)); break; // Data register
                case 1: opsource = "a" + tostring(opcodesourceregister(opcode)); break; // Address register
                case 2: opsource = "(a" + tostring(opcodesourceregister(opcode)) + ")"; break; // Address
                case 3: opsource = "(a" + tostring(opcodesourceregister(opcode)) + ")+"; break; // Address with post-increment
                case 4: opsource = "-(a" + tostring(opcodesourceregister(opcode)) + ")"; break; // Address with pre-decrement
                case 5: mnemonic = "NAN"; break; // Address with displacement
                case 6: mnemonic = "NAN"; break; // Address with index
                case 7:
                {
                    switch (opcodesourceregister(opcode)) // Determines source register
                    {
                        case 0:
                        {
                            uint16_t wordtemp = readWord(addr);
                                
                            int16_t bytetemp = (int16_t)(wordtemp);
                            uint32_t temp = 0;
                                
                            if (bytetemp < 0)
                            {
                                temp = 0xFFFFFFFF;
                            }
                            
                            temp &= 0xFFFF0000;
                            temp |= wordtemp;
                            
                            opsource = "$" + tohexstring(temp);
                        }
                        break; // Absolute word
                        case 1: opsource = "$" + tohexstring(readLong(addr + 2)); break; // Absolute long
                        case 2: mnemonic = "NAN"; break; // PC with displacement
                        case 3: mnemonic = "NAN"; break; // PC with index
                        case 4: opsource = "#$" + tohexstring(readLong(addr)); break; // Immediate
                    }
                }
                break; // Addressing mode 7
                default: mnemonic = "NAN"; break;
            }
                
                
            switch (opcodedestmode(opcode)) // Determines destination mode
            {
                case 0:
                {
                    opdest = "d" + tostring(opcodedestregister(opcode));
                    mnemonic = "move.b " + opsource + "," + opdest;
                }
                break; // Data register
                case 1:
                {
                    opdest = "a" + tostring(opcodedestregister(opcode));
                    mnemonic = "move.b " + opsource + "," + opdest;
                }
                break; // Address register
                case 2: 
                {
                    opdest = "(a" + tostring(opcodedestregister(opcode)) + ")";
                    mnemonic = "move.b " + opsource + "," + opdest;
                }
                break; // Address
                case 3: 
                {
                    opdest = "(a" + tostring(opcodedestregister(opcode)) + ")+";
                    mnemonic = "move.b " + opsource + "," + opdest;
                }
                break; // Address with post-increment
                case 4: 
                {
                    opdest = "-(a" + tostring(opcodedestregister(opcode)) + ")";
                    mnemonic = "move.b " + opsource + "," + opdest;
                }
                break; // Address with pre-decrement
                case 5: mnemonic = "NAN"; break; // Address with displacement
                case 6: mnemonic = "NAN"; break; // Address with index
                case 7:
                {
                    switch (opcodedestregister(opcode)) // Determines destination register
                    {
                        case 0:
                        {
                            uint16_t wordtemp = readWord(addr + 1);
                            addr += 2;
                                
                            int16_t bytetemp = (int16_t)(wordtemp);
                            uint32_t temp = 0;
                        
                            if (bytetemp < 0)
                            {
                                temp = 0xFFFFFFFF;
                            }
                                
                            temp &= 0xFFFF0000;
                            temp |= wordtemp;
                                
                            opdest = "$" + tohexstring(temp);
                            mnemonic = "move.b " + opsource + "," + opdest; 
                        }
                        break; // Absolute word
                        case 1: mnemonic = "NAN"; break; // Absolute long
                        default: mnemonic = "NAN"; break;
                    }
                }
                break; // Addressing mode 7
            }
        }
        break;

        // Move long
        case 0x2000:
        {
            switch (opcodesourcemode(opcode)) // Determines source mode
            {
                case 0: opsource = "d" + tostring(opcodesourceregister(opcode)); break; // Data register
                case 1: opsource = "a" + tostring(opcodesourceregister(opcode)); break; // Address register
                case 2: opsource = "(a" + tostring(opcodesourceregister(opcode)) + ")"; break; // Address
                case 3: opsource = "(a" + tostring(opcodesourceregister(opcode)) + ")+"; break; // Address with post-increment
                case 4: opsource = "-(a" + tostring(opcodesourceregister(opcode)) + ")"; break; // Address with pre-decrement
                case 5: mnemonic = "NAN"; break; // Address with displacement
                case 6: mnemonic = "NAN"; break; // Address with index
                case 7:
                {
                    switch (opcodesourceregister(opcode)) // Determines source register
                    {
                        case 0:
                        {
                            uint16_t wordtemp = readWord(addr);
                            addr += 2;
                                
                            int16_t bytetemp = (int16_t)(wordtemp);
                            uint32_t temp = 0;
                                
                            if (bytetemp < 0)
                            {
                                temp = 0xFFFFFFFF;
                            }
                            
                            temp &= 0xFFFF0000;
                            temp |= wordtemp;
                            
                            opsource = "$" + tohexstring(temp);
                        }
                        break; // Absolute word
                        case 1: opsource = "$" + tohexstring(readLong(addr)); break; // Absolute long
                        case 2: mnemonic = "NAN"; break; // PC with displacement
                        case 3: mnemonic = "NAN"; break; // PC with index
                        case 4: opsource = "#$" + tohexstring(readLong(addr + 2)); break; // Immediate
                    }
                }
                break; // Addressing mode 7
                default: mnemonic = "NAN"; break;
            }
                
                
            switch (opcodedestmode(opcode)) // Determines destination mode
            {
                case 0:
                {
                    opdest = "d" + tostring(opcodedestregister(opcode));
                    mnemonic = "move.l " + opsource + "," + opdest;
                }
                break; // Data register
                case 1:
                {
                    opdest = "a" + tostring(opcodedestregister(opcode));
                    mnemonic = "move.l " + opsource + "," + opdest;
                }
                break; // Address register
                case 2: 
                {
                    opdest = "(a" + tostring(opcodedestregister(opcode)) + ")";
                    mnemonic = "move.l " + opsource + "," + opdest;
                }
                break; // Address
                case 3: 
                {
                    opdest = "(a" + tostring(opcodedestregister(opcode)) + ")+";
                    mnemonic = "move.l " + opsource + "," + opdest;
                }
                break; // Address with post-increment
                case 4: 
                {
                    opdest = "-(a" + tostring(opcodedestregister(opcode)) + ")";
                    mnemonic = "move.l " + opsource + "," + opdest;
                }
                break; // Address with pre-decrement
                case 5: mnemonic = "NAN"; break; // Address with displacement
                case 6: mnemonic = "NAN"; break; // Address with index
                case 7:
                {
                    switch (opcodedestregister(opcode)) // Determines destination register
                    {
                        case 0: mnemonic = "NAN"; break; // Absolute word
                        case 1: mnemonic = "NAN"; break; // Absolute long
                        default: mnemonic = "NAN"; break;
                    }
                }
                break; // Addressing mode 7
            }
        }
        break;
            

        // Move word
        case 0x3000:
        {
            switch (opcodesourcemode(opcode)) // Determines source mode
            {
                case 0: opsource = "d" + tostring(opcodesourceregister(opcode)); break; // Data register
                case 1: opsource = "a" + tostring(opcodesourceregister(opcode)); break; // Address register
                case 2: opsource = "(a" + tostring(opcodesourceregister(opcode)) + ")"; break; // Address
                case 3: opsource = "(a" + tostring(opcodesourceregister(opcode)) + ")+"; break; // Address with post-increment
                case 4: opsource = "-(a" + tostring(opcodesourceregister(opcode)) + ")"; break; // Address with pre-decrement
                case 5: mnemonic = "NAN"; break; // Address with displacement
                case 6: mnemonic = "NAN"; break; // Address with index
                case 7:
                {
                    switch (opcodesourceregister(opcode)) // Determines source register
                    {
                        case 0:
                        {
                            uint16_t wordtemp = readWord(addr);
                            addr += 2;
                            
                            int16_t bytetemp = (int16_t)(wordtemp);
                            uint32_t temp = 0;
                                
                            if (bytetemp < 0)
                            {
                                temp = 0xFFFFFFFF;
                            }
                                
                            temp &= 0xFFFF0000;
                            temp |= wordtemp;
                            
                            opsource = "$" + tohexstring(temp);
                        } 
                        break; // Absolute word
                        case 1: opsource = "$" + tohexstring(readLong(addr)); break; // Absolute long
                        case 2: mnemonic = "NAN"; break; // PC with displacement
                        case 3: mnemonic = "NAN"; break; // PC with index
                        case 4: opsource = "#$" + tohexstring(readWord(addr + 2)); break; // Immediate
                    }
                }
                break; // Addressing mode 7
                default: mnemonic = "NAN"; break;
            }
                
                
            switch (opcodedestmode(opcode)) // Determines destination mode
            {
                case 0:
                {
                    opdest = "d" + tostring(opcodedestregister(opcode));
                    mnemonic = "move.w " + opsource + "," + opdest;
                }
                break; // Data register
                case 1:
                {
                    opdest = "a" + tostring(opcodedestregister(opcode));
                    mnemonic = "move.w " + opsource + "," + opdest; 
                }
                break; // Address register
                case 2: 
                {
                    opdest = "(a" + tostring(opcodedestregister(opcode)) + ")";
                    mnemonic = "move.w " + opsource + "," + opdest;
                }
                break; // Address
                case 3: 
                {
                    opdest = "(a" + tostring(opcodedestregister(opcode)) + ")+";
                    mnemonic = "move.w " + opsource + "," + opdest;
                }
                break; // Address with post-increment
                case 4: 
                {
                    opdest = "-(a" + tostring(opcodedestregister(opcode)) + ")";
                    mnemonic = "move.w " + opsource + "," + opdest;
                }
                break; // Address with pre-decrement
                case 5: mnemonic = "NAN"; break; // Address with displacement
                case 6: mnemonic = "NAN"; break; // Address with index
                case 7:
                {
                    switch (opcodedestregister(opcode)) // Determines destination register
                    {
                        case 0: mnemonic = "NAN"; break; // Absolute word
                        case 1: mnemonic = "NAN"; break; // Absolute long
                        default: break;
                    }
                }
                break; // Addressing mode 7
            }
        }
        break;
            
        // Misc opcodes
        case 0x4000:
        {
            switch (opcodedestmode(opcode))
            {
                case 6: mnemonic = "NAN"; break; // CHK
                case 7: mnemonic = "NAN"; break; // LEA
                default:
                {
                    switch (opcodedestregister(opcode))
                    {
                        case 0: mnemonic = "NAN"; break; // MOVE from SR / NEGX
                        case 1: mnemonic = "NAN"; break; // CLR
                        case 2: mnemonic = "NAN"; break; // MOVE to CCR / NEG
                        case 3: mnemonic = "NAN"; break; // MOVE to SR / NOT
                        case 4: mnemonic = "NAN"; break; // EXT / NBCD / SWAP / PEA / MOVEM (reg-to-mem)
                        case 5: mnemonic = "NAN"; break; // ILLEGAL / TAS / TST
                        case 6: mnemonic = "NAN"; break; // MOVEM (mem-to-reg)
                        case 7:
                        {
                            switch (opcodedestmode(opcode))
                            {
                                case 1: 
                                {
                                    switch (opcodemiscencode(opcode))
                                    {
                                        case 0: mnemonic = "NAN"; break; // TRAP
                                        case 1: mnemonic = "NAN"; break; // LINK / UNLK
                                        case 2: mnemonic = "NAN"; break; // MOVE USP
                                        case 3:
                                        {
                                            switch (opcode)
                                            {
                                                case 0x4E70: mnemonic = "NAN"; break; // RESET
                                                case 0x4E71: mnemonic = "NAN"; break; // NOP
                                                case 0x4E72: mnemonic = "NAN"; break; // STOP
                                                case 0x4E73: mnemonic = "NAN"; break; // RTE
                                                case 0x4E75: 
                                                {
                                                    mnemonic = "rts";
                                                }
                                                break; // RTS
                                                case 0x4E76: mnemonic = "NAN"; break; // TRAPV
                                                case 0x4E77: mnemonic = "NAN"; break; // RTR
                                            }
                                        }
                                        break; // Remaining misc opcodes
                                    }
                                }
                                break;
                                case 2: mnemonic = "NAN"; break; // JSR
                                case 3: mnemonic = "NAN"; break; // JMP
                            }
                        }
                        break; // Remaining misc opcodes
                    }
                }
                break;
            }
        }
        break;
        
        // ADDQ / SUBQ / Scc / DBcc
        case 0x5000:
        {
            switch (opcodesize(opcode))
            {
                case 0: mnemonic = "NAN"; break; // ADDQ.B / SUBQ.B
                case 1: mnemonic = "NAN"; break; // ADDQ.W / SUBQ.W
                case 2: 
                {
                    switch (opcodebit8(opcode))
                    {
                        case 0:
                        {
                            int adder = 0;
                            switch (opcodedestregister(opcode)) // Determines immediate data
                            {
                                case 0: adder = 8; break; // Immediate value 0
                                default: adder = (int)opcodedestregister(opcode); break; // Immediate values 1-7
                            }
                        
                            switch (opcodesourcemode(opcode)) // Determines destination register
                            {
                                case 0:
                                {
                                    mnemonic = "addq.l #$0" + tostring(adder) + ", d" + tostring(opcodesourceregister(opcode));
                                }
                                break; // Data register
                                case 1:
                                {
                                    mnemonic = "addq.l #$0" + tostring(adder) + ", a" + tostring(opcodesourceregister(opcode));
                                }
                                break; // Address register
                                case 2: mnemonic = "NAN"; break; // Address
                                case 3: mnemonic = "NAN"; break; // Address with post-increment
                                case 4: mnemonic = "NAN"; break; // Address with pre-decrement
                                case 5: mnemonic = "NAN"; break; // Address with displacement
                                case 6: mnemonic = "NAN"; break; // Address with index
                                case 7:
                                {
                                    switch (opcodesourceregister(opcode))
                                    {
                                        case 0: mnemonic = "NAN"; break; // Absolute word
                                        case 1: mnemonic = "NAN"; break; // Absolute long
                                        default: mnemonic = "NAN"; break;
                                    }
                                }
                                break; // Addressing mode 7
                                default: mnemonic = "NAN"; break;
                            }
                        }
                        break; // ADDQ.L
                        case 1: mnemonic = "NAN"; break; // SUBQ.L
                        default: mnemonic = "NAN"; break;
                    }
                }
                break; // ADDQ.L / SUBQ.L
                case 3:
                {
                    switch (opcodesourcemode(opcode))
                    {
                        case 1:
                        {
                            switch (opcodecondition(opcode))
                            {
                                case 0: mnemonic = "NAN"; break; // DBT
                                case 1:
                                {
                                    mnemonic = "dbf d" + tostring(opcodesourceregister(opcode)) + ", #$" + tohexstring(readWord(addr + 2));
                                }
                                break; // DBF
                                case 2: mnemonic = "NAN"; break; // DBHI
                                case 3: mnemonic = "NAN"; break; // DBLS
                                case 4: mnemonic = "NAN"; break; // DBCC
                                case 5: mnemonic = "NAN"; break; // DBCS
                                case 6: mnemonic = "NAN"; break; // DBNE
                                case 7: mnemonic = "NAN"; break; // DBEQ
                                case 8: mnemonic = "NAN"; break; // DBVC
                                case 9: mnemonic = "NAN"; break; // DBVS
                                case 10: mnemonic = "NAN"; break; // DBPL
                                case 11: mnemonic = "NAN"; break; // DBMI
                                case 12: mnemonic = "NAN"; break; // DBGE
                                case 13: mnemonic = "NAN"; break; // DBLT
                                case 14: mnemonic = "NAN"; break; // DBGT
                                case 15: mnemonic = "NAN"; break; // DBLE
                            }
                        }
                        break; // DBcc
                        default:
                        {
                            switch (opcodesourcemode(opcode))
                            {
                                case 0: mnemonic = "NAN"; break; // Data register
                                case 2: mnemonic = "NAN"; break; // Address
                                case 3: mnemonic = "NAN"; break; // Address with postincrement
                                case 4: mnemonic = "NAN"; break; // Address with predecrement
                                case 5: mnemonic = "NAN"; break; // Address with displacement
                                case 6: mnemonic = "NAN"; break; // Address with index
                                case 7:
                                {
                                    switch (opcodesourceregister(opcode))
                                    {
                                        case 0: mnemonic = "NAN"; break; // Absolute word
                                        case 1: mnemonic = "NAN"; break; // Absolute long
                                        default: mnemonic = "NAN"; break;
                                    }
                                }
                            }
                        }
                        break; // Scc
                    }
                }
            }
        }
        break;
            
        // Bcc / BSR / BSA
        case 0x6000:
        {
            switch (opcodecondition(opcode))
            {
                case 0: 
                {
                    uint8_t distemp = (opcode & 0xFF);
                    uint32_t pctemp = 0;
                        
                    if (distemp == 0)
                    {
                        uint16_t wordtemp = readWord(addr + 2);
                        pctemp = wordtemp;
                    }
                    else
                    {
                        pctemp = distemp;
                    }
                    
                    mnemonic = "bra #$" + tohexstring(pctemp);
                }
                break; // BRA
                case 1: 
                {
                    uint8_t distemp = (opcode & 0xFF);
                    uint32_t pctemp = 0;
                        
                    if (distemp == 0)
                    {
                        uint16_t wordtemp = readWord(addr + 2);
                        pctemp = wordtemp;
                    }
                    else
                    {
                        pctemp = distemp;
                    }
                    
                    mnemonic = "bsr #$" + tohexstring(pctemp);
                }
                break; // BSR
                case 2: mnemonic = "NAN"; break; // BHI
                case 3: mnemonic = "NAN"; break; // BLS
                case 4: mnemonic = "NAN"; break; // BCC
                case 5: mnemonic = "NAN"; break; // BCS
                case 6: mnemonic = "NAN"; break; // BNE
                case 7:
                {
                    uint8_t distemp = (opcode & 0xFF);
                    uint32_t pctemp = 0;
                        
                    if (distemp == 0)
                    {
                        uint16_t wordtemp = readWord(addr + 2);
                        pctemp = wordtemp;
                    }
                    else
                    {
                        pctemp = distemp;
                    }
                    
                    mnemonic = "beq #$" + tohexstring(pctemp);
                }
                break; // BEQ
                case 8: mnemonic = "NAN"; break; // BVC
                case 9: mnemonic = "NAN"; break; // BVS
                case 10: mnemonic = "NAN"; break; // BPL
                case 11: mnemonic = "NAN"; break; // BMI
                case 12: mnemonic = "NAN"; break; // BGE
                case 13: mnemonic = "NAN"; break; // BLT
                case 14: mnemonic = "NAN"; break; // BGT
                case 15: mnemonic = "NAN"; break; // BLE
            }
        }
        break;
            
        // MOVEQ
        case 0x7000:
        {
            uint8_t bytevalue = (opcode & 0xFF);
            uint32_t temp = (bytevalue << 24);
            temp |= (bytevalue << 16);
            temp |= (bytevalue << 8);
            temp |= bytevalue;
            opsource = "#$" + tohexstring(temp);
            opdest = "d" + tostring(opcodedestregister(opcode));
            mnemonic = "moveq #$" + tohexstring(temp) + ",d" + tostring(opcodedestregister(opcode));
        }
        break;
            
        default: mnemonic = "NAN"; break;
    }
        
    return mnemonic;
}
