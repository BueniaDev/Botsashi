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

array<int, 12> movem_reg_cycles = 
{
     0,  0, 12, 12,  0, 16,
    18, 16, 20, 16, 18,  0
};

array<int, 12> movem_mem_cycles = 
{
     0,  0,  8, 0, 8, 12,
    14, 12, 16, 0, 0,  0
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
	case 15: return (iszero() || (issign() != isoverflow())); break; // Less or Equal (LE)
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

template<int Size, bool is_extend_mode = false>
auto sub_internal(uint32_t source, uint32_t operand) -> uint32_t
{
    bool extend_flag = (is_extend_mode) ? isextend() : false;
    operand = clip<Size>(operand);
    source = clip<Size>(source);

    uint32_t result = (source - operand - extend_flag);
    uint32_t carries = (source ^ operand ^ result);
    uint32_t overflow = ((source ^ result) & (operand ^ source));

    setcarry((carries ^ overflow) & msb<Size>());
    setoverflow(overflow & msb<Size>());
    setzero(getZero<Size>(result, is_extend_mode));
    setsign(getSign<Size>(result));
    setextend(iscarry());
    return result;
}

template<int Size>
auto and_internal(uint32_t source, uint32_t operand) -> uint32_t
{
    uint32_t result = (source & operand);
    setcarry(false);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    return clip<Size>(result);
}

template<int Size>
auto or_internal(uint32_t source, uint32_t operand) -> uint32_t
{
    uint32_t result = (source | operand);
    setcarry(false);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    return clip<Size>(result);
}

template<int Size>
auto eor_internal(uint32_t source, uint32_t operand) -> uint32_t
{
    uint32_t result = (source ^ operand);
    setcarry(false);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    return clip<Size>(result);
}

template<int Size>
auto cmp_internal(uint32_t source, uint32_t operand) -> uint32_t
{
    operand = clip<Size>(operand);
    source = clip<Size>(source);
    uint32_t result = (source - operand);
    uint32_t carries = (source ^ operand ^ result);
    uint32_t overflow = ((source ^ result) & (operand ^ source));

    setcarry((carries ^ overflow) & msb<Size>());
    setoverflow(overflow & msb<Size>());
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    return clip<Size>(result);
}

template<int Size>
auto asl_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;
    bool carry = false;
    uint32_t overflow = 0;

    for (uint32_t i = 0; i < shift; i++)
    {
	carry = testbit(result, topbit<Size>());
	uint32_t before = result;
	result <<= 1;
	overflow |= (before ^ result);
    }

    setcarry(carry);
    setoverflow(getSign<Size>(overflow));
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    if (shift != 0)
    {
	setextend(iscarry());
    }

    return clip<Size>(result);
}

template<int Size>
auto asr_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;
    bool carry = false;
    uint32_t overflow = 0;

    for (uint32_t i = 0; i < shift; i++)
    {
	carry = testbit(result, 0);
	uint32_t before = result;
	result = (sign<Size>(result) >> 1);
	overflow |= (before ^ result);
    }

    setcarry(carry);
    setoverflow(getSign<Size>(overflow));
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    if (shift != 0)
    {
	setextend(iscarry());
    }

    return clip<Size>(result);
}

template<int Size>
auto lsl_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;

    bool carry = false;

    for (uint32_t i = 0; i < shift; i++)
    {
	carry = testbit(result, topbit<Size>());
	result <<= 1;
    }

    setcarry(carry);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    if (shift != 0)
    {
	setextend(iscarry());
    }

    return clip<Size>(result);
}

template<int Size>
auto lsr_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;

    bool carry = false;

    for (uint32_t i = 0; i < shift; i++)
    {
	carry = testbit(result, 0);
	result >>= 1;
    }

    setcarry(carry);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    if (shift != 0)
    {
	setextend(iscarry());
    }

    return clip<Size>(result);
}

template<int Size>
auto rol_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;

    bool carry = false;

    for (uint32_t i = 0; i < shift; i++)
    {
	carry = testbit(result, topbit<Size>());
	result = ((result << 1) | carry);
    }

    setcarry(carry);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    return clip<Size>(result);
}

template<int Size>
auto ror_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;

    bool carry = false;

    for (uint32_t i = 0; i < shift; i++)
    {
	carry = testbit(result, 0);
	result >>= 1;

	if (carry)
	{
	    result = setbit(result, topbit<Size>());
	}
    }

    setcarry(carry);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    return clip<Size>(result);
}

template<int Size>
auto roxl_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;

    bool carry = isextend();

    for (uint32_t i = 0; i < shift; i++)
    {
	bool extend = carry;
	carry = testbit(result, topbit<Size>());
	result = ((result << 1) | extend);
    }

    setcarry(carry);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setextend(iscarry());

    return clip<Size>(result);
}

template<int Size>
auto roxr_internal(uint32_t reg, uint32_t shift) -> uint32_t
{
    uint32_t result = reg;

    bool carry = isextend();

    for (uint32_t i = 0; i < shift; i++)
    {
	bool extend = carry;
	carry = testbit(result, 0);
	result >>= 1;

	if (extend)
	{
	    result = setbit(result, topbit<Size>());
	}
    }

    setcarry(carry);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
    setextend(iscarry());

    return clip<Size>(result);
}

