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

#include "botsashi.h"

namespace botsashi
{
    enum : int { Byte, Word, Long };

    #include "utils.cpp"
    #include "traits.cpp"
    #include "memreg.cpp"

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

    void Botsashi::init(uint32_t init_pc)
    {
	for (int i = 0; i < 8; i++)
	{
	    m68kreg.datareg[i] = 0;
	    m68kreg.addrreg[i] = 0;
	}

	m68kreg.pc = init_pc;
	setstatusreg(0x2700);

	cout << "Botsashi::Initialized" << endl;
    }

    void Botsashi::shutdown()
    {
	if (inter)
	{
	    inter.reset();
	}

	cout << "Botsashi::Shutting down..." << endl;
    }

    void Botsashi::setinterface(BotsashiInterface &cb)
    {
	inter = unique_ptr<BotsashiInterface>(&cb);
    }

    void Botsashi::setstatusreg(uint16_t val)
    {
	m68kreg.statusreg = val;
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
	    inter->trapException(val, *this);
	}
    }

    int Botsashi::executenextinstr()
    {
	uint16_t currentinstr = read<Word>(m68kreg.pc);
	m68kreg.pc += 2;
	return executeinstr(currentinstr);
    }

    int Botsashi::executeinstr(uint16_t instr)
    {
	for (auto mappingiter = funcmappings.rbegin(); mappingiter != funcmappings.rend(); mappingiter++)
	{
	    auto mapping = *mappingiter;
	    if ((instr & mapping.mask) == mapping.value)
	    {
		return mapping.function(instr);
	    }
	}

	unrecognizedinstr(instr);
	return 0;
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
	    cout << "Current instruction (disassembly): " << disassembleinstr(m68kreg.pc) << endl;
	}

	cout << endl;
    }

    string Botsashi::disassembleinstr(uint32_t pc)
    {
	uint32_t pcval = pc;

	uint16_t instr = read<Word>(pcval);
	pcval += 2;

	for (auto mappingiter = funcmappings.rbegin(); mappingiter != funcmappings.rend(); mappingiter++)
	{
	    auto mapping = *mappingiter;
	    if ((instr & mapping.mask) == mapping.value)
	    {
		return mapping.disfunc(pcval, instr);
	    }
	}

	return m68kdis_unknown(pcval, instr);
    }
};