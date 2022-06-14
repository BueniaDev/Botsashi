/*
    This file is part of Botsashi.
    Copyright (C) 2022 BueniaDev.

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
	    virtual void trapException(int val) = 0;
	    virtual void stopFunction() = 0;
    };

    class BOTSASHI_API Botsashi
    {
	public:
	    Botsashi();
	    ~Botsashi();

	    enum : int { Byte, Word, Long };

	    enum : int
	    {
		None = 0,
		BitInstr = (1 << 0),
		Hold = (1 << 1),
		MoveA = (1 << 2),
	    };

	    enum : int
	    {
		BusError = 2,
		AddrError = 3,
		IllegalInst = 4,
		DivByZero = 5,
		BoundsChk = 6,
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
		ControlRegAddr = 0x7EC,
		AlterableAddr = 0x1FF,
		AllAddr = 0xFFF,
		DataAltAddr = 0x1FD,
		MemAltAddr = 0x1FC,
		ControlAltAddr = 0x1E4,
		ControlAltMemAddr = 0x1F4,
	    };

	    uint32_t getPC()
	    {
		return m68kreg.pc;
	    }

	    uint16_t getStatusReg()
	    {
		return m68kreg.statusreg;
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

		if (reg == 7)
		{
		    updateSP();
		}
	    }

	    void init();
	    void init(uint32_t init_pc);
	    void shutdown();
	    void reset();
	    void reset_exception();
	    int executenextinstr();
	    int executeinstr(uint16_t instr);
	    void debugoutput(bool printdisassembly = true);
	    size_t disassembleinstr(ostream &stream, uint32_t pc);
	    void fire_irq(int level, bool line);

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

	    template<int Size, bool is_reverse = false>
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
			if (is_reverse)
			{
			    interWrite(true, true, (addr + 2) & ~1, val);
			    interWrite(true, true, addr & ~1, val >> 16);
			}
			else
			{
			    interWrite(true, true, addr & ~1, val >> 16);
			    interWrite(true, true, (addr + 2) & ~1, val);
			}
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

	    void init_regs();
	    bool is_reset_exception = false;

	    unique_ptr<BotsashiInterface> inter;

	    template<int Size>
	    void pushStack(uint32_t val)
	    {
		uint32_t stack_pointer = getSP();
		stack_pointer -= bytes<Size>();
		write<Size>(stack_pointer, val);
		setSP(stack_pointer);
	    }

	    template<int Size>
	    uint32_t popStack()
	    {
		uint32_t stack_pointer = getSP();
		uint32_t value = read<Size>(stack_pointer);
		stack_pointer += bytes<Size>();
		setSP(stack_pointer);
		return value;
	    }

	    uint32_t getSP()
	    {
		return (ismodesupervisor() ? m68kreg.ssp : m68kreg.usp);
	    }

	    void setSP(uint32_t val)
	    {
		setAddrReg<Long>(7, val);
	    }

	    void updateSP()
	    {
		uint32_t data = getAddrReg<Long>(7);
		if (ismodesupervisor())
		{
		    m68kreg.ssp = data;
		}
		else
		{
		    m68kreg.usp = data;
		}
	    }

	    m68kregisters m68kreg;

	    uint8_t getConditonReg()
	    {
		return (m68kreg.statusreg & 0xFF);
	    }

	    void setConditionReg(uint8_t data)
	    {
		m68kreg.statusreg = ((m68kreg.statusreg & 0xFF00) | data);
	    }

	    void setStatusReg(uint16_t data)
	    {
		// TODO: Add support for other M68K architectures?
		bool prev_supervisor = ismodesupervisor();

		data &= 0xA71F; // T1 -- S -- -- I2 I1 I0 -- -- -- X N Z V C
		m68kreg.statusreg = data;

		// Update A7 when switching modes
		if (prev_supervisor != ismodesupervisor())
		{
		    setAddrReg<Long>(7, getSP());
		}
	    }

	    void stopFunction();
	    int handle_interrupts();

	    auto getsrcmode(uint16_t instr) -> int;
	    auto getsrcreg(uint16_t instr) -> int;
	    auto getdstmode(uint16_t instr) -> int;
	    auto getdstreg(uint16_t instr) -> int;
	    auto getopcond(uint16_t instr) -> int;

	    bool ismodesupervisor();
	    void set_supervisor_flag(bool is_set);
	    void set_trace_flag(bool is_set);
	    int get_irq_mask();
	    void set_irq_mask(int val);
	    bool iscarry();
	    bool isoverflow();
	    bool iszero();
	    bool issign();
	    bool isextend();

	    int irq_line = 0;

	    void setcarry(bool val);
	    void setoverflow(bool val);
	    void setzero(bool val);
	    void setsign(bool val);
	    void setextend(bool val);

	    template<int Size> auto getZero(uint32_t temp, bool is_extend = false) -> bool;
	    template<int Size> auto getSign(uint32_t temp) -> bool;

	    template<typename T> bool testbit(T reg, int bit);
	    template<typename T> T setbit(T reg, int bit);
	    template<typename T> T resetbit(T reg, int bit);
	    template<typename T> T changebit(T reg, int bit, bool val);
	    template<typename T> T togglebit(T reg, int bit);

	    #include "traits.inl"

	    #include "disassembly.inl"
	    #include "instructions.inl"

	    using m68kfunc = function<int(uint16_t)>;
	    using m68kdasmfunc = function<size_t(ostream&, uint32_t, uint16_t)>;

	    struct m68kmapping
	    {
		uint16_t mask;
		uint16_t value;
		m68kfunc function;
		m68kdasmfunc disfunc;
	    };

	    vector<m68kmapping> func_table;

	    m68kmapping decode_instr(uint16_t instr);

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