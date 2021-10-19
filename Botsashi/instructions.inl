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

template<int Size, bool is_bit_instr = false>
auto addrmodedata(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    switch (mode)
    {
	case 0: 
	{
	    if (is_bit_instr)
	    {
		return getDataReg<Long>(reg);
	    }
	    else
	    {
		return getDataReg<Size>(reg);
	    }
	}
	break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t ext_word = read<Word>(m68kreg.pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(ext_word));
		    return read<Size>(addr);
		}
		break;
		case 1:
		{
		    // Fetch extension long
		    uint32_t addr = extension<Long>(m68kreg.pc);
		    return read<Size>(addr);
		}
		break;
		case 4: return extension<Size>(m68kreg.pc); break;
		default: cout << "Unrecognized mode 7 register index of " << dec << (int)(reg) << endl; exit(1); break;
	    }
	}
	break;
	default: cout << "Unrecognized data address mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }

    return 0;
}

template<int Size, bool is_bit_instr = false>
auto addrmodedataload(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    switch (mode)
    {
	case 0: 
	{
	    if (is_bit_instr)
	    {
		return getDataReg<Long>(reg);
	    }
	    else
	    {
		return getDataReg<Size>(reg);
	    }
	}
	break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t ext_word = read<Word>(m68kreg.pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(ext_word));
		    return read<Size>(addr);
		}
		break;
		case 1:
		{
		    // Fetch extension long
		    uint32_t addr = extension<Long>(m68kreg.pc);
		    return read<Size>(addr);
		}
		break;
		default: cout << "Unrecognized mode 7 register index of " << dec << (int)(reg) << endl; exit(1); break;
	    }
	}
	break;
	default: cout << "Unrecognized data address mode index of " << dec << (int)(mode) << endl; exit(1); break;
    }

    return 0;
}

template<int Size, bool is_bit_instr = false>
auto addrmodedatastore(int mode, int reg, uint32_t val) -> void
{
    mode &= 7;
    reg &= 7;

    switch (mode)
    {
	case 0:
	{
	    if (is_bit_instr)
	    {
		setDataReg<Long>(reg, val);
	    }
	    else
	    {
		setDataReg<Size>(reg, val);
	    }
	}
	break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t ext_word = extension<Word>(m68kreg.pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(ext_word));
		    write<Size>(addr, val);
		}
		break;
		case 1:
		{
		    // Fetch extension long
		    uint32_t addr = extension<Long>(m68kreg.pc);
		    write<Size>(addr, val);
		}
		break;
		default: cout << "Unrecognized mode 7 register store index of " << dec << (int)(reg) << endl; exit(1); break;
	    }
	}
	break;
	default: cout << "Unrecognized data address store mode index of " << dec << (int)(mode) << endl; exit(1); break;
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
		    uint16_t ext_word = extension<Word>(m68kreg.pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(ext_word));
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

auto m68k_unknown(uint16_t instr) -> int
{
    unrecognizedinstr(instr);
    return 0;
}

