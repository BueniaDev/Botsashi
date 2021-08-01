array<array<int, 9>, 12> move_bw_cycles = 
{
     4,  4,  8,  8,  8, 12, 14, 12, 16,
     4,  4,  8,  8,  8, 12, 14, 12, 16,
     8,  8, 12, 12, 12, 16, 18, 16, 20,
     8,  8, 12, 12, 12, 16, 18, 16, 20,
    10, 10, 14, 14, 14, 18, 20, 18, 22,
    12, 12, 16, 16, 16, 20, 22, 20, 24,
    14, 14, 18, 18, 18, 22, 24, 22, 26,
    12, 12, 16, 16, 16, 20, 22, 20, 24,
    16, 16, 20, 20, 20, 24, 26, 24, 28,
    12, 12, 16, 16, 16, 20, 22, 20, 24,
    14, 14, 18, 18, 18, 22, 24, 22, 26,
     8,  8, 12, 12, 12, 16, 18, 16, 20
};

array<array<int, 9>, 12> move_l_cycles = 
{
     4,  4, 12, 12, 12, 16, 18, 16, 20,
     4,  4, 12, 12, 12, 16, 18, 16, 20,
    12, 12, 20, 20, 20, 24, 26, 24, 28,
    12, 12, 20, 20, 20, 24, 26, 24, 28,
    14, 14, 22, 22, 22, 26, 28, 26, 30,
    16, 16, 24, 24, 24, 28, 30, 28, 32,
    18, 18, 26, 26, 26, 30, 32, 30, 34,
    16, 16, 24, 24, 24, 28, 30, 28, 32,
    20, 20, 28, 28, 28, 32, 34, 32, 36,
    16, 16, 24, 24, 24, 28, 30, 28, 32,
    18, 18, 26, 26, 26, 30, 32, 30, 34,
    12, 12, 20, 20, 20, 24, 26, 24, 28
};

array<int, 12> effective_address_bw_cycles =
{
     0, 0,  4, 4,  6, 8,
    10, 8, 12, 8, 10, 4
};

array<int, 12> effective_address_l_cycles =
{
     0,  0,  8,  8, 10, 12,
    14, 12, 16, 12, 14,  8
};

auto calc_mode(int mode, int reg) -> int
{
    int mode_calc = mode;

    if (mode == 7)
    {
	mode_calc += reg;
    }

    return mode_calc;
}

template<int Size, int Flags = 0>
auto srcaddrmode(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    if ((Size == Byte) && (mode == 1))
    {
	cout << "[warning] Invalid access of address registers, ignoring transfer..." << endl;
	return 0;
    }

    switch (mode)
    {
	case 0: return getDataReg<Size>(reg); break;
	case 2: return read<Size>(m68kreg.addrreg[reg]); break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t temp = extension<Word>(m68kreg.pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(temp));
		    return read<Size>(addr);
		}
		break;
		case 4: return extension<Size>(m68kreg.pc); break;
		default: cout << "Unrecognized mode 7 source register index of " << dec << (int)(reg) << endl; exit(1); break;
	    }
	}
	break;
	default: cout << "Unrecognized source mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }

    return 0;
}

