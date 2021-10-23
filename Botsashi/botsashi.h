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
	    virtual ~BotsashiInterface();

	    virtual uint16_t readWord(bool upper, bool lower, uint32_t addr) = 0;
	    virtual void writeWord(bool upper, bool lower, uint32_t addr, uint16_t val) = 0;
	    virtual bool istrapOverride(int val) = 0;
	    virtual void trapException(int val, Botsashi &m68k) = 0;
	    virtual void stopFunction() = 0;
    };

    class BOTSASHI_API Botsashi
    {
	public:
	    Botsashi();
	    ~Botsashi();

	    unique_ptr<BotsashiInterface> inter;

	    enum : int { Byte, Word, Long };

	    enum : int
	    {
		BusError = 2,
		AddrError = 3,
		IllegalInst = 4,
		DivByZero = 5,
		BoundsChl = 6,
		Overflow = 7,
		Unprivileged = 8,
		Trace = 9,
		IllegalLineA = 10,
		IllegalLineF = 11,
	    };

	    // Possible addressing modes permitted by an instruction.
	    // Each bit represents a different addressing mode.
	    // For example ControlAddr = 0x7E4 which means
	    // the addressing modes are allowed as follows:
	    // Dn = Prohibited
	    // An = Prohibited
	    // (An) = Permitted
	    // (An)+ = Prohibited
	    // -(An) = Prohibited
	    // (d16,An) = Permitted
	    // (d8,An,Xn) = Permitted
	    // (xxx).W = Permitted
	    // (xxx).L = Permitted
	    // (d16,PC) = Permitted
	    // (d8,PC,Xn) = Permitted
	    // #<data> = Prohibited
	    enum : uint16_t 
	    {
		DataAddr = 0xFFD,
		MemAddr = 0xFFC,
		ControlAddr = 0x7E4,
		AlterableAddr = 0x1FF,
		AllAddr = 0xFFF,
		DataAltAddr = 0x1FD,
		MemAltAddr = 0x1FC,
		ControlAltAddr = 0x1E4
	    };

	    uint32_t getPC()
	    {
		return m68kreg.pc;
	    }

	    template<int Size>
	    uint32_t getDataReg(int reg)
	    {
		reg &= 7;
		return clip<Size>(m68kreg.datareg[reg]);
	    }

	    template<int Size>
	    uint32_t getAddrReg(int reg)
	    {
		reg &= 7;
		return clip<Size>(m68kreg.addrreg[reg]);
	    }

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

	    bool iscarry();
	    bool isoverflow();
	    bool iszero();
	    bool issign();
	    bool isextend();

	    void setcarry(bool val);
	    void setoverflow(bool val);
	    void setzero(bool val);
	    void setsign(bool val);
	    void setextend(bool val);

	    void init(uint32_t init_pc = 0);
	    void shutdown();
	    void reset_exception(uint32_t vector_offs = 0);
	    int executenextinstr();
	    int executeinstr(uint16_t instr);
	    void debugoutput(bool printdisassembly = true);
	    string disassembleinstr(uint32_t pc);

	    void setstatusreg(uint16_t val);

	    void stopFunction();

	    auto getsrcmode(uint16_t instr) -> int;
	    auto getsrcreg(uint16_t instr) -> int;
	    auto getdstmode(uint16_t instr) -> int;
	    auto getdstreg(uint16_t instr) -> int;

	    template<int Size> auto getZero(uint32_t temp, bool is_extend = false) -> bool;
	    template<int Size> auto getSign(uint32_t temp) -> bool;

	    template<typename T> bool testbit(T reg, int bit);
	    template<typename T> T setbit(T reg, int bit);
	    template<typename T> T resetbit(T reg, int bit);
	    template<typename T> T changebit(T reg, int bit, bool val);
	    template<typename T> T togglebit(T reg, int bit);

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

	private:
	    struct m68kregisters
	    {
		uint32_t datareg[8];
		uint32_t addrreg[8];
		uint32_t usp = 0;
		uint32_t ssp = 0;
		uint32_t pc = 0;
		uint16_t statusreg;
	    };

	    struct m68kexceptions
	    {
		bool is_exception = false;
		int exception_type = -1;
	    };

	    m68kexceptions m68kexcept;

	    bool is_m68k_exception()
	    {
		return m68kexcept.is_exception;
	    }

	    void set_m68k_exception(int type)
	    {
		m68kexcept.is_exception = true;
		m68kexcept.exception_type = type;
	    }

	    m68kregisters m68kreg;

	    #include "traits.inl"

	    #include "disassembly.inl"
	    #include "instructions.inl"

	    using m68kfunc = function<int(uint16_t)>;
	    using m68kdasmfunc = function<string(uint32_t, uint16_t)>;

	    struct m68kmapping
	    {
		uint16_t mask;
		uint16_t value;
		m68kfunc function;
		m68kdasmfunc disfunc;
	    };

	    #include "instr_tables.inl"

	    auto count_bits(uint64_t source) -> uint32_t;

	    auto clipAddr(uint32_t addr) -> uint32_t;
	    auto interRead(bool upper, bool lower, uint32_t addr) -> uint16_t;
	    auto interWrite(bool upper, bool lower, uint32_t addr, uint16_t val) -> void;
	    auto istrapOverride(int val) -> bool;
	    auto trapException(int val) -> void;
    };
};

#endif // BOTSASHI_H