// Botsashi
// (C) Buenia 2019
// Licensed under the GNU GPL v3
// See LICENSE for full license text

// File: utils.h
// Date: March 7, 2019
// Description: Provides misc utilities for the emulator

#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

bool TestBit(uint32_t reg, int bit);

uint32_t BitSet(uint32_t reg, int bit);

uint32_t BitReset(uint32_t reg, int bit);

int BitGetVal(uint32_t reg, int bit);

uint32_t BitSetTo(uint32_t reg, int bit, bool set);

extern uint16_t opcodeencode(uint16_t opcode);
            
extern uint16_t opcodecondition(uint16_t opcode);
            
extern uint16_t opcodedestregister(uint16_t opcode);
            
extern uint16_t opcodedestmode(uint16_t opcode);
            
extern uint16_t opcodebit8(uint16_t opcode);
            
extern uint16_t opcodesize(uint16_t opcode);
            
extern uint16_t opcodesourcemode(uint16_t opcode);

extern uint16_t opcodemiscencode(uint16_t opcode);
            
extern uint16_t opcodesourceregister(uint16_t opcode);

extern string tohexstring(uint32_t val);

extern string tostring(uint32_t val);

#endif // UTIL_H