template<int Size, bool is_bit_clear = false>
auto addrmodedata(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    switch (mode)
    {
	case 0: 
	{
	    if (is_bit_clear)
	    {
		return getDataReg<Long>(reg);
	    }
	    else
	    {
		return getDataReg<Size>(reg);
	    }
	}
	break;
	default: cout << "Unrecognized data address mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }

    return 0;
}

template<int Size, bool is_bit_clear = false>
auto addrmodedatastore(int mode, int reg, uint32_t val) -> void
{
    mode &= 7;
    reg &= 7;

    switch (mode)
    {
	case 0:
	{
	    if (is_bit_clear)
	    {
		setDataReg<Long>(reg, val);
	    }
	    else
	    {
		setDataReg<Size>(reg, val);
	    }
	}
	break;
	default: cout << "Unrecognized data address mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }
}

auto loadaddrmode(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    switch (mode)
    {
	case 7:
	{
	    switch (reg)
	    {
		case 1: return extension<Long>(m68kreg.pc); break;
	    }
	}
	break;
	default: cout << "Unrecognized loading mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }

    return 0;
}

template<int Size, int Flags = 0>
auto dstaddrmode(int mode, int reg, uint32_t val) -> void
{
    mode &= 7;
    reg &= 7;

    if ((Size == Byte) && (mode == 1))
    {
	cout << "[warning] Invalid access of address registers, ignoring transfer..." << endl;
	return;
    }

    switch (mode)
    {
	case 0: setDataReg<Size>(reg, val); break;
	case 1: setAddrReg<Size>(reg, val); break;
	case 2: write<Size>(m68kreg.addrreg[reg], val); break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t temp = extension<Word>(m68kreg.pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(temp));
		    write<Size>(addr, val);
		}
		break;
		default: cout << "Unrecognized mode 7 destination register index of " << dec << (int)(reg) << endl; exit(1); break;
	    }
	}
	break;
	default: cout << "Unrecognized destination mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }
}

void unrecognizedinstr(uint16_t instr)
{
    cout << "Unrecognized M68K instruction of " << hex << (int)(instr) << endl;
    exit(1);
}

auto m68k_unknown() -> int
{
    unrecognizedinstr(currentinstr);
    return 0;
}

template<int Size> 
auto m68k_move() -> int
{
    uint16_t instr = currentinstr;
    int dstmode = getdstmode(instr);
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t srcvalue = srcaddrmode<Size>(srcmode, srcreg);

    setzero(getZero<Size>(srcvalue));
    setsign(getSign<Size>(srcvalue));
    setcarry(false);
    setoverflow(false);

    dstaddrmode<Size>(dstmode, dstreg, srcvalue);

    int dest_mode = calc_mode(dstmode, dstreg);
    int source_mode = calc_mode(srcmode, srcreg);

    if (Size == Long)
    {
	return move_l_cycles[source_mode][dest_mode];
    }
    else
    {
	return move_bw_cycles[source_mode][dest_mode];
    }
}

template<int Size> 
auto m68k_add() -> int
{
    uint16_t instr = currentinstr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint64_t target = clip<Size>(srcaddrmode<Size>(srcmode, srcreg));
    uint64_t source = clip<Size>(m68kreg.datareg[dstreg]);

    auto result = source + target;

    setcarry(sign<Size>(result >> 1) < 0);
    setoverflow(sign<Size>(~(target ^ source) & (target ^ result)) < 0);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setextend(iscarry());

    m68kreg.datareg[dstreg] = (m68kreg.datareg[dstreg] & ~mask<Size>()) | (result & mask<Size>());

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    if (Size == Long)
    {
	if (((instr & 0x30) == 0) || ((instr & 0x3F) == 0x3C))
	{
	    cycles = 8;
	}
	else
	{
	    cycles = 6;
	}

	cycles += effective_address_l_cycles[source_mode];
    }
    else
    {
	cycles = (4 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

template<int Size> 
auto m68k_addr() -> int
{
    // uint16_t instr = currentinstr;
    cout << "Reversed operands" << endl;
    exit(0);
    return 0;
}

template<int Size> 
auto m68k_sub() -> int
{
    uint16_t instr = currentinstr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t target = clip<Size>(srcaddrmode<Size>(srcmode, srcreg));
    uint32_t source = clip<Size>(m68kreg.datareg[dstreg]);

    auto result = source - target;

    setcarry(sign<Size>(result >> 1) < 0);
    setoverflow(sign<Size>((target ^ source) & (target ^ result)) < 0);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setextend(iscarry());

    m68kreg.datareg[dstreg] = (m68kreg.datareg[dstreg] & ~mask<Size>()) | (result & mask<Size>());

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    if (Size == Long)
    {
	if (((instr & 0x30) == 0) || ((instr & 0x3F) == 0x3C))
	{
	    cycles = 8;
	}
	else
	{
	    cycles = 6;
	}

	cycles += effective_address_l_cycles[source_mode];
    }
    else
    {
	cycles = (4 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

template<int Size> 
auto m68k_subr() -> int
{
    // uint16_t instr = currentinstr;
    cout << "Reversed operands" << endl;
    exit(0);
    return 0;
}

auto m68k_lea() -> int
{
    uint16_t instr = currentinstr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t temp = loadaddrmode(srcmode, srcreg);
    m68kreg.addrreg[dstreg] = temp;

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    switch (source_mode)
    {
	case 2: cycles = 4; break;
	case 5: cycles = 8; break;
	case 6: cycles = 12; break;
	case 7: cycles = 8; break;
	case 8: cycles = 12; break;
	case 9: cycles = 8; break;
	case 10: cycles = 12; break;
    }

    return cycles;
}

auto m68k_swap() -> int
{
    uint16_t instr = currentinstr;
    // Fetch data register number
    // NOTE: getsrcreg(instr) is shorthand for (instr & 0x7)
    int regnum = getsrcreg(instr);

    uint32_t regval = m68kreg.datareg[regnum];
    uint32_t result = ((regval >> 16) | (regval << 16));

    setzero(getZero<Long>(result));
    setsign(getSign<Long>(result));
    setcarry(false);
    setoverflow(false);

    m68kreg.datareg[regnum] = result;
    return 4;
}

auto m68k_exgdreg() -> int
{
    uint16_t instr = currentinstr;
    int regxnum = getdstreg(instr);
    int regynum = getsrcreg(instr);

    uint32_t oldregx = m68kreg.datareg[regxnum];
    m68kreg.datareg[regxnum] = m68kreg.datareg[regynum];
    m68kreg.datareg[regynum] = oldregx;
    return 6;
}

template<int Size>
auto m68k_clear() -> int
{
    uint16_t instr = currentinstr;

    // Fetch operand mode and register numbers
    // NOTE: getsrcmode(instr) and getsrcreg(instr)
    // are shorthand for ((instr >> 3) & 0x7)
    // and (instr & 0x7), respectively
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    // Set the status flags accordingly...
    setzero(true);
    setsign(false);
    setcarry(false);
    setoverflow(false);

    // ...and clear the appropriate portion of the destination operand to zero
    addrmodedatastore<Size>(dstmode, dstreg, 0);

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (instr & 0x0030)
    {
	cycles = (Size == Long) ? 12 : 8;

	if (Size == Long)
	{
	    cycles += effective_address_l_cycles[dest_mode];
	}
	else
	{
	    cycles += effective_address_bw_cycles[dest_mode];
	}
    }
    else
    {
	cycles = (Size == Long) ? 6 : 4;
    }

    return cycles;
}

auto m68k_bclrimm() -> int
{
    uint16_t instr = currentinstr;
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    uint32_t data_addr = addrmodedata<true>(dstmode, dstreg);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others
    bool is_bit_set = ((data_addr >> bit_num) & 1);
    setzero((is_bit_set == false));
    data_addr &= ~(1 << bit_num);
    addrmodedatastore<true>(dstmode, dstreg, data_addr);

    int cycles = 0;

    // TODO: Cycle timings for memory operands
    if (dstmode == 0)
    {
	cycles = (bit_num < 16) ? 12 : 14;
    }

    return cycles;
}

auto m68k_trap() -> int
{
    uint16_t instr = currentinstr;
    int val = (instr & 0xF);

    if (istrapOverride(val))
    {
	trapException(val);
	return 34;
    }

    cout << "Unimplemented: Low-level trap exception handling" << endl;
    exit(0);
    return 34;
}

auto m68k_stop() -> int
{
    uint16_t stopimm = extension<Word>(m68kreg.pc);
    // Set status register to value of stopimm
    m68kreg.statusreg = bitset<16>(stopimm);
    // Stop program execution
    stopFunction();
    return 4;
}