// Botsashi
// (C) Buenia 2019
// Licensed under the GNU GPL v3
// See LICENSE for full license text

// File: m68kops.cpp
// Date: March 7, 2019
// Description: Emulates the Motorola 68000 opcodes

#include "m68k.h"
#include <iostream>
using namespace m68k;
using namespace std;

namespace m68k
{
    void M68K::executem68kopcode(uint16_t opcode)
    {
        // Bits 15-12 determine the opcode
        switch (opcodeencode(opcode))
        {
            // Bit manipulation / MOVEP / immediate
            case 0x0000:
            {
                bool andccr = false;
                
                uint32_t temp = 0;
                
                switch (opcodebit8(opcode)) // Determines type of opcode
                {
                    case 0:
                    {
                        switch (opcodedestregister(opcode)) // Determines immediate opcode
                        {
                            case 0: unimplementedopcode(opcode); break; // Or immediate opcodes
                            case 1:
                            {
                                switch (opcodesize(opcode)) // Determines size of operand
                                {
                                    case 0:
                                    {
					uint8_t source = 0;

					switch (opcodesourcemode(opcode)) // Determines effective address mode
                                        {
                                            case 0:
                                            {
                                                source = (m68kreg.datareg[opcodesourceregister(opcode)] & 0xFF);
                                                source &= readByte(m68kreg.pc + 1);
                                                m68kreg.pc += 2;
                                                
                                                temp = m68kreg.datareg[opcodesourceregister(opcode)];
                                                m68kreg.datareg[opcodesourceregister(opcode)] = ((temp & 0xFFFFFF00) | source);
                                                
                                                mcycles += 8;
                                            }
                                            break; // Data register
                                            case 2: unimplementedopcode(opcode); break; // Address
                                            case 3: unimplementedopcode(opcode); break; // Address with post-increment
                                            case 4: unimplementedopcode(opcode); break; // Address with pre-decrement
                                            case 5: unimplementedopcode(opcode); break; // Address with displacement
                                            case 6: unimplementedopcode(opcode); break; // Address with index
                                            case 7:
                                            {
                                                switch (opcodedestregister(opcode))
                                                {
                                                    case 0: unimplementedopcode(opcode); break; // Absolute short
                                                    case 1: unimplementedopcode(opcode); break; // Absolute long
                                                    case 4: 
                                                    {
                                                        andccr = true;
                                                    }
                                                    break; // And immediate with condition code register
                                                }
                                            }
                                            break; // Addressing mode 7
                                        }
                                        
                                        if (!andccr)
                                        {
					    setnegative(sign(source, 1));
					    setzero((source == 0));
					}
                                    }
                                    break; // Size is byte
                                    case 1: unimplementedopcode(opcode); break; // Size is word
                                    case 2: unimplementedopcode(opcode); break; // Size is long
                                }
                            }
                            break; // And immediate opcodes
                            case 2: unimplementedopcode(opcode); break; // Sub immediate
                            case 3: unimplementedopcode(opcode); break; // Add immediate
                            case 4: unimplementedopcode(opcode); break; // Immediate bit manipulation opcodes
                            case 5: unimplementedopcode(opcode); break; // Xor immeidate opcodes
                            case 6:
                            {
                                switch (opcodesize(opcode))
                                {
                                    case 0: unimplementedopcode(opcode); break; // Size is byte
                                    case 1:
                                    {
                                        uint16_t wordtemp = 0;
                                        uint16_t desttemp = 0;
                                        uint32_t temp = 0;
                                        switch (opcodesourcemode(opcode))
                                        {
                                            case 0:
                                            {
                                                wordtemp = readWord(m68kreg.pc);
                                                m68kreg.pc += 2;
                                                temp = m68kreg.datareg[opcodesourceregister(opcode)];
                                                desttemp = (temp & 0xFFFF);
                                                temp -= wordtemp;
                                                
                                                mcycles += 8;
                                            }
                                            break; // Data register
                                            case 2: unimplementedopcode(opcode); break; // Address
                                            case 3: unimplementedopcode(opcode); break; // Address with post-increment
                                            case 4: unimplementedopcode(opcode); break; // Address with pre-decrement
                                            case 5: unimplementedopcode(opcode); break; // Address with displacement
                                            case 6: unimplementedopcode(opcode); break; // Address with index
                                            case 7:
                                            {
                                                switch (opcodesourceregister(opcode))
                                                {
                                                    case 0: unimplementedopcode(opcode); break; // Absolute word
                                                    case 1: unimplementedopcode(opcode); break; // Absolute long
                                                    case 2: unimplementedopcode(opcode); break; // PC with displacement
                                                    case 3: unimplementedopcode(opcode); break; // PC with index
                                                }
                                            }
                                            break; // Addressing mode 7
                                        }
                                        
					setnegative(sign(temp, 2));
					setzero((temp == 0));
					setoverflow(overflow(desttemp, wordtemp, temp, 2));
				        setcarry((desttemp < wordtemp));
                                    }
                                    break; // Size is word
                                    case 2: unimplementedopcode(opcode); break; // Size is long
                                }
                            }
                            break; // Compare immediate
                        }
                    }
                    break; // Immediate
                    case 1: unimplementedopcode(opcode); break; // Bit manipulation / MOVEP
                }
            }
            break;
            
            // Move byte
            case 0x1000:
            {
                uint8_t source = 0;

		source = (uint8_t)(getsourcereg(opcodesourcemode(opcode), opcodesourceregister(opcode), opcode, 1));
                
                switch (opcodedestmode(opcode)) // Determines destination mode
                {
                    case 0:
                    {
                         uint32_t temp = m68kreg.datareg[opcodedestregister(opcode)];
                         m68kreg.datareg[opcodedestregister(opcode)] = ((temp & 0xFFFFFF00) | source);
                    }
                    break; // Data register
                    case 1:
                    {
                        uint32_t temp = m68kreg.addrreg[opcodedestregister(opcode)];
                        m68kreg.addrreg[opcodedestregister(opcode)] = ((temp & 0xFFFFFF00) | source);
                    }
                    break; // Address register
                    case 2: 
                    {
                        writeByte(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        mcycles += 4;
                    }
                    break; // Address
                    case 3: 
                    {
                        writeByte(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        m68kreg.addrreg[opcodedestregister(opcode)] += 1;
                        mcycles += 4;
                    }
                    break; // Address with post-increment
                    case 4: 
                    {
                        m68kreg.addrreg[opcodedestregister(opcode)] -= 1;
                        writeByte(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        mcycles += 4;
                    }
                    break; // Address with pre-decrement
                    case 5: unimplementedopcode(opcode); break; // Address with displacement
                    case 6: unimplementedopcode(opcode); break; // Address with index
                    case 7:
                    {
                        switch (opcodedestregister(opcode)) // Determines destination register
                        {
                            case 0:
                            {
                                uint16_t wordtemp = readWord(m68kreg.pc + 1);
                                m68kreg.pc += 2;
                                
                                uint32_t temp = (uint32_t)(int16_t)(wordtemp);
                                
                                writeByte(temp, source);
                                mcycles += 8;
                            }
                            break; // Absolute word
                            case 1: writeByte(readLong(m68kreg.pc), source); m68kreg.pc += 4; mcycles += 12; break; // Absolute long
                            default: unimplementedopcode(opcode); break;
                        }
                    }
                    break; // Addressing mode 7
                }
                
		setnegative(sign(source, 1));
		setzero((source == 0));
            }
            break;

            // Move long
            case 0x2000:
            {
                uint32_t source = 0;

		source = (uint32_t)(getsourcereg(opcodesourcemode(opcode), opcodesourceregister(opcode), opcode, 3));

                switch (opcodedestmode(opcode)) // Determines destination mode
                {
                    case 0:
                    {
                        m68kreg.datareg[opcodedestregister(opcode)] = source;
                    }
                    break; // Data register
                    case 1:
                    {
                        m68kreg.addrreg[opcodedestregister(opcode)] = source;
                    }
                    break; // Address register
                    case 2: 
                    {
                        writeLong(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        mcycles += 8;
                    }
                    break; // Address
                    case 3: 
                    {
                        writeLong(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        m68kreg.addrreg[opcodedestregister(opcode)] += 4;
                        mcycles += 8;
                    }
                    break; // Address with post-increment
                    case 4: 
                    {
                        m68kreg.addrreg[opcodedestregister(opcode)] -= 4;
                        writeLong(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        mcycles += 8;
                    }
                    break; // Address with pre-decrement
                    case 5: unimplementedopcode(opcode); break; // Address with displacement
                    case 6: unimplementedopcode(opcode); break; // Address with index
                    case 7:
                    {
                        switch (opcodedestregister(opcode)) // Determines destination register
                        {
                            case 0: 
                            {
                                uint16_t wordtemp = readWord(m68kreg.pc);
                                m68kreg.pc += 2;
                                
                                uint32_t temp = (uint32_t)(int16_t)(wordtemp);
                                
                                writeLong(temp, source);
                                mcycles += 12;
                            }
                            break; // Absolute word
                            case 1: writeLong(readLong(m68kreg.pc), source); m68kreg.pc += 4; mcycles += 16; break; // Absolute long
                            default: unimplementedopcode(opcode); break;
                        }
                    }
                    break; // Addressing mode 7
                }
                
		setnegative(sign(source, 3));
		setzero((source == 0));
            }
            break;
            

            // Move word
            case 0x3000:
            {
                uint16_t source = 0;
		source = (uint16_t)(getsourcereg(opcodesourcemode(opcode), opcodesourceregister(opcode), opcode, 2));
                
                switch (opcodedestmode(opcode)) // Determines destination mode
                {
                    case 0:
                    {
                         uint32_t temp = m68kreg.datareg[opcodedestregister(opcode)];
                         m68kreg.datareg[opcodedestregister(opcode)] = ((temp & 0xFFFF0000) | source);
                    }
                    break; // Data register
                    case 1:
                    {
                        uint32_t temp = m68kreg.addrreg[opcodedestregister(opcode)];
                        m68kreg.addrreg[opcodedestregister(opcode)] = ((temp & 0xFFFF0000) | source);
                    }
                    break; // Address register
                    case 2: 
                    {
                        writeWord(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        mcycles += 4;
                    }
                    break; // Address
                    case 3: 
                    {
                        writeWord(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        m68kreg.addrreg[opcodedestregister(opcode)] += 2;
                        mcycles += 4;
                    }
                    break; // Address with post-increment
                    case 4: 
                    {
                        m68kreg.addrreg[opcodedestregister(opcode)] -= 2;
                        writeWord(m68kreg.addrreg[opcodedestregister(opcode)], source);
                        mcycles += 4;
                    }
                    break; // Address with pre-decrement
                    case 5: unimplementedopcode(opcode); break; // Address with displacement
                    case 6: unimplementedopcode(opcode); break; // Address with index
                    case 7:
                    {
                        switch (opcodedestregister(opcode)) // Determines destination register
                        {
                            case 0: unimplementedopcode(opcode); break; // Absolute word
                            case 1: unimplementedopcode(opcode); break; // Absolute long
                            default: break;
                        }
                    }
                    break; // Addressing mode 7
                }
                
		setnegative(sign(source, 2));
		setzero((source == 0));
            }
            break;
            
            // Misc opcodes
            case 0x4000:
            {
                switch (opcodedestmode(opcode))
                {
                    case 6: unimplementedopcode(opcode); break; // CHK
                    case 7:
                    {
                        uint32_t source = 0;
                        
                        switch (opcodesourcemode(opcode)) // Determines addressing mode
                        {
                            case 2: source = readLong(m68kreg.addrreg[opcodesourceregister(opcode)]); mcycles += 4; break; // Address
                            case 5: unimplementedopcode(opcode); break; // Address with displacement
                            case 6: unimplementedopcode(opcode); break; // Address with index
                            case 7:
                            {
                                switch (opcodesourceregister(opcode)) // Addressing mode 7
                                {
                                    case 0: 
                                    {
                                        uint16_t wordtemp = readWord(m68kreg.pc);
                                        m68kreg.pc += 2;
                                
                                        uint32_t temp = (uint32_t)(int16_t)(wordtemp);
                                
                                        source = readLong(temp);
                                        mcycles += 8;
                                    }
                                    break; // Absolute word
                                    case 1: source = readLong(readLong(m68kreg.pc)); m68kreg.pc += 4; mcycles += 12; break; // Absolute long
                                    case 2: unimplementedopcode(opcode); break; // PC with displacement
                                    case 3: unimplementedopcode(opcode); break; // PC with index
                                }
                            }
                            default: break;
                        }
                        
                        m68kreg.addrreg[opcodedestregister(opcode)] = source;
                    }
                    break; // LEA
                    default:
                    {
                        switch (opcodedestregister(opcode))
                        {
                            case 0: unimplementedopcode(opcode); break; // MOVE from SR / NEGX
                            case 1: unimplementedopcode(opcode); break; // CLR
                            case 2: unimplementedopcode(opcode); break; // MOVE to CCR / NEG
                            case 3: 
                            {
                                if (opcodesize(opcode) == 3) // MOVE to SR
                                {
                                    uint16_t temp = m68kreg.sr;
                                    
                                    uint16_t source = 0;
                                    
                                    if (TestBit(temp, 13)) // Must be exeucted in supervisor mode
                                    {
                                        switch (opcodesourcemode(opcode))
                                        {
                                            case 0: unimplementedopcode(opcode); break; // Data register
                                            case 2: unimplementedopcode(opcode); break; // Address
                                            case 3: unimplementedopcode(opcode); break; // Address with post-increment
                                            case 4: unimplementedopcode(opcode); break; // Address with pre-decrement
                                            case 5: unimplementedopcode(opcode); break; // Address with displacement
                                            case 6: unimplementedopcode(opcode); break; // Address with index
                                            case 7:
                                            {
                                                switch (opcodesourceregister(opcode))
                                                {
                                                    case 0: unimplementedopcode(opcode); break; // Absolute word
                                                    case 1: unimplementedopcode(opcode); break; // Absolute long
                                                    case 2: unimplementedopcode(opcode); break; // PC with displacement
                                                    case 3: unimplementedopcode(opcode); break; // PC with index
                                                    case 4: source = readWord(m68kreg.pc); m68kreg.pc += 2; break; // Immediate
                                                    default: break;
                                                }
                                            }
                                            default: break;
                                        }
                                        
                                        m68kreg.sr = source;
                                        mcycles += 12;
                                    }
                                    else // Privilege violation
                                    {
                                        cout << "Error: Supervisor state exception" <<  endl;
                                        exit(1);
                                    }
                                }
                                else // NOT
                                {
                                    unimplementedopcode(opcode);
                                }
                            }
                            break; // MOVE to SR / NOT
                            case 4: unimplementedopcode(opcode); break; // EXT / NBCD / SWAP / PEA / MOVEM (reg-to-mem)
                            case 5: unimplementedopcode(opcode); break; // ILLEGAL / TAS / TST
                            case 6: unimplementedopcode(opcode); break; // MOVEM (mem-to-reg)
                            case 7:
                            {
                                switch (opcodedestmode(opcode))
                                {
                                    case 1: 
                                    {
                                        switch (opcodemiscencode(opcode))
                                        {
                                            case 0: unimplementedopcode(opcode); break; // TRAP
                                            case 1: unimplementedopcode(opcode); break; // LINK / UNLK
                                            case 2: unimplementedopcode(opcode); break; // MOVE USP
                                            case 3:
                                            {
                                                switch (opcode)
                                                {
                                                    case 0x4E70: unimplementedopcode(opcode); break; // RESET
                                                    case 0x4E71: unimplementedopcode(opcode); break; // NOP
                                                    case 0x4E72: unimplementedopcode(opcode); break; // STOP
                                                    case 0x4E73: unimplementedopcode(opcode); break; // RTE
                                                    case 0x4E75: 
                                                    {
                                                        m68kreg.pc = readLong(m68kreg.getsp());
                                                        m68kreg.setsp(m68kreg.getsp() + 4);
                                                        mcycles += 16;
                                                    }
                                                    break; // RTS
                                                    case 0x4E76: unimplementedopcode(opcode); break; // TRAPV
                                                    case 0x4E77: unimplementedopcode(opcode); break; // RTR
                                                }
                                            }
                                            break; // Remaining misc opcodes
                                        }
                                    }
                                    break;
                                    case 2: unimplementedopcode(opcode); break; // JSR
                                    case 3: unimplementedopcode(opcode); break; // JMP
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
                switch (opcodesize(opcode)) // Determines opcode
                {
                    case 0: unimplementedopcode(opcode); break; // ADDQ.B / SUBQ.B
                    case 1: unimplementedopcode(opcode); break; // ADDQ.W / SUBQ.W
                    case 2:
                    {
                        switch (opcodebit8(opcode))
                        {
                            case 0:
                            {
                                int adder = 0;
                                uint32_t addtemp = 0;
                                uint32_t overflowtemp = 0;
                                switch (opcodedestregister(opcode)) // Determines immediate data
                                {
                                    case 0: adder = 8; break; // Immediate value 0
                                    default: adder = (int)opcodedestregister(opcode); break; // Immediate values 1-7
                                }
                        
                                switch (opcodesourcemode(opcode)) // Determines destination register
                                {
                                    case 0:
                                    {
                                        addtemp = m68kreg.datareg[opcodesourceregister(opcode)];
                                        overflowtemp = addtemp;
                                        m68kreg.datareg[opcodesourceregister(opcode)] = (addtemp + adder);
                                        mcycles += 8;
                                    }
                                    break; // Data register
                                    case 1:
                                    {
                                        addtemp = m68kreg.addrreg[opcodesourceregister(opcode)];
                                        overflowtemp = addtemp;
                                        m68kreg.addrreg[opcodesourceregister(opcode)] = (addtemp + adder);
                                        mcycles += 8;
                                    }
                                    break; // Address register
                                    case 2: unimplementedopcode(opcode); break; // Address
                                    case 3: unimplementedopcode(opcode); break; // Address with post-increment
                                    case 4: unimplementedopcode(opcode); break; // Address with pre-decrement
                                    case 5: unimplementedopcode(opcode); break; // Address with displacement
                                    case 6: unimplementedopcode(opcode); break; // Address with index
                                    case 7:
                                    {
                                        switch (opcodesourceregister(opcode))
                                        {
                                            case 0: unimplementedopcode(opcode); break; // Absolute word
                                            case 1: unimplementedopcode(opcode); break; // Absolute long
                                            default: break;
                                        }
                                    }
                                    break; // Addressing mode 7
                                    default: break;
                                }

				setnegative(sign(addtemp, 3));
				setzero((addtemp == 0));
				setcarry((addtemp > 0xFFFFFFFF));
				setextended(iscarry());
				setoverflow(overflow(overflowtemp, adder, addtemp, 3));
                            }
                            break; // ADDQ.L
                            case 1: unimplementedopcode(opcode); break; // SUBQ.L
                            default: break;
                        }
                    }
                    break; // ADDQ.L / SUBQ.L
                    case 3:
                    {
                        switch (opcodesourcemode(opcode))
                        {
                            case 1:
                            {
                                if (!getcond(opcodecondition(opcode)))
				{
                                    uint16_t temp = (m68kreg.datareg[opcodesourceregister(opcode)] & 0xFFFF);
				    temp -= 1;
                                        
                                    int16_t tempreg = (int16_t)(temp);
                                        
                                    if (tempreg != -1)
                                    {
                                        int16_t wordtemp = (int16_t)(readWord(m68kreg.pc));
                                        m68kreg.pc += wordtemp;
                                    }
                                    else
                                    {
                                        m68kreg.pc += 2;
                                        mcycles += 4;
                                    }
                                        
                                    mcycles += 10;
				}
                            }
                            break; // DBcc
                            default:
                            {
                                switch (opcodesourcemode(opcode))
                                {
                                    case 0: unimplementedopcode(opcode); break; // Data register
                                    case 2: unimplementedopcode(opcode); break; // Address
                                    case 3: unimplementedopcode(opcode); break; // Address with postincrement
                                    case 4: unimplementedopcode(opcode); break; // Address with predecrement
                                    case 5: unimplementedopcode(opcode); break; // Address with displacement
                                    case 6: unimplementedopcode(opcode); break; // Address with index
                                    case 7:
                                    {
                                        switch (opcodesourceregister(opcode))
                                        {
                                            case 0: unimplementedopcode(opcode); break; // Absolute word
                                            case 1: unimplementedopcode(opcode); break; // Absolute long
                                            default: unimplementedopcode(opcode); break;
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
		if (opcodecondition(opcode) == 0) // BRA
		{
                    uint8_t distemp = (opcode & 0xFF);
                    uint32_t pctemp = m68kreg.pc;
                        
                    if (distemp == 0)
                    {
                        uint16_t wordtemp = readWord(m68kreg.pc);
                        pctemp += wordtemp;
                    }
                    else
                    {
                        pctemp += (int8_t)distemp;
                    }
                        
                    m68kreg.pc = pctemp;
                        
                    mcycles += 10;
		}
		else if (opcodecondition(opcode) == 1) // BSR
		{
                    uint8_t distemp = (opcode & 0xFF);
                    uint32_t pctemp = m68kreg.pc;
                        
                    if (distemp == 0)
                    {
                        uint16_t wordtemp = readWord(m68kreg.pc);
                        pctemp += wordtemp;
                    }
                    else
                    {
                        pctemp += (int8_t)distemp;
                    }
                        
                    m68kreg.setsp(m68kreg.getsp() - 4);
                    writeLong(m68kreg.getsp(), m68kreg.pc + 2);
                    m68kreg.pc = pctemp;
                    mcycles += 18;
		}
		else // Bcc
		{
		    bool isbyte = false;		    


		    if (getcond(opcodecondition(opcode)))
		    {
                        uint8_t distemp = (opcode & 0xFF);
                        uint32_t pctemp = m68kreg.pc;
                        
                        if (distemp == 0)
                        {
                            uint16_t wordtemp = readWord(m68kreg.pc);
                            pctemp += wordtemp;
                            isbyte = false;
                        }
                        else
                        {
                           pctemp += (int8_t)distemp;
                            isbyte = true;
                        }
                        
                        m68kreg.pc = pctemp;
                        mcycles += 10;
		    }
		    else
		    {
                        m68kreg.pc += 2;
                        mcycles += 8;
                        if (!isbyte)
                        {
                            mcycles += 4;
                        }
		    }
		}
            }
            break;
            
            // MOVEQ
            case 0x7000:
            {
                uint8_t bytevalue = (opcode & 0xFF);
                uint32_t temp = (uint32_t)(int8_t)(bytevalue);
                m68kreg.datareg[opcodedestregister(opcode)] = temp;

		setnegative(sign(bytevalue, 1));
		setzero((bytevalue == 0));
                mcycles += 4;
            }
            break;
            
            default: unimplementedopcode(opcode); break;
        }
    }
}
