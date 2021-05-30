/*
    This file is part of Botsashi.
    Copyright (C) 2021 BueniaDev.

    Botsashi is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Botsashi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Botsashi.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BOTSASHI_H
#define BOTSASHI_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>
#include <functional>
#include <memory>
#include <bitset>
#include "botsashi_api.h"
using namespace std;
using namespace std::placeholders;

namespace botsashi
{
    class Botsashi;

    class BOTSASHI_API BotsashiInterface
    {
	friend class Botsashi;

	public:
	    BotsashiInterface();
	    ~BotsashiInterface();

	    virtual uint16_t readWord(bool upper, bool lower, uint32_t addr) = 0;
	    virtual void writeWord(bool upper, bool lower, uint32_t addr, uint16_t val) = 0;
	    virtual bool istrapOverride(int val) = 0;
	    virtual void trapException(int val, Botsashi *m68k) = 0;
	    virtual void stopFunction() = 0;
    };

    class BOTSASHI_API Botsashi
    {
	public:
	    Botsashi();
	    ~Botsashi();

	    unique_ptr<BotsashiInterface> inter;

	    enum : int { Byte, Word, Long };
	    enum : int { Alu = 1, Logical = 2, Clear = 4 };

	    template<int Size>
	    void setDataReg(int reg, uint32_t val)
	    {
		reg &= 7;
		m68kreg.datareg[reg] = ((m68kreg.datareg[reg] & ~mask<Size>()) | (val & mask<Size>()));	
	    }

	    template<int Size>
	    void setAddrReg(int reg, uint32_t val)
	    {
		reg &= 7;
		m68kreg.addrreg[reg] = ((m68kreg.addrreg[reg] & ~mask<Size>()) | (val & mask<Size>()));
	    }

	    struct m68kregisters
	    {
		uint32_t datareg[8];
		uint32_t addrreg[8];
		uint32_t usp = 0;
		uint32_t ssp = 0;
		uint32_t pc = 0;
		bitset<16> statusreg;
	    };

	    m68kregisters m68kreg;

	    bool iscarry();

	    void setcarry(bool val);
	    void setoverflow(bool val);
	    void setzero(bool val);
	    void setsign(bool val);
	    void setextend(bool val);

	    void init(uint32_t init_pc = 0);
	    void shutdown();
	    void executenextinstr();
	    void executeinstr(uint16_t instr);
	    void debugoutput(bool printdisassembly = true);
	    string disassembleinstr(uint32_t pc);

	    void setstatusreg(uint16_t val);

	    void stopFunction();

	    auto getsrcmode(uint16_t instr) -> int;
	    auto getsrcreg(uint16_t instr) -> int;
	    auto getdstmode(uint16_t instr) -> int;
	    auto getdstreg(uint16_t instr) -> int;

	    template<int Size> auto getZero(uint32_t temp) -> bool;
	    template<int Size> auto getSign(uint32_t temp) -> bool;

	    uint16_t currentinstr = 0;
	    bool stopped = false;

	    void setinterface(BotsashiInterface &cb);

	    template<int Size>
	    auto read(uint32_t addr) -> uint32_t
	    {
		switch (Size)
		{
		    case Byte:
		    {
			bool is_odd_addr = (addr & 1);
			int bit_offs = is_odd_addr ? 0 : 8;
			return clip<Byte>(interRead(!is_odd_addr, is_odd_addr, addr & ~1) >> bit_offs);
		    }
		    break;
		    case Word:
		    {
			return interRead(true, true, addr & ~1);
		    }
		    break;
		    case Long:
		    {
			uint32_t hi = interRead(true, true, addr & ~1);
			uint32_t lo = interRead(true, true, (addr + 2) & ~1);
			return (hi << 16) | lo;
		    }
		    break;
		}
	    }

	    template<int Size>
	    auto write(uint32_t addr, uint32_t val) -> void
	    {
		switch (Size)
		{
		    case Byte:
		    {
			bool is_odd_addr = (addr & 1);
			interWrite(!is_odd_addr, is_odd_addr, addr & ~1, val << 8 | clip<Byte>(val));
		    }
		    break;
		    case Word:
		    {
			interWrite(true, true, addr & ~1, val);
		    }
		    break;
		    case Long:
		    {
			interWrite(true, true, addr & ~1, val >> 16);
			interWrite(true, true, (addr + 2) & ~1, val);
		    }
		    break;
		}
	    }

	    template<int Size> 
	    auto extension(uint32_t &pc) -> uint32_t
	    {
		switch (Size)
		{
		    case Byte:
		    {
			uint16_t temp = read<Word>(pc);
			pc += 2;
			return clip<Byte>(temp);
		    }
		    break;
		    case Word:
		    {
			uint16_t temp = read<Word>(pc);
			pc += 2;
			return temp;
		    }
		    break;
		    case Long:
		    {
			uint32_t temp = read<Long>(pc);
			pc += 4;
			return temp;
		    }
		    break;
		}
	    }

	    #include "disassembly.inl"
	    #include "instructions.inl"

	    using m68kfunc = function<void()>;
	    using m68kdasmfunc = function<string(uint32_t, uint16_t)>;

	    struct m68kmapping
	    {
		uint16_t mask;
		uint16_t value;
		m68kfunc function;
		m68kdasmfunc disfunc;
	    };

	    #include "instr_tables.inl"

	private:
	    auto interRead(bool upper, bool lower, uint32_t addr) -> uint16_t;
	    auto interWrite(bool upper, bool lower, uint32_t addr, uint16_t val) -> void;
	    auto istrapOverride(int val) -> bool;
	    auto trapException(int val) -> void;

	    template<int Size> auto msb() -> uint32_t;
	    template<int Size> auto clip(uint32_t data) -> uint32_t;
	    template<int Size> auto sign(uint32_t data) -> int32_t;
	    template<int Size> auto mask() -> uint32_t;
    };
};

#endif // BOTSASHI_H