template<int Size, uint16_t mask = AllAddr, int Flags = None>
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
		if ((Flags & BitInstr) != 0)
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
	case 1:
	{
	    if (testbit(mask, 1) && (Size != Byte))
	    {
		temp = getAddrReg<Size>(reg);
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
	case 3:
	{
	    if (testbit(mask, 3))
	    {
		uint32_t inc_bytes = ((reg == 7) && (Size == Byte)) ? bytes<Word>() : bytes<Size>();
		uint32_t addr_reg = getAddrReg<Long>(reg);
		temp = read<Size>(addr_reg);

		if ((Flags & Hold) == 0)
		{
		    setAddrReg<Long>(reg, (addr_reg + inc_bytes));
		}

		is_inst_legal = true;
	    }
	}
	break;
	case 4:
	{
	    if (testbit(mask, 4))
	    {
		uint32_t inc_bytes = ((reg == 7) && (Size == Byte)) ? bytes<Word>() : bytes<Size>();
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint32_t ea_addr = (addr_reg - inc_bytes);
		temp = read<Size>(ea_addr);

		if ((Flags & Hold) == 0)
		{
		    setAddrReg<Long>(reg, ea_addr);
		}

		is_inst_legal = true;
	    }
	}
	break;
	case 5:
	{
	    if (testbit(mask, 5))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);

		uint16_t ext_word = 0;

		if ((Flags & Hold) == 0)
		{
		    ext_word = extension<Word>(m68kreg.pc);
		}
		else
		{
		    ext_word = read<Word>(m68kreg.pc);
		}

		uint32_t displacement = clip<Long>(sign<Word>(ext_word));

		temp = read<Size>(addr_reg + displacement);
		is_inst_legal = true;
	    }
	}
	break;
	case 6:
	{
	    if (testbit(mask, 6))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);

		uint16_t ext_word = 0;

		if ((Flags & Hold) == 0)
		{
		    ext_word = extension<Word>(m68kreg.pc);
		}
		else
		{
		    ext_word = read<Word>(m68kreg.pc);
		}

		int reg_val = ((ext_word >> 12) & 0x7);

		bool is_addr_reg = testbit(ext_word, 15);
		bool is_longword = testbit(ext_word, 11);

		uint32_t register_index = (is_addr_reg) ? getAddrReg<Long>(reg_val) : getDataReg<Long>(reg_val);

		uint32_t index_value = register_index;

		if (!is_longword)
		{
		    index_value = sign<Word>(register_index);
		}

		uint8_t displacement = (ext_word & 0xFF);
		uint32_t ext_displace = sign<Byte>(displacement);

		temp = read<Size>(addr_reg + index_value + ext_displace);
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
		    if (testbit(mask, 7))
		    {
			uint16_t ext_word = 0;

			if ((Flags & Hold) == 0)
			{
			    ext_word = extension<Word>(m68kreg.pc);
			}
			else
			{
			    ext_word = read<Word>(m68kreg.pc);
			}
			
			uint32_t addr = clip<Long>(sign<Word>(ext_word));
			temp = read<Size>(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 8))
		    {
			uint32_t addr = 0;

			if ((Flags & Hold) == 0)
			{
			    addr = extension<Long>(m68kreg.pc);
			}
			else
			{
			    addr = read<Long>(m68kreg.pc);
			}

			temp = read<Size>(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 2:
		{
		    if (testbit(mask, 9))
		    {
			uint32_t pc_val = m68kreg.pc;
			uint16_t ext_word = extension<Word>(m68kreg.pc);

			uint32_t displacement = clip<Long>(sign<Word>(ext_word));

			temp = read<Size>(pc_val + displacement);
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
		    debugoutput(false);
		    exit(1);
		}
		break;
	    }
	}
	break;
	default:
	{
	    cout << "Unrecognized source addressing mode of " << dec << int(mode) << endl;
	    debugoutput(false);
	    exit(1);
	}
	break;
    }

    if (!is_inst_legal)
    {
	cout << "Illegal instruction" << endl;
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
	case 2:
	{
	    if (testbit(mask, 2))
	    {
		temp = getAddrReg<Long>(reg);
		is_inst_legal = true;
	    }
	}
	break;
	case 3:
	{
	    if (testbit(mask, 3))
	    {
		temp = getAddrReg<Long>(reg);
		is_inst_legal = true;
	    }
	}
	break;
	case 4:
	{
	    if (testbit(mask, 4))
	    {
		temp = getAddrReg<Long>(reg);
		is_inst_legal = true;
	    }
	}
	break;
	case 5:
	{
	    if (testbit(mask, 5))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint16_t ext_word = extension<Word>(m68kreg.pc);

		uint32_t displacement = clip<Long>(sign<Word>(ext_word));

		temp = (addr_reg + displacement);
		is_inst_legal = true;
	    }
	}
	break;
	case 6:
	{
	    if (testbit(mask, 6))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);

		uint16_t ext_word = 0;

		if ((Flags & Hold) == 0)
		{
		    ext_word = extension<Word>(m68kreg.pc);
		}
		else
		{
		    ext_word = read<Word>(m68kreg.pc);
		}

		int reg_val = ((ext_word >> 12) & 0x7);

		bool is_addr_reg = testbit(ext_word, 15);
		bool is_longword = testbit(ext_word, 11);

		uint32_t register_index = (is_addr_reg) ? getAddrReg<Long>(reg_val) : getDataReg<Long>(reg_val);

		uint32_t index_value = register_index;

		if (!is_longword)
		{
		    index_value = sign<Word>(register_index);
		}

		uint8_t displacement = (ext_word & 0xFF);
		uint32_t ext_displace = sign<Byte>(displacement);

		temp = (addr_reg + index_value + ext_displace);
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
		    if (testbit(mask, 7))
		    {
			uint16_t ext_word = extension<Word>(m68kreg.pc);
			temp = clip<Long>(sign<Word>(ext_word));
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 8))
		    {
			temp = extension<Long>(m68kreg.pc);
			is_inst_legal = true;
		    }
		}
		break;
		case 2:
		{
		    if (testbit(mask, 9))
		    {
			uint32_t pc_val = m68kreg.pc;
			uint16_t ext_word = extension<Word>(m68kreg.pc);

			uint32_t displacement = clip<Long>(sign<Word>(ext_word));

			temp = (pc_val + displacement);
			is_inst_legal = true;
		    }
		}
		break;
		default:
		{
		    cout << "Unrecognized mode 7 raw addressing mode of " << dec << int(reg) << endl;
		    debugoutput(false);
		    exit(1);
		}
		break;
	    }
	}
	break;
	default:
	{
	    cout << "Unrecognized raw addressing mode of " << dec << int(mode) << endl;
	    debugoutput(false);
	    exit(1);
	}
	break;
    }

    if (!is_inst_legal)
    {
	cout << "Illegal instruction" << endl;
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
		if ((Flags & BitInstr) != 0)
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
		if ((Flags & MoveA) != 0)
		{
		    uint32_t movea_val = 0;

		    switch (Size)
		    {
			case Word: movea_val = sign<Word>(val); break;
			case Long: movea_val = clip<Long>(val); break;
		    }

		    setAddrReg<Long>(reg, movea_val);
		}
		else
		{
		    setAddrReg<Size>(reg, val);
		}

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
		uint32_t addr_reg = getAddrReg<Long>(reg);
		write<Size>(addr_reg, val);
		setAddrReg<Long>(reg, (addr_reg + inc_bytes));
		is_inst_legal = true;
	    }
	}
	break;
	case 4:
	{
	    if (testbit(mask, 4))
	    {
		uint32_t inc_bytes = ((reg == 7) && (Size == Byte)) ? bytes<Word>() : bytes<Size>();
		uint32_t addr_reg = getAddrReg<Long>(reg);
		setAddrReg<Long>(reg, (addr_reg - inc_bytes));
		uint32_t address = getAddrReg<Long>(reg);
		write<Size, true>(address, val);
		is_inst_legal = true;
	    }
	}
	break;
	case 5:
	{
	    if (testbit(mask, 5))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint16_t ext_word = extension<Word>(m68kreg.pc);

		uint32_t displacement = clip<Long>(sign<Word>(ext_word));

		write<Size>(addr_reg + displacement, val);
		is_inst_legal = true;
	    }
	}
	break;
	case 6:
	{
	    if (testbit(mask, 6))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint16_t ext_word = extension<Word>(m68kreg.pc);

		int reg_val = ((ext_word >> 12) & 0x7);

		bool is_addr_reg = testbit(ext_word, 15);
		bool is_longword = testbit(ext_word, 11);

		uint32_t register_index = (is_addr_reg) ? getAddrReg<Long>(reg_val) : getDataReg<Long>(reg_val);

		uint32_t index_value = register_index;

		if (!is_longword)
		{
		    index_value = sign<Word>(register_index);
		}

		uint8_t displacement = (ext_word & 0xFF);
		uint32_t ext_displace = sign<Byte>(displacement);

		write<Size>((addr_reg + index_value + ext_displace), val);
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
		    if (testbit(mask, 7))
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
		    if (testbit(mask, 8))
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
		    debugoutput(false);
		    exit(1);
		}
		break;
	    }
	}
	break;
	default:
	{
	    cout << "Unrecognized destination addressing mode of " << dec << int(mode) << endl;
	    debugoutput(false);
	    exit(1);
	}
	break;
    }

    if (!is_inst_legal)
    {
	cout << "Illegal instruction" << endl;
	set_m68k_exception(IllegalInst);
    }
}