template<int Size> 
auto m68k_move(uint16_t instr) -> int
{
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
auto m68k_add(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint64_t target = clip<Size>(srcaddrmode<Size>(srcmode, srcreg));
    uint64_t source = getDataReg<Size>(dstreg);

    auto result = source + target;

    setcarry(sign<Size>(result >> 1) < 0);
    setoverflow(sign<Size>(~(target ^ source) & (target ^ result)) < 0);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setextend(iscarry());

    setDataReg<Size>(dstreg, result);

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
auto m68k_addr(uint16_t instr) -> int
{
    cout << "Reversed operands" << endl;
    exit(0);
    return 0;
}

template<int Size> 
auto m68k_sub(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t target = clip<Size>(srcaddrmode<Size>(srcmode, srcreg));
    uint32_t source = getDataReg<Size>(dstreg);

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
auto m68k_subr(uint16_t instr) -> int
{
    cout << "Reversed operands" << endl;
    exit(0);
    return 0;
}

auto m68k_lea(uint16_t instr) -> int
{
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

template<int Size>
auto m68k_and(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    uint32_t res_val = addrmodedata<Size>(srcmode, srcreg);
    uint32_t reg_val = getDataReg<Size>(dstreg);

    uint32_t result = (reg_val & res_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    setDataReg<Size>(dstreg, result);

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

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
auto m68k_or(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    uint32_t res_val = addrmodedata<Size>(srcmode, srcreg);
    uint32_t reg_val = getDataReg<Size>(dstreg);

    uint32_t result = (reg_val | res_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    setDataReg<Size>(dstreg, result);

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

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
auto m68k_not(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    uint32_t reg_val = addrmodedataload<Size>(dstmode, dstreg);

    uint32_t result = clip<Size>(~reg_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    addrmodedatastore<Size>(dstmode, dstreg, result);

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (Size == Long)
    {
	cycles = (dest_mode == 0) ? 6 : 12;
	cycles += effective_address_l_cycles[dest_mode];
    }
    else
    {
	cycles = (dest_mode == 0) ? 4 : 8;
	cycles += (effective_address_bw_cycles[dest_mode]);
    }

    return cycles;
}

template<int Size>
auto m68k_ori(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = addrmodedataload<Size>(dstmode, dstreg);

    uint32_t result = (reg_val | imm_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    addrmodedatastore<Size>(dstmode, dstreg, result);

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 16 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 20 : 12;

	if (Size == Long)
	{
	    cycles += effective_address_l_cycles[dest_mode];
	}
	else
	{
	    cycles += effective_address_bw_cycles[dest_mode];
	}
    }

    return cycles;
}

template<int Size>
auto m68k_eori(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = addrmodedataload<Size>(dstmode, dstreg);

    uint32_t result = (reg_val ^ imm_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    addrmodedatastore<Size>(dstmode, dstreg, result);

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 16 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 20 : 12;

	if (Size == Long)
	{
	    cycles += effective_address_l_cycles[dest_mode];
	}
	else
	{
	    cycles += effective_address_bw_cycles[dest_mode];
	}
    }

    return cycles;
}

auto m68k_swap(uint16_t instr) -> int
{
    // Fetch data register number
    // NOTE: getsrcreg(instr) is shorthand for (instr & 0x7)
    int regnum = getsrcreg(instr);

    uint32_t regval = getDataReg<Long>(regnum);
    uint32_t result = ((regval >> 16) | (regval << 16));

    setzero(getZero<Long>(result));
    setsign(getSign<Long>(result));
    setcarry(false);
    setoverflow(false);

    setDataReg<Long>(regnum, result);
    return 4;
}

auto m68k_exgdreg(uint16_t instr) -> int
{
    int regxnum = getdstreg(instr);
    int regynum = getsrcreg(instr);

    uint32_t oldregx = getDataReg<Long>(regxnum);
    setDataReg<Long>(regxnum, getDataReg<Long>(regynum));
    setDataReg<Long>(regynum, oldregx);
    return 6;
}

template<int Size>
auto m68k_clear(uint16_t instr) -> int
{
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

auto m68k_bchg(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);
    int bit_num = getDataReg<Long>(dstreg);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others
    uint32_t data_addr = addrmodedataload<Byte, true>(srcmode, srcreg);
    setzero(!testbit(data_addr, bit_num));

    // Invert the selected bit of the destination operand
    data_addr = togglebit(data_addr, bit_num);
    addrmodedatastore<Byte, true>(srcmode, srcreg, data_addr);

    // TODO: Cycle timings
    return 0;
}

auto m68k_bclr(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);
    int bit_num = getDataReg<Long>(dstreg);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others
    uint32_t data_addr = addrmodedataload<Byte, true>(srcmode, srcreg);
    setzero(!testbit(data_addr, bit_num));
    data_addr = resetbit(data_addr, bit_num);
    addrmodedatastore<Byte, true>(srcmode, srcreg, data_addr);

    // TODO: Cycle timings
    return 0;
}

auto m68k_bset(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);
    int bit_num = getDataReg<Long>(dstreg);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others
    uint32_t data_addr = addrmodedataload<Byte, true>(srcmode, srcreg);
    setzero(!testbit(data_addr, bit_num));
    data_addr = setbit(data_addr, bit_num);
    addrmodedatastore<Byte, true>(srcmode, srcreg, data_addr);

    // TODO: Cycle timings
    return 0;
}

auto m68k_bclrimm(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    uint32_t data_addr = addrmodedataload<Byte, true>(dstmode, dstreg);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    setzero(!testbit(data_addr, bit_num));
    data_addr = resetbit(data_addr, bit_num);
    addrmodedatastore<Byte, true>(dstmode, dstreg, data_addr);

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (dstmode == 0)
    {
	cycles = (bit_num < 16) ? 12 : 14;
    }
    else
    {
	cycles = (12 + effective_address_bw_cycles[dest_mode]);
    }

    return cycles;
}

auto m68k_bsetimm(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    uint32_t data_addr = addrmodedataload<Byte, true>(dstmode, dstreg);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others
    setzero(!testbit(data_addr, bit_num));
    data_addr = setbit(data_addr, bit_num);
    addrmodedatastore<Byte, true>(dstmode, dstreg, data_addr);

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (dstmode == 0)
    {
	cycles = (bit_num < 16) ? 10 : 12;
    }
    else
    {
	cycles = (12 + effective_address_bw_cycles[dest_mode]);
    }

    return cycles;
}

auto m68k_bchgimm(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    uint32_t data_addr = addrmodedataload<Byte, true>(dstmode, dstreg);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others
    setzero(!testbit(data_addr, bit_num));

    // Invert the selected bit of the destination operand
    data_addr = togglebit(data_addr, bit_num);
    addrmodedatastore<Byte, true>(dstmode, dstreg, data_addr);

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (dstmode == 0)
    {
	cycles = (bit_num < 16) ? 10 : 12;
    }
    else
    {
	cycles = (12 + effective_address_bw_cycles[dest_mode]);
    }

    return cycles;
}

auto m68k_mulu(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t data_addr = addrmodedata<Word>(srcmode, srcreg);
    uint32_t dstreg_addr = getDataReg<Word>(dstreg);
    uint32_t result = (dstreg_addr * data_addr);
    
    cout << "Multiplicand: " << hex << int(dstreg_addr) << endl;
    cout << "Multiplier: " << hex << int(data_addr) << endl;
    cout << "Product: " << hex << int(result) << endl;
    cout << endl;

    setzero(getZero<Long>(result));
    setsign(getSign<Long>(result));
    setcarry(false);
    setoverflow(false);
    setDataReg<Long>(dstreg, result);

    int source_mode = calc_mode(srcmode, srcreg);

    auto count = count_bits(data_addr);

    int cycles = (38 + (2 * count));
    cycles += effective_address_bw_cycles[source_mode];
    return cycles;
};

auto m68k_trap(uint16_t instr) -> int
{
    int val = (instr & 0xF);

    if (istrapOverride(val))
    {
	trapException(val);
	return 38;
    }

    cout << "Unimplemented: Low-level trap exception handling" << endl;
    exit(0);
    return 38;
}

auto m68k_stop(uint16_t instr) -> int
{
    uint16_t stopimm = extension<Word>(m68kreg.pc);
    // Set status register to value of stopimm
    m68kreg.statusreg = stopimm;
    // Stop program execution
    stopFunction();
    return 4;
}