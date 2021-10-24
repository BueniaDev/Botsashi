array<array<int, 9>, 12> move_bw_cycles = 
{{
   {  4,  4,  8,  8,  8, 12, 14, 12, 16 },
   {  4,  4,  8,  8,  8, 12, 14, 12, 16 },
   {  8,  8, 12, 12, 12, 16, 18, 16, 20 },
   {  8,  8, 12, 12, 12, 16, 18, 16, 20 },
   { 10, 10, 14, 14, 14, 18, 20, 18, 22 },
   { 12, 12, 16, 16, 16, 20, 22, 20, 24 },
   { 14, 14, 18, 18, 18, 22, 24, 22, 26 },
   { 12, 12, 16, 16, 16, 20, 22, 20, 24 },
   { 16, 16, 20, 20, 20, 24, 26, 24, 28 },
   { 12, 12, 16, 16, 16, 20, 22, 20, 24 },
   { 14, 14, 18, 18, 18, 22, 24, 22, 26 },
   {  8,  8, 12, 12, 12, 16, 18, 16, 20 }
}};

array<array<int, 9>, 12> move_l_cycles = 
{{
   {  4,  4, 12, 12, 12, 16, 18, 16, 20 },
   {  4,  4, 12, 12, 12, 16, 18, 16, 20 },
   { 12, 12, 20, 20, 20, 24, 26, 24, 28 },
   { 12, 12, 20, 20, 20, 24, 26, 24, 28 },
   { 14, 14, 22, 22, 22, 26, 28, 26, 30 },
   { 16, 16, 24, 24, 24, 28, 30, 28, 32 },
   { 18, 18, 26, 26, 26, 30, 32, 30, 34 },
   { 16, 16, 24, 24, 24, 28, 30, 28, 32 },
   { 20, 20, 28, 28, 28, 32, 34, 32, 36 },
   { 16, 16, 24, 24, 24, 28, 30, 28, 32 },
   { 18, 18, 26, 26, 26, 30, 32, 30, 34 },
   { 12, 12, 20, 20, 20, 24, 26, 24, 28 }
}};

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

template<int Size>
auto effective_address_cycles(int mode) -> int
{
    if (Size == Long)
    {
	return effective_address_l_cycles[mode];
    }
    else
    {
	return effective_address_bw_cycles[mode];
    }
}

auto calc_mode(int mode, int reg) -> int
{
    int mode_calc = mode;

    if (mode == 7)
    {
	mode_calc += reg;
    }

    return mode_calc;
}

auto getcond(int cond_val) -> bool
{
    cond_val &= 0xF;

    switch (cond_val)
    {
	case 0: return true; break; // True (T)
	case 1: return false; break; // False (F)
	case 2: return (!iscarry() && !iszero()); break; // High (HI)
	case 3: return (iscarry() || iszero()); break; // Low or Same (LS)
	case 4: return !iscarry(); break; // Carry Clear (CC)
	case 5: return iscarry(); break; // Carry Set (CS)
	case 6: return !iszero(); break; // Not Equal (NE)
	case 7: return iszero(); break; // Equal (EQ)
	case 8: return !isoverflow(); break; // Overflow Clear (VC)
	case 9: return isoverflow(); break; // Overflow Set (VS)
	case 10: return !issign(); break; // Plus (PL)
	case 11: return issign(); break; // Minus (MI)
	case 12: return (issign() == isoverflow()); break; // Greater or Equal (GE)
	case 13: return (issign() != isoverflow()); break; // Less Than (LT)
	case 14: return (!iszero() && (issign() == isoverflow())); break; // Greater Than (GT)
	case 15: return (!iszero() || (issign() != isoverflow())); break; // Less or Equal (LE)
	default: return false; break;
    }
}

template<int Size, bool is_extend_mode = false>
auto add_internal(uint32_t source, uint32_t operand) -> uint32_t
{
    bool extend_flag = (is_extend_mode) ? isextend() : false;
    operand = clip<Size>(operand);
    source = clip<Size>(source);
    uint32_t result = (source + operand + extend_flag);
    uint32_t carries = (source ^ operand ^ result);
    uint32_t overflow = ((source ^ result) & (operand ^ result));
    setcarry((carries ^ overflow) & msb<Size>());
    setoverflow(overflow & msb<Size>());
    setzero(getZero<Size>(result, is_extend_mode));
    setsign(getSign<Size>(result));
    setextend(iscarry());
    return clip<Size>(result);
}