void unrecognizedinstr(uint16_t instr)
{
    cout << "Unrecognized M68K instruction of " << hex << (int)(instr) << endl;
    debugoutput(false);
    exit(1);
}

auto m68k_unknown(uint16_t instr) -> int
{
    unrecognizedinstr(instr);
    return 0;
}

auto m68k_move_usp(uint16_t instr) -> int
{
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    bool is_reg = testbit(instr, 3);
    int reg = getsrcreg(instr);

    if (is_reg)
    {
	setAddrReg<Long>(reg, m68kreg.usp);
    }
    else
    {
	m68kreg.usp = getAddrReg<Long>(reg);
    }

    return 4;
}

auto m68k_andi_to_sr(uint16_t instr) -> int
{
    (void)instr;
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    uint16_t status_reg = m68kreg.statusreg;
    uint16_t bitmask_val = extension<Word>(m68kreg.pc);
    status_reg &= bitmask_val;
    setStatusReg(status_reg);
    return 20;
}

auto m68k_andi_to_ccr(uint16_t instr) -> int
{
    (void)instr;
    uint8_t status_reg = getConditonReg();
    uint8_t bitmask_val = extension<Byte>(m68kreg.pc);
    status_reg &= bitmask_val;
    setConditionReg(status_reg);
    return 20;
}

