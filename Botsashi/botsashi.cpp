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

#include "botsashi.h"
using namespace botsashi;
using namespace std;

namespace botsashi
{
    enum : int { Byte, Word, Long };

    BotsashiInterface::BotsashiInterface()
    {

    }

    BotsashiInterface::~BotsashiInterface()
    {

    }

    Botsashi::Botsashi()
    {

    }

    Botsashi::~Botsashi()
    {

    }

    #include "utils.cpp"
    #include "memreg.cpp"

    void Botsashi::init()
    {
	init_regs();
	reset_exception();
	cout << "Botsashi::Initialized" << endl;
    }

    void Botsashi::init(uint32_t init_pc)
    {
	init_regs();
	m68kreg.pc = init_pc;
	setStatusReg(0x2000);
	setSP(0x01000000);
	m68kreg.usp = 0xFF0000;
	is_reset_exception = false;
	cout << "Botsashi::Initialized" << endl;
    }

    void Botsashi::reset()
    {
	cout << "Botsashi::Resetting..." << endl;

	// Only call reset exception if
	// system was initialized with one
	if (is_reset_exception)
	{
	    init();
	}
	else
	{
	    init(m68kreg.pc);
	}
    }

    void Botsashi::init_regs()
    {
	for (int i = 0; i < 8; i++)
	{
	    setDataReg<Long>(i, 0);
	    setAddrReg<Long>(i, 0);
	}

	m68kreg.usp = 0;
	m68kreg.ssp = 0;
    }

    void Botsashi::reset_exception()
    {
	m68kreg.pc = read<Long>(4);
	setStatusReg(0x2700);
	setSP(read<Long>(0));
	is_reset_exception = true;
    }

    void Botsashi::shutdown()
    {
	cout << "Botsashi::Shutting down..." << endl;
    }

    void Botsashi::setinterface(BotsashiInterface &cb)
    {
	inter = unique_ptr<BotsashiInterface>(&cb);
    }

    void Botsashi::stopFunction()
    {
	if (inter)
	{
	    inter->stopFunction();
	}
    }

    auto Botsashi::istrapOverride(int val) -> bool
    {
	if (inter)
	{
	    return inter->istrapOverride(val);
	}

	return false;
    }

    auto Botsashi::trapException(int val) -> void
    {
	if (inter)
	{
	    inter->trapException(val);
	}
    }

    int Botsashi::executenextinstr()
    {
	uint16_t currentinstr = read<Word>(m68kreg.pc);
	m68kreg.pc += 2;

	int cycles = executeinstr(currentinstr);

	if (cycles < 0)
	{
	    cout << "Exception occuring..." << endl;
	    debugoutput();
	    exit(1);
	}

	cycles += handle_interrupts();

	return cycles;
    }

    int Botsashi::executeinstr(uint16_t instr)
    {
	int first_four = (instr >> 12);
	int start = (table_offsets[first_four]);
	int finish = (table_offsets[(first_four + 1)] - 1);

	for (int i = finish; i >= start; i--)
	{
	    auto mapping = funcmappings.at(i);

	    if ((instr & mapping.mask) == mapping.value)
	    {
		return mapping.function(instr);
	    }
	}

	unrecognizedinstr(instr);
	return 0;
    }

    int Botsashi::handle_interrupts()
    {
	int irq_mask = get_irq_mask();

	int irq_level = 0;

	for (int level = 7; level > 0; level--)
	{
	    if (testbit(irq_line, level))
	    {
		irq_level = level;
		break;
	    }
	}

	if (irq_level <= irq_mask)
	{
	    return 0;
	}

	uint16_t status_copy = m68kreg.statusreg;
	set_supervisor_flag(true);
	set_trace_flag(false);
	set_irq_mask(irq_level);
	uint32_t vector_addr = (0x60 + (irq_level << 2));

	pushStack<Long>(m68kreg.pc);
	pushStack<Word>(status_copy);

	m68kreg.pc = read<Long>(vector_addr);
	irq_line = 0;
	return 44;
    }

    void Botsashi::fire_irq(int level, bool line)
    {
	// TODO: Implement NMIs and spurious/uninitalized interrupts
	if ((level <= 0) || (level >= 7))
	{
	    throw out_of_range("Invalid IRQ level");
	}

	irq_line = changebit(irq_line, level, line);
    }

    void Botsashi::debugoutput(bool printdisassembly)
    {
	for (int dataindex = 0; dataindex < 8; dataindex++)
	{
	    cout << "D" << dec << dataindex << ": " << hex << m68kreg.datareg[dataindex] << endl;
	}

	for (int addrindex = 0; addrindex < 8; addrindex++)
	{
	    cout << "A" << dec << addrindex << ": " << hex << m68kreg.addrreg[addrindex] << endl;
	}

	cout << "USP: " << hex << m68kreg.usp << endl;
	cout << "SSP: " << hex << m68kreg.ssp << endl;
	cout << "PC: " << hex << m68kreg.pc << endl;
	cout << "Status: " << hex << int(m68kreg.statusreg) << endl;
	cout << "Current instruction (in hex): " << hex << (int)read<Word>(m68kreg.pc) << endl;

	if (printdisassembly)
	{
	    stringstream instr_str;
	    disassembleinstr(instr_str, m68kreg.pc);
	    cout << "Current instruction (disassembly): " << instr_str.str() << endl;
	}

	cout << endl;
    }

    size_t Botsashi::disassembleinstr(ostream &stream, uint32_t pc)
    {
	uint32_t pcval = pc;

	uint16_t instr = read<Word>(pcval);
	pcval += 2;

	int first_four = (instr >> 12);
	int start = (table_offsets[first_four]);
	int finish = (table_offsets[(first_four + 1)] - 1);

	for (int i = finish; i >= start; i--)
	{
	    auto mapping = funcmappings.at(i);

	    if ((instr & mapping.mask) == mapping.value)
	    {
		return mapping.disfunc(stream, pcval, instr);
	    }
	}

	return m68kdis_unknown(stream, pcval, instr);
    }
};