template<int Size, uint16_t mask = AllAddr, int Flags = 0>
auto srcaddrmode(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    bool is_inst_legal = false;

    uint32_t temp = 0;

    switch (mode)
    {
	case 0:
	{
	    if (testbit(mask, 0))
	    {
		if (testbit(Flags, 0))
		{
		    temp = getDataReg<Long>(reg);
		}
		else
		{
		    temp = getDataReg<Size>(reg);
		}

		is_inst_legal = true;
	    }
	}
	break;
	case 2:
	{
	    if (testbit(mask, 2))
	    {
		temp = read<Size>(getAddrReg<Long>(reg));
		is_inst_legal = true;
	    }
	}
	break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    if (testbit(mask, 8))
		    {
			uint16_t ext_word = extension<Word>(m68kreg.pc);
			uint32_t addr =  clip<Long>(sign<Word>(ext_word));
			temp = read<Size>(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 9))
		    {
			uint32_t addr = extension<Long>(m68kreg.pc);
			temp = read<Size>(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 4:
		{
		    if (testbit(mask, 11))
		    {
			temp = extension<Size>(m68kreg.pc);
			is_inst_legal = true;
		    }
		}
		break;
		default:
		{
		    cout << "Unrecognized mode 7 source addressing mode of " << dec << int(reg) << endl;
		    exit(1);
		}
		break;
	    }
	}
	break;
	default:
	{
	    cout << "Unrecognized source addressing mode of " << dec << int(mode) << endl;
	    exit(1);
	}
	break;
    }

    if (!is_inst_legal)
    {
	cout << "Illegal instruction" << endl;
	exit(1);
	set_m68k_exception(IllegalInst);
    }

    return temp;
}

template<int Size, uint16_t mask = AllAddr, int Flags = 0>
auto rawaddrmode(int mode, int reg) -> uint32_t
{
    mode &= 7;
    reg &= 7;

    bool is_inst_legal = false;

    uint32_t temp = 0;

    switch (mode)
    {
	case 7:
	{
	    switch (reg)
	    {
		case 1:
		{
		    if (testbit(mask, 8))
		    {
			temp = extension<Long>(m68kreg.pc);
			is_inst_legal = true;
		    }
		}
		break;
		default:
		{
		    cout << "Unrecognized mode 7 raw addressing mode of " << dec << int(reg) << endl;
		    exit(1);
		}
		break;
	    }
	}
	break;
	default:
	{
	    cout << "Unrecognized raw addressing mode of " << dec << int(mode) << endl;
	    exit(1);
	}
	break;
    }

    if (!is_inst_legal)
    {
	cout << "Illegal instruction" << endl;
	exit(1);
	set_m68k_exception(IllegalInst);
    }

    return temp;
}