auto m68k_eori_to_sr(uint16_t instr) -> int
{
    (void)instr;
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    uint16_t status_reg = m68kreg.statusreg;
    uint16_t bitmask_val = extension<Word>(m68kreg.pc);
    status_reg ^= bitmask_val;
    setStatusReg(status_reg);
    return 20;
}

auto m68k_eori_to_ccr(uint16_t instr) -> int
{
    (void)instr;
    uint8_t status_reg = getConditonReg();
    uint8_t bitmask_val = extension<Byte>(m68kreg.pc);
    status_reg ^= bitmask_val;
    setConditionReg(status_reg);
    return 20;
}

auto m68k_ori_to_sr(uint16_t instr) -> int
{
    (void)instr;
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    uint16_t status_reg = m68kreg.statusreg;
    uint16_t bitmask_val = extension<Word>(m68kreg.pc);
    status_reg |= bitmask_val;
    setStatusReg(status_reg);
    return 20;
}

auto m68k_ori_to_ccr(uint16_t instr) -> int
{
    (void)instr;
    uint8_t status_reg = getConditonReg();
    uint8_t bitmask_val = extension<Byte>(m68kreg.pc);
    status_reg |= bitmask_val;
    setConditionReg(status_reg);
    return 20;
}

auto m68k_move_from_sr(uint16_t instr) -> int
{
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint16_t status_reg = m68kreg.statusreg;

    dstaddrmode<Word, DataAltAddr>(srcmode, srcreg, status_reg);

    if (is_m68k_exception())
    {
	return -1;
    }

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    if (srcmode == 0)
    {
	cycles = 6;
    }
    else
    {
	cycles = (8 + effective_address_cycles<Word>(source_mode));
    }

    return cycles;
}

auto m68k_move_to_ccr(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t ccr_value = srcaddrmode<Word, DataAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setConditionReg(ccr_value);
    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = (12 + effective_address_cycles<Word>(source_mode));
    return cycles;
}

auto m68k_move_to_sr(uint16_t instr) -> int
{
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t sr_value = srcaddrmode<Word, DataAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setStatusReg(sr_value);
    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = (12 + effective_address_cycles<Word>(source_mode));
    return cycles;
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

    dstaddrmode<Size, AllAddr, MoveA>(dstmode, dstreg, srcvalue);

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

auto m68k_movem(uint16_t instr) -> int
{
    bool is_mem_to_reg = testbit(instr, 10);
    bool is_long_or_word = testbit(instr, 6);

    int mask = (is_mem_to_reg << 1) | is_long_or_word;

    int cycles = 0;

    switch (mask)
    {
	case 0: cycles = m68k_movem_to_mem<Word>(instr); break;
	case 1: cycles = m68k_movem_to_mem<Long>(instr); break;
	case 2: cycles = m68k_movem_to_reg<Word>(instr); break;
	case 3: cycles = m68k_movem_to_reg<Long>(instr); break;
	// This shouldn't happen
	default:
	{
	    cout << "Unrecognized movem mask of " << dec << int(mask) << endl;
	    exit(0);
	}
	break;
    }

    return cycles;
}

template<int Size>
auto m68k_movem_to_mem(uint16_t instr) -> int
{
    uint16_t list_mask = extension<Word>(m68kreg.pc);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t addr = rawaddrmode<Long, ControlAltMemAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    int num_regs = 0;

    for (int i = 0; i < 16; i++)
    {
	if (!testbit(list_mask, i))
	{
	    continue;
	}

	num_regs += 1;

	int index = (srcmode == 4) ? (15 - i) : i;

	if (srcmode == 4)
	{
	    addr -= bytes<Size>();
	}

	auto data = (index < 8) ? getDataReg<Size>(index) : getAddrReg<Size>(index - 8);
	write<Size>(addr, data);

	if (srcmode != 4)
	{
	    addr += bytes<Size>();
	}
    }

    if ((srcmode == 4) || (srcmode == 3))
    {
	setAddrReg<Long>(srcreg, addr);
    }

    int cycle_mul = (Size == Long) ? 8 : 4;

    int cycles = (cycle_mul * num_regs);

    int source_mode = calc_mode(srcmode, srcreg);

    cycles += movem_mem_cycles[source_mode];
    return cycles;
}

template<int Size>
auto m68k_movem_to_reg(uint16_t instr) -> int
{
    uint16_t list_mask = extension<Word>(m68kreg.pc);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t addr = rawaddrmode<Long, ControlRegAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    int num_regs = 0;

    for (int i = 0; i < 16; i++)
    {
	if (!testbit(list_mask, i))
	{
	    continue;
	}

	num_regs += 1;

	int index = (srcmode == 4) ? (15 - i) : i;

	if (srcmode == 4)
	{
	    addr -= bytes<Size>();
	}

	auto data = read<Size>(addr);
	data = sign<Size>(data);

	if (index < 8)
	{
	    setDataReg<Long>(index, data);
	}
	else
	{
	    setAddrReg<Long>((index - 8), data);
	}

	if (srcmode != 4)
	{
	    addr += bytes<Size>();
	}
    }

    if ((srcmode == 4) || (srcmode == 3))
    {
	setAddrReg<Long>(srcreg, addr);
    }

    int cycle_mul = (Size == Long) ? 8 : 4;

    int cycles = (cycle_mul * num_regs);

    int source_mode = calc_mode(srcmode, srcreg);

    cycles += movem_reg_cycles[source_mode];
    return cycles;
}

auto m68k_abcd(uint16_t instr) -> int
{
    // Bit 3: 1=Pre-decrement mode, 0=Data register mode
    int address_mode = testbit(instr, 3) ? 4 : 0;

    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);

    uint32_t target = srcaddrmode<Byte, AllAddr, Hold>(address_mode, dstreg);
    uint32_t source = srcaddrmode<Byte, AllAddr>(address_mode, srcreg);
    uint32_t result = (target + source + isextend());

    bool is_carry = false;
    bool is_overflow = false;

    bool adjust_low = testbit((target ^ source ^ result), 4);

    if (adjust_low || ((result & 0xF) >= 0xA))
    {
	auto prev_result = result;
	result += 0x06;
	is_overflow |= (!testbit(prev_result, 7) && testbit(result, 7));
    }

    if (result >= 0xA0)
    {
	auto prev_result = result;
	result += 0x60;
	is_carry = true;
	is_overflow |= (!testbit(prev_result, 7) && testbit(result, 7));
    }

    dstaddrmode<Byte, AllAddr>(address_mode, dstreg, result);

    setzero(getZero<Byte>(result, true));
    setsign(getSign<Byte>(result));
    setcarry(is_carry);
    setoverflow(is_overflow);
    setextend(iscarry());

    int cycles = testbit(instr, 3) ? 18 : 6;
    return cycles;
}

auto m68k_sbcd(uint16_t instr) -> int
{
    // Bit 3: 1=Pre-decrement mode, 0=Data register mode
    int address_mode = testbit(instr, 3) ? 4 : 0;

    int srcreg = getsrcreg(instr);
    int dstreg = getdstreg(instr);

    uint32_t target = srcaddrmode<Byte, AllAddr, Hold>(address_mode, dstreg);
    uint32_t source = srcaddrmode<Byte, AllAddr>(address_mode, srcreg);
    uint32_t result = (target - source - isextend());

    bool is_carry = false;
    bool is_overflow = false;

    bool adjust_low = testbit((target ^ source ^ result), 4);
    bool adjust_high = testbit(result, 8);

    if (adjust_low)
    {
	auto prev_result = result;
	result -= 0x06;
	is_carry = (!testbit(prev_result, 7) && testbit(result, 7));
	is_overflow |= (testbit(prev_result, 7) && !testbit(result, 7));
    }

    if (adjust_high)
    {
	auto prev_result = result;
	result -= 0x60;
	is_carry = true;
	is_overflow |= (testbit(prev_result, 7) && !testbit(result, 7));
    }

    dstaddrmode<Byte, AllAddr>(address_mode, dstreg, result);

    setzero(getZero<Byte>(result, true));
    setsign(getSign<Byte>(result));
    setcarry(is_carry);
    setoverflow(is_overflow);
    setextend(iscarry());

    int cycles = testbit(instr, 3) ? 18 : 6;
    return cycles;
}

template<int Size>
auto m68k_add(uint16_t instr) -> int
{
    return m68k_add_internal<Size, false>(instr);
}

template<int Size>
auto m68k_addr(uint16_t instr) -> int
{
    return m68k_add_internal<Size, true>(instr);
}