template<int Size, uint16_t mask = AllAddr, int Flags = 0>
auto dstaddrmode(int mode, int reg, uint32_t val) -> void
{
    mode &= 7;
    reg &= 7;

    bool is_inst_legal = false;

    switch (mode)
    {
	case 0:
	{
	    if (testbit(mask, 0))
	    {
		if (testbit(Flags, 0))
		{
		    setDataReg<Long>(reg, val);
		}
		else
		{
		    setDataReg<Size>(reg, val);
		}

		is_inst_legal = true;
	    }
	}
	break;
	case 1:
	{
	    if (testbit(mask, 1) && (Size != Byte))
	    {
		setAddrReg<Size>(reg, val);
		is_inst_legal = true;
	    }
	}
	break;
	case 2:
	{
	    if (testbit(mask, 2))
	    {
		write<Size>(getAddrReg<Long>(reg), val);
		is_inst_legal = true;
	    }
	}
	break;
	case 3:
	{
	    if (testbit(mask, 3))
	    {
		uint32_t inc_bytes = ((reg == 7) && (Size == Byte)) ? bytes<Word>() : bytes<Size>();
		cout << "Incrementing address by " << dec << inc_bytes << " bytes" << endl;
		uint32_t addr_reg = getAddrReg<Long>(reg);
		write<Size>(addr_reg, val);
		setAddrReg<Long>(reg, (addr_reg + inc_bytes));
		is_inst_legal = true;
	    }
	}
	break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    if (testbit(mask, 8))
		    {
			uint16_t ext_word = extension<Word>(m68kreg.pc);
			uint32_t addr = clip<Long>(sign<Word>(ext_word));
			write<Size>(addr, val);
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 9))
		    {
			uint32_t addr = extension<Long>(m68kreg.pc);
			write<Size>(addr, val);
			is_inst_legal = true;
		    }
		}
		break;
		default:
		{
		    cout << "Unrecognized mode 7 destination addressing mode of " << dec << int(reg) << endl;
		    exit(1);
		}
		break;
	    }
	}
	break;
	default:
	{
	    cout << "Unrecognized destination addressing mode of " << dec << int(mode) << endl;
	    exit(1);
	}
	break;
    }

    if (!is_inst_legal)
    {
	cout << "Illegal instruction" << endl;
	exit(1);
	set_m68k_exception(IllegalInst);
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

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(getZero<Size>(srcvalue));
    setsign(getSign<Size>(srcvalue));
    setcarry(false);
    setoverflow(false);

    dstaddrmode<Size>(dstmode, dstreg, srcvalue);

    if (is_m68k_exception())
    {
	return -1;
    }

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

auto m68k_moveq(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    uint32_t result = sign<Byte>(instr & 0xFF);
    
    setcarry(false);
    setoverflow(false);
    setzero(getZero<Byte>(result));
    setsign(getSign<Byte>(result));

    setDataReg<Long>(dstreg, result);
    return 4;
}

template<int Size> 
auto m68k_add(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint64_t target = clip<Size>(srcaddrmode<Size>(srcmode, srcreg));
    uint64_t source = getDataReg<Size>(dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    auto result = add_internal<Size>(source, target);

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
auto m68k_addq(uint16_t instr) -> int
{
    // NOTE: getdstreg(instr) is equivalent to
    // bits 9-11 of instr (aka. ((instr >> 9) & 0x7)),
    // where the immediate value is stored
    int imm_val = getdstreg(instr);

    // The immediate value of 0 represents a value of 8
    if (imm_val == 0)
    {
	imm_val = 8;
    }

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t source_val = srcaddrmode<Size, AlterableAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = 0;

    if (srcmode == 1)
    {
	result = (source_val + imm_val);
    }
    else
    {
	result = add_internal<Size>(source_val, imm_val);
    }

    dstaddrmode<Size, AlterableAddr>(srcmode, srcreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    if (Size == Long)
    {
	cycles = (srcmode <= 1) ? 8 : 12;
	cycles += effective_address_l_cycles[source_mode];
    }
    else
    {
	cycles = (srcmode == 0) ? 4 : 8;
	cycles += effective_address_bw_cycles[source_mode];
    }

    return cycles;
}

template<int Size> 
auto m68k_sub(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint64_t target = clip<Size>(srcaddrmode<Size>(srcmode, srcreg));
    uint64_t source = getDataReg<Size>(dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    auto result = source - target;

    setcarry(sign<Size>(result >> 1) < 0);
    setoverflow(sign<Size>((target ^ source) & (target ^ result)) < 0);
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

    uint32_t address = rawaddrmode<Long, ControlAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setAddrReg<Long>(dstreg, address);

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

auto m68k_jmp(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t subroutine_addr = rawaddrmode<Long, ControlAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    m68kreg.pc = clipAddr(subroutine_addr);

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    switch (source_mode)
    {
	case 2: cycles = 8; break;
	case 5: cycles = 10; break;
	case 6: cycles = 14; break;
	case 7: cycles = 10; break;
	case 8: cycles = 12; break;
	case 9: cycles = 10; break;
	case 10: cycles = 14; break;
	default: break;
    }

    return cycles;
}

auto m68k_jsr(uint16_t instr) -> int
{
    uint32_t stack_pointer = getAddrReg<Long>(7);

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t subroutine_addr = rawaddrmode<Long, ControlAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    stack_pointer -= 4;
    write<Long>(stack_pointer, m68kreg.pc);
    setAddrReg<Long>(7, stack_pointer);

    m68kreg.pc = clipAddr(subroutine_addr);

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    switch (source_mode)
    {
	case 2: cycles = 16; break;
	case 5: cycles = 18; break;
	case 6: cycles = 22; break;
	case 7: cycles = 18; break;
	case 8: cycles = 20; break;
	case 9: cycles = 18; break;
	case 10: cycles = 22; break;
	default: break;
    }

    return cycles;
}

auto m68k_bra(uint16_t instr) -> int
{
    uint8_t byte_dis = (instr & 0xFF);
    uint32_t pc_val = m68kreg.pc;

    if (byte_dis == 0)
    {
	uint16_t word_dis = extension<Word>(m68kreg.pc);
	pc_val += int16_t(word_dis);
    }
    else
    {
	pc_val += int8_t(byte_dis);
    }

    m68kreg.pc = pc_val;
    return 18;
};

auto m68k_bsr(uint16_t instr) -> int
{
    uint32_t stack_pointer = getAddrReg<Long>(7);

    uint8_t byte_dis = (instr & 0xFF);
    uint32_t pc_val = m68kreg.pc;

    if (byte_dis == 0)
    {
	uint16_t word_dis = extension<Word>(m68kreg.pc);
	pc_val += int16_t(word_dis);
    }
    else
    {
	pc_val += int8_t(byte_dis);
    }

    stack_pointer -= 4;
    write<Long>(stack_pointer, m68kreg.pc);
    setAddrReg<Long>(7, stack_pointer);
    m68kreg.pc = pc_val;
    return 18;
};

auto m68k_bcc(uint16_t instr) -> int
{
    int cond = getopcond(instr);

    uint8_t byte_dis = (instr & 0xFF);

    int cycles = 0;

    if (getcond(cond) == true)
    {
	uint32_t pc_val = m68kreg.pc;

	if (byte_dis == 0)
	{
	    uint16_t word_dis = extension<Word>(m68kreg.pc);
	    pc_val += int16_t(word_dis);
	}
	else
	{
	    pc_val += int8_t(byte_dis);
	}

	m68kreg.pc = pc_val;
	cycles = 10;
    }
    else
    {
	if (byte_dis == 0)
	{
	    m68kreg.pc += 2;
	}

	cycles = (byte_dis == 0) ? 12 : 8;
    }

    return cycles;
}

auto m68k_dbcc(uint16_t instr) -> int
{
    int cycles = 0;
    int cond = getopcond(instr);
    int srcreg = getsrcreg(instr);
    uint32_t pc_val = m68kreg.pc;
    int16_t word_dis = extension<Word>(m68kreg.pc);

    if (getcond(cond) == true)
    {
	cycles = 12;
    }
    else
    {
	uint32_t dreg_val = getDataReg<Word>(srcreg);
	setDataReg<Word>(srcreg, (dreg_val - 1));

	if (dreg_val > 0)
	{
	    pc_val += word_dis;
	    m68kreg.pc = pc_val;
	    cycles = 10;
	}
	else
	{
	    cycles = 14;
	}
    }

    return cycles;
}

auto m68k_rts(uint16_t instr) -> int
{
    uint32_t stack_pointer = getAddrReg<Long>(7);
    uint32_t pc_val = read<Long>(stack_pointer);
    stack_pointer += 4;
    setAddrReg<Long>(7, stack_pointer);
    m68kreg.pc = pc_val;
    return 16;
}

template<int Size, bool is_rev = false>
auto m68k_and(uint16_t instr) -> int
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, addr_mode_mask>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    uint32_t result = (reg_val & res_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    if (is_rev)
    {
	dstaddrmode<Size, addr_mode_mask>(srcmode, srcreg, result);

	if (is_m68k_exception())
	{
	    return -1;
	}
    }
    else
    {
	setDataReg<Size>(dstreg, result);
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (is_rev)
    {
	cycles = (Size == Long) ? 12 : 8;
    }
    else
    {
	if (Size == Long)
	{
	    if ((source_mode == 0) || (source_mode == 11))
	    {
		cycles = 8;
	    }
	    else
	    {
		cycles = 6;
	    }
	}
	else
	{
	    cycles = 4;
	}
    }

    cycles += effective_address_cycles<Size>(source_mode);

    return cycles;
}

template<int Size, bool is_rev = false>
auto m68k_or(uint16_t instr) -> int
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, addr_mode_mask>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    uint32_t result = (reg_val | res_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    if (is_rev)
    {
	dstaddrmode<Size, addr_mode_mask>(srcmode, srcreg, result);

	if (is_m68k_exception())
	{
	    return -1;
	}
    }
    else
    {
	setDataReg<Size>(dstreg, result);
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (is_rev)
    {
	cycles = (Size == Long) ? 12 : 8;
    }
    else
    {
	if (Size == Long)
	{
	    if ((source_mode == 0) || (source_mode == 11))
	    {
		cycles = 8;
	    }
	    else
	    {
		cycles = 6;
	    }
	}
	else
	{
	    cycles = 4;
	}
    }

    cycles += effective_address_cycles<Size>(source_mode);

    return cycles;
}

template<int Size>
auto m68k_not(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = clip<Size>(~reg_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    dstaddrmode<Size, DataAltAddr>(dstmode, dstreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (Size == Long)
    {
	cycles = (dest_mode == 0) ? 6 : 12;
    }
    else
    {
	cycles = (dest_mode == 0) ? 4 : 8;
    }

    cycles += effective_address_cycles<Size>(dest_mode);
    return cycles;
}

template<int Size>
auto m68k_addi(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = add_internal<Size>(reg_val, imm_val);

    dstaddrmode<Size, DataAltAddr>(dstmode, dstreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 16 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 20 : 12;
	cycles += effective_address_cycles<Size>(dest_mode);
    }

    return cycles;
}

template<int Size>
auto m68k_andi(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = (reg_val & imm_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    dstaddrmode<Size, DataAltAddr>(dstmode, dstreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 16 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 20 : 12;
	cycles += effective_address_cycles<Size>(dest_mode);
    }

    return cycles;
}

template<int Size>
auto m68k_ori(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = (reg_val | imm_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    dstaddrmode<Size, DataAltAddr>(dstmode, dstreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 16 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 20 : 12;
	cycles += effective_address_cycles<Size>(dest_mode);
    }

    return cycles;
}

template<int Size>
auto m68k_eori(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = (reg_val ^ imm_val);

    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setcarry(false);
    setoverflow(false);

    dstaddrmode<Size, DataAltAddr>(dstmode, dstreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 16 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 20 : 12;
	cycles += effective_address_cycles<Size>(dest_mode);
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
    dstaddrmode<Size, DataAltAddr>(dstmode, dstreg, 0);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int dest_mode = calc_mode(dstmode, dstreg);

    if (Size == Long)
    {
	cycles = (dest_mode == 0) ? 6 : 12;
    }
    else
    {
	cycles = (dest_mode == 0) ? 4 : 8;
    }

    cycles += effective_address_cycles<Size>(dest_mode);

    return cycles;
}

auto m68k_bchg(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);
    int bit_num = getDataReg<Long>(dstreg);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(!testbit(data_addr, bit_num));

    // Invert the selected bit of the destination operand
    data_addr = togglebit(data_addr, bit_num);

    dstaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg, data_addr);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (bit_num < 16) ? 6 : 8;
    }
    else
    {
	cycles = (8 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

auto m68k_bclr(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);
    int bit_num = getDataReg<Long>(dstreg);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(!testbit(data_addr, bit_num));
    data_addr = resetbit(data_addr, bit_num);

    dstaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg, data_addr);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (bit_num < 16) ? 8 : 10;
    }
    else
    {
	cycles = (8 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

auto m68k_bset(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);
    int bit_num = getDataReg<Long>(dstreg);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(!testbit(data_addr, bit_num));
    data_addr = setbit(data_addr, bit_num);

    dstaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg, data_addr);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (bit_num < 16) ? 6 : 8;
    }
    else
    {
	cycles = (8 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

auto m68k_bclrimm(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(!testbit(data_addr, bit_num));
    data_addr = resetbit(data_addr, bit_num);

    dstaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg, data_addr);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (bit_num < 16) ? 12 : 14;
    }
    else
    {
	cycles = (12 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

auto m68k_bsetimm(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(!testbit(data_addr, bit_num));
    data_addr = setbit(data_addr, bit_num);

    dstaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg, data_addr);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (bit_num < 16) ? 10 : 12;
    }
    else
    {
	cycles = (12 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

auto m68k_bchgimm(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    int bit_num = extension<Byte>(m68kreg.pc);
    bit_num &= (srcmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and 8-bits for the others

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setzero(!testbit(data_addr, bit_num));

    // Invert the selected bit of the destination operand
    data_addr = togglebit(data_addr, bit_num);

    dstaddrmode<Byte, DataAddr, BitInstr>(srcmode, srcreg, data_addr);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (bit_num < 16) ? 10 : 12;
    }
    else
    {
	cycles = (12 + effective_address_bw_cycles[source_mode]);
    }

    return cycles;
}

auto m68k_mulu(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t data_addr = srcaddrmode<Word, DataAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

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
}

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

auto m68k_nop(uint16_t instr) -> int
{
    // NOP instruction
    return 4;
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