template<int Size, bool is_rev>
auto m68k_add_internal(uint16_t instr) -> int
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    constexpr uint16_t addr_mode_flags = is_rev ? Hold : None;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, addr_mode_mask, addr_mode_flags>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    if (is_rev)
    {
	uint32_t result = add_internal<Size>(res_val, reg_val);
	dstaddrmode<Size, addr_mode_mask>(srcmode, srcreg, result);

	if (is_m68k_exception())
	{
	    return -1;
	}
    }
    else
    {
	uint32_t result = add_internal<Size>(reg_val, res_val);
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
auto m68k_adda(uint16_t instr) -> int
{
    int addr_reg = getdstreg(instr);

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getAddrReg<Size>(addr_reg);

    setAddrReg<Size>(addr_reg, (reg_val + res_val));

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    if (Size == Long)
    {
	if ((source_mode == 0) || (source_mode == 1) || (source_mode == 11))
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
	cycles = 8;
    }

    cycles += effective_address_cycles<Size>(source_mode);
    return cycles;
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

    uint32_t source_val = srcaddrmode<Size, AlterableAddr, Hold>(srcmode, srcreg);

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
    return m68k_sub_internal<Size, false>(instr);
}

template<int Size>
auto m68k_subr(uint16_t instr) -> int
{
    return m68k_sub_internal<Size, true>(instr);
}

template<int Size, bool is_rev>
auto m68k_sub_internal(uint16_t instr) -> int
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    constexpr uint16_t addr_mode_flags = is_rev ? Hold : None;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, addr_mode_mask, addr_mode_flags>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    if (is_rev)
    {
	uint32_t result = sub_internal<Size>(res_val, reg_val);
	dstaddrmode<Size, addr_mode_mask>(srcmode, srcreg, result);

	if (is_m68k_exception())
	{
	    return -1;
	}
    }
    else
    {
	uint32_t result = sub_internal<Size>(reg_val, res_val);
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
auto m68k_suba(uint16_t instr) -> int
{
    int addr_reg = getdstreg(instr);

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getAddrReg<Size>(addr_reg);

    setAddrReg<Size>(addr_reg, (reg_val - res_val));

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    if (Size == Long)
    {
	if ((source_mode == 0) || (source_mode == 1) || (source_mode == 11))
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
	cycles = 8;
    }

    cycles += effective_address_cycles<Size>(source_mode);
    return cycles;
}

template<int Size>
auto m68k_subq(uint16_t instr) -> int
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

    uint32_t source_val = srcaddrmode<Size, AlterableAddr, Hold>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = 0;

    if (srcmode == 1)
    {
	result = (source_val - imm_val);
    }
    else
    {
	result = sub_internal<Size>(source_val, imm_val);
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
auto m68k_neg(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = sub_internal<Size>(0, reg_val);

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

auto m68k_pea(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t address = rawaddrmode<Long, ControlAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    pushStack<Long>(address);

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = 0;

    switch (source_mode)
    {
	case 2: cycles = 12; break;
	case 5: cycles = 16; break;
	case 6: cycles = 20; break;
	case 7: cycles = 16; break;
	case 8: cycles = 20; break;
	case 9: cycles = 16; break;
	case 10: cycles = 20; break;
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
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t subroutine_addr = rawaddrmode<Long, ControlAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    pushStack<Long>(m68kreg.pc);

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

    pushStack<Long>(m68kreg.pc);
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

auto m68k_rte(uint16_t instr) -> int
{
    if (!ismodesupervisor())
    {
	// Privilege violation
	set_m68k_exception(Unprivileged);
	return -1;
    }

    (void)instr;
    uint16_t status_reg = popStack<Word>();
    setStatusReg(status_reg);
    m68kreg.pc = popStack<Long>();
    return 20;
}

auto m68k_rtr(uint16_t instr) -> int
{
    (void)instr;
    uint16_t status_reg = popStack<Word>();
    setConditionReg(status_reg);
    m68kreg.pc = popStack<Long>();
    return 20;
}

auto m68k_rts(uint16_t instr) -> int
{
    (void)instr;
    m68kreg.pc = popStack<Long>();
    return 16;
}

template<int Size>
auto m68k_and(uint16_t instr) -> int
{
    return m68k_and_internal<Size, false>(instr);
}

template<int Size>
auto m68k_andr(uint16_t instr) -> int
{
    return m68k_and_internal<Size, true>(instr);
}

template<int Size, bool is_rev>
auto m68k_and_internal(uint16_t instr) -> int
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    constexpr uint16_t addr_mode_flags = is_rev ? Hold : None;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, addr_mode_mask, addr_mode_flags>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    if (is_rev)
    {
	uint32_t result = and_internal<Size>(res_val, reg_val);
	dstaddrmode<Size, addr_mode_mask>(srcmode, srcreg, result);

	if (is_m68k_exception())
	{
	    return -1;
	}
    }
    else
    {
	uint32_t result = and_internal<Size>(reg_val, res_val);
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
auto m68k_or(uint16_t instr) -> int
{
    return m68k_or_internal<Size, false>(instr);
}

template<int Size>
auto m68k_orr(uint16_t instr) -> int
{
    return m68k_or_internal<Size, true>(instr);
}

template<int Size, bool is_rev>
auto m68k_or_internal(uint16_t instr) -> int
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    constexpr uint16_t addr_mode_flags = is_rev ? Hold : None;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, addr_mode_mask, addr_mode_flags>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    if (is_rev)
    {
	uint32_t result = or_internal<Size>(res_val, reg_val);
	dstaddrmode<Size, addr_mode_mask>(srcmode, srcreg, result);

	if (is_m68k_exception())
	{
	    return -1;
	}
    }
    else
    {
	uint32_t result = or_internal<Size>(reg_val, res_val);
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
auto m68k_eor(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, DataAltAddr, Hold>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    uint32_t result = eor_internal<Size>(res_val, reg_val);
    dstaddrmode<Size, DataAltAddr>(srcmode, srcreg, result);

    if (is_m68k_exception())
    {
	return -1;
    }

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = (Size == Long) ? 8 : 4;
    }
    else
    {
	cycles = (Size == Long) ? 12 : 8;
    }

    cycles += effective_address_cycles<Size>(source_mode);
    return cycles;
}

template<int Size>
auto m68k_cmp(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size, DataAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getDataReg<Size>(dstreg);

    cmp_internal<Size>(reg_val, res_val);

    int cycles = (Size == Long) ? 6 : 4;

    int source_mode = calc_mode(srcmode, srcreg);
    cycles += effective_address_cycles<Size>(source_mode);

    return cycles;
}

template<int Size>
auto m68k_cmpa(uint16_t instr) -> int
{
    int addr_reg = getdstreg(instr);

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t res_val = srcaddrmode<Size>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t reg_val = getAddrReg<Size>(addr_reg);

    cmp_internal<Size>(reg_val, res_val);

    int source_mode = calc_mode(srcmode, srcreg);

    int cycles = (6 + effective_address_cycles<Size>(source_mode));
    return cycles;
}

template<int Size>
auto m68k_tst(uint16_t instr) -> int
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t result = srcaddrmode<Size, DataAddr>(srcmode, srcreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    setcarry(false);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));

    int cycles = 4;
    int source_mode = calc_mode(srcmode, srcreg);
    cycles += effective_address_cycles<Size>(source_mode);

    return cycles;
}

template<int Size>
auto m68k_not(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

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

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

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
auto m68k_subi(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = sub_internal<Size>(reg_val, imm_val);
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

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = and_internal<Size>(reg_val, imm_val);
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

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = or_internal<Size>(reg_val, imm_val);
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
auto m68k_cmpi(uint16_t instr) -> int
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(m68kreg.pc);

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    cmp_internal<Size>(reg_val, imm_val);

    int dest_mode = calc_mode(dstmode, dstreg);

    int cycles = 0;

    if (dstmode == 0)
    {
	cycles = (Size == Long) ? 14 : 8;
    }
    else
    {
	cycles = (Size == Long) ? 12 : 8;
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

    uint32_t reg_val = srcaddrmode<Size, DataAltAddr, Hold>(dstmode, dstreg);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t result = eor_internal<Size>(reg_val, imm_val);
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
auto m68k_asl(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = asl_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_asr(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = asr_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_lsl(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = lsl_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_lsr(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = lsr_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_rol(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = rol_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_ror(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = ror_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_roxl(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = rol_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
    }

    return cycles;
}

template<int Size>
auto m68k_roxr(uint16_t instr) -> int
{
    bool is_reg = testbit(instr, 5);
    int reg_count = getdstreg(instr);
    int srcreg = getsrcreg(instr);

    int shift_count = 0;

    if (is_reg)
    {
	shift_count = (getDataReg<Size>(reg_count) % 64);
    }
    else
    {
	shift_count = reg_count;

	if (shift_count == 0)
	{
	    shift_count = 8;
	}
    }

    uint32_t shift_reg = getDataReg<Size>(srcreg);
    uint32_t result = ror_internal<Size>(shift_reg, shift_count);
    setDataReg<Size>(srcreg, result);

    int cycles = (2 * shift_count);

    if (Size == Long)
    {
	cycles += 8;
    }
    else
    {
	cycles += 6;
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

auto m68k_ext(uint16_t instr) -> int
{
    int opmode = getdstmode(instr);

    int cycles = 0;

    switch (opmode)
    {
	case 0b010: cycles = m68k_ext_internal<Byte, Word>(instr); break;
	case 0b011: cycles = m68k_ext_internal<Word, Long>(instr); break;
	default:
	{
	    cout << "Unrecognized opcode mode of " << dec << int(opmode) << endl;
	    exit(0);
	}
	break;
    }

    return cycles;
}

template<int OpSize, int Size>
auto m68k_ext_internal(uint16_t instr) -> int
{
    int regnum = getsrcreg(instr);

    uint32_t data_reg = getDataReg<OpSize>(regnum);

    uint32_t result = clip<Size>(sign<OpSize>(data_reg));

    setDataReg<Size>(regnum, result);

    setcarry(false);
    setoverflow(false);
    setzero(getZero<Size>(result));
    setsign(getSign<Size>(result));
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

auto m68k_exgareg(uint16_t instr) -> int
{
    int regxnum = getdstreg(instr);
    int regynum = getsrcreg(instr);

    uint32_t oldregx = getAddrReg<Long>(regxnum);
    setAddrReg<Long>(regxnum, getAddrReg<Long>(regynum));
    setAddrReg<Long>(regynum, oldregx);
    return 6;
}

auto m68k_exgdareg(uint16_t instr) -> int
{
    int regxnum = getdstreg(instr);
    int regynum = getsrcreg(instr);

    uint32_t oldregx = getDataReg<Long>(regxnum);
    setDataReg<Long>(regxnum, getAddrReg<Long>(regynum));
    setAddrReg<Long>(regynum, oldregx);
    return 6;
}

auto m68k_scc(uint16_t instr) -> int
{
    int cycles = 0;
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    int cond = getopcond(instr);

    int dest_mode = calc_mode(dstmode, dstreg);

    if (getcond(cond) == true)
    {
	cycles = (dest_mode == 0) ? 6 : 8;
	dstaddrmode<Byte, DataAltAddr>(dstmode, dstreg, 0xFFFFFFFF);
    }
    else
    {
	cycles = (dest_mode == 0) ? 4 : 8;
	dstaddrmode<Byte, DataAltAddr>(dstmode, dstreg, 0);
    }

    cycles += effective_address_cycles<Byte>(dest_mode);

    return cycles;
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

auto m68k_btst(uint16_t instr) -> int
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

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = 6;
    }
    else
    {
	cycles = (4 + effective_address_bw_cycles[source_mode]);
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

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, (BitInstr | Hold)>(srcmode, srcreg);

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

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, (BitInstr | Hold)>(srcmode, srcreg);

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

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, (BitInstr | Hold)>(srcmode, srcreg);

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

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, (BitInstr | Hold)>(srcmode, srcreg);

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

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, (BitInstr | Hold)>(srcmode, srcreg);

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

    uint32_t data_addr = srcaddrmode<Byte, DataAddr, (BitInstr | Hold)>(srcmode, srcreg);

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

auto m68k_btstimm(uint16_t instr) -> int
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

    int cycles = 0;

    int source_mode = calc_mode(srcmode, srcreg);

    if (source_mode == 0)
    {
	cycles = 10;
    }
    else
    {
	cycles = (8 + effective_address_bw_cycles[source_mode]);
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

auto m68k_divu(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t dividend = getDataReg<Long>(dstreg);
    uint32_t divisor = (srcaddrmode<Word, DataAddr>(srcmode, srcreg) << 16);

    if (is_m68k_exception())
    {
	return -1;
    }

    int source_mode = calc_mode(srcmode, srcreg);
    int ea_cycles = effective_address_cycles<Word>(source_mode);

    if (divisor == 0)
    {
	set_m68k_exception(DivByZero);
	return -1;
    }

    setcarry(false);

    if (dividend >= divisor)
    {
	setoverflow(true);
	setzero(false);
	setsign(true);
	return (10 + ea_cycles);
    }

    uint16_t quotient = 0;
    bool is_force = false;
    bool is_carry = false;

    int cycles = 6;

    for (int i = 0; i < 16; i++)
    {
	is_force = testbit(dividend, 31);
	dividend <<= 1;
	quotient = ((quotient << 1) | is_carry);

	is_carry = (is_force || (dividend >= divisor));

	if (is_carry)
	{
	    dividend -= divisor;
	}

	cycles += (!is_carry) ? 8 : (!is_force) ? 6 : 4;
    }

    cycles += is_force ? 6 : is_carry ? 4 : 2;
    quotient = ((quotient << 1) | is_carry);

    setsign(getSign<Word>(quotient));
    setzero(getZero<Word>(quotient));

    setDataReg<Long>(dstreg, (dividend | quotient));
    cycles += ea_cycles;
    return cycles;
}

auto m68k_divs(uint16_t instr) -> int
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t dividend = getDataReg<Long>(dstreg);
    uint32_t divisor = (srcaddrmode<Word, DataAddr>(srcmode, srcreg) << 16);

    if (is_m68k_exception())
    {
	return -1;
    }

    uint32_t dividend_i32 = dividend;
    uint32_t divisor_i32 = divisor;

    int source_mode = calc_mode(srcmode, srcreg);
    int ea_cycles = effective_address_cycles<Word>(source_mode);

    if (divisor == 0)
    {
	set_m68k_exception(DivByZero);
	return -1;
    }

    int sign_cycles = 0;

    if (testbit(divisor, 31))
    {
	divisor_i32 = -divisor_i32;
    }

    if (testbit(dividend, 31))
    {
	dividend_i32 = -dividend_i32;
	sign_cycles = 2;
    }

    setcarry(false);

    if (dividend_i32 >= divisor_i32)
    {
	setoverflow(true);
	setzero(false);
	setsign(true);
	return (16 + sign_cycles + ea_cycles);
    }

    uint16_t quotient = 0;
    bool is_carry = false;
    int cycles = (12 + sign_cycles);

    for (int i = 0; i < 15; i++)
    {
	dividend_i32 <<= 1;
	quotient = ((quotient << 1) | is_carry);
	is_carry = (dividend_i32 >= divisor_i32);

	if (is_carry)
	{
	    dividend_i32 -= divisor_i32;
	}

	cycles += !is_carry ? 8 : 6;
    }

    quotient = ((quotient << 1) | is_carry);
    dividend_i32 <<= 1;

    is_carry = (dividend_i32 >= divisor_i32);

    if (is_carry)
    {
	dividend_i32 -= divisor_i32;
    }

    quotient = ((quotient << 1) | is_carry);
    cycles += 4;

    if (testbit(divisor, 31))
    {
	cycles += 16;

	if (testbit(dividend, 31))
	{
	    if (testbit(quotient, 15))
	    {
		setoverflow(true);
	    }

	    dividend_i32 = -dividend_i32;
	}
	else
	{
	    quotient = -quotient;

	    if (quotient && !testbit(quotient, 15))
	    {
		setoverflow(true);
	    }
	}
    }
    else if (testbit(dividend, 31))
    {
	cycles += 18;
	quotient = -quotient;

	if (quotient && !testbit(quotient, 15))
	{
	    setoverflow(true);
	}

	dividend_i32 = -dividend_i32;
    }
    else
    {
	cycles += 14;

	if (testbit(quotient, 15))
	{
	    setoverflow(true);
	}
    }

    if (isoverflow())
    {
	setzero(false);
	setsign(true);
	return (cycles + ea_cycles);
    }

    setsign(getSign<Word>(quotient));
    setzero(getZero<Word>(quotient));

    setDataReg<Long>(dstreg, (dividend_i32 | quotient));

    cycles += ea_cycles;
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
    (void)instr;
    // NOP instruction
    return 4;
}

auto m68k_stop(uint16_t instr) -> int
{
    (void)instr;
    uint16_t stopimm = extension<Word>(m68kreg.pc);
    // Set status register to value of stopimm
    m68kreg.statusreg = stopimm;
    // Stop program execution
    stopFunction();
    return 4;
}