template<int Size, uint16_t mask = AllAddr>
auto srcmodedasm(int mode, int reg, ostream &stream, uint32_t &pc) -> size_t
{
    mode &= 7;
    reg &= 7;

    size_t offset = 0;

    size_t prev_pc = pc;

    bool is_inst_legal = false;

    stringstream ss;

    switch (mode)
    {
	case 0:
	{
	    if (testbit(mask, 0))
	    {
		ss << "d" << dec << reg;
		is_inst_legal = true;
	    }
	}
	break;
	case 1:
	{
	    if (testbit(mask, 1) && (Size != Byte))
	    {
		ss << "a" << dec << reg;
		is_inst_legal = true;
	    }
	}
	break;
	case 2:
	{
	    if (testbit(mask, 2))
	    {
		ss << "(a" << dec << reg << ")";
		is_inst_legal = true;
	    }
	}
	break;
	case 3:
	{
	    if (testbit(mask, 3))
	    {
		ss << "(a" << dec << reg << ")+";
		is_inst_legal = true;
	    }
	}
	break;
	case 5:
	{
	    if (testbit(mask, 5))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint16_t ext_word = extension<Word>(pc);

		uint32_t displacement = clip<Long>(sign<Word>(ext_word));

		ss << "$" << hex << int(addr_reg + displacement);
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
			uint16_t ext_word = extension<Word>(pc);
			uint32_t addr = clip<Long>(sign<Word>(ext_word));
			ss << "$" << hex << int(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 8))
		    {
			uint32_t addr = extension<Long>(pc);
			ss << "$" << hex << int(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 2:
		{
		    if (testbit(mask, 9))
		    {
			uint32_t pc_val = pc;
			uint16_t ext_word = extension<Word>(pc);

			uint32_t displacement = clip<Long>(sign<Word>(ext_word));

			ss << "$" << hex << int(pc_val + displacement);
			is_inst_legal = true;
		    }
		}
		break;
		case 4:
		{
		    if (testbit(mask, 11))
		    {
			uint32_t imm_val = extension<Size>(pc);
			ss << "#$" << hex << int(imm_val);
			is_inst_legal = true;
		    }
		}
		break;
		default: break;
	    }
	}
	break;
	default: break;
    }

    if (is_inst_legal)
    {
	stream << ss.str();
	offset = (pc - prev_pc);
    }
    else
    {
	stream << "und ";

	if (mode == 7)
	{
	    stream << "m7, r" << dec << reg;
	}
	else
	{
	    stream << "m" << dec << mode;
	}

	offset = 0;
    }

    return offset;
}

template<int Size, uint16_t mask = AllAddr>
auto rawmodedasm(int mode, int reg, ostream &stream, uint32_t &pc) -> size_t
{
    mode &= 7;
    reg &= 7;

    size_t offset = 0;

    size_t prev_pc = pc;

    uint32_t temp_addr = 0;

    bool is_inst_legal = false;

    switch (mode)
    {
	case 2:
	{
	    if (testbit(mask, 2))
	    {
		temp_addr = getAddrReg<Long>(reg);
		is_inst_legal = true;
	    }
	}
	break;
	case 3:
	{
	    if (testbit(mask, 3))
	    {
		temp_addr = getAddrReg<Long>(reg);
		is_inst_legal = true;
	    }
	}
	break;
	case 4:
	{
	    if (testbit(mask, 4))
	    {
		temp_addr = getAddrReg<Long>(reg);
		is_inst_legal = true;
	    }
	}
	break;
	case 5:
	{
	    if (testbit(mask, 5))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint16_t ext_word = extension<Word>(pc);

		uint32_t displacement = clip<Long>(sign<Word>(ext_word));

		temp_addr = (addr_reg + displacement);
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
			uint16_t ext_word = extension<Word>(pc);
			temp_addr = clip<Long>(sign<Word>(ext_word));
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 8))
		    {
			temp_addr = extension<Long>(pc);
			is_inst_legal = true;
		    }
		}
		break;
		case 2:
		{
		    if (testbit(mask, 9))
		    {
			uint32_t pc_val = pc;
			uint16_t ext_word = extension<Word>(pc);

			uint32_t displacement = clip<Long>(sign<Word>(ext_word));

			temp_addr = (pc_val + displacement);
			is_inst_legal = true;
		    }
		}
		break;
		default: break;
	    }
	}
    }

    if (is_inst_legal)
    {
	stream << "$" << hex << int(temp_addr);
	offset = (pc - prev_pc);
    }
    else
    {
	stream << "und";
	offset = 0;
    }

    return offset;
}

template<int Size, uint16_t mask = AllAddr>
auto dstmodedasm(int mode, int reg, ostream &stream, uint32_t &pc) -> size_t
{
    mode &= 7;
    reg &= 7;

    size_t offset = 0;

    size_t prev_pc = pc;

    bool is_inst_legal = false;

    stringstream ss;

    switch (mode)
    {
	case 0:
	{
	    if (testbit(mask, 0))
	    {
		ss << "d" << dec << reg;
		is_inst_legal = true;
	    }
	}
	break;
	case 1:
	{
	    if (testbit(mask, 1) && (Size != Byte))
	    {
		ss << "a" << dec << reg;
		is_inst_legal = true;
	    }
	}
	break;
	case 2:
	{
	    if (testbit(mask, 2))
	    {
		ss << "(a" << dec << reg << ")";
		is_inst_legal = true;
	    }
	}
	break;
	case 3:
	{
	    if (testbit(mask, 3))
	    {
		ss << "(a" << dec << reg << ")+";
		is_inst_legal = true;
	    }
	}
	break;
	case 4:
	{
	    if (testbit(mask, 4))
	    {
		ss << "-(a" << dec << reg << ")";
		is_inst_legal = true;
	    }
	}
	break;
	case 5:
	{
	    if (testbit(mask, 5))
	    {
		uint32_t addr_reg = getAddrReg<Long>(reg);
		uint16_t ext_word = extension<Word>(pc);

		uint32_t displacement = clip<Long>(sign<Word>(ext_word));

		ss << "$" << hex << int(addr_reg + displacement);
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
			uint16_t ext_word = extension<Word>(pc);
			uint32_t addr = clip<Long>(sign<Word>(ext_word));
			ss << "$" << hex << int(addr);
			is_inst_legal = true;
		    }
		}
		break;
		case 1:
		{
		    if (testbit(mask, 8))
		    {
			uint32_t addr = extension<Long>(pc);
			ss << "$" << hex << int(addr);
			is_inst_legal = true;
		    }
		}
		break;
		default: break;
	    }
	}
	break;
	default: break;
    }

    if (is_inst_legal)
    {
	stream << ss.str();
	offset = (prev_pc - pc);
    }
    else
    {
	stream << "und ";

	if (mode == 7)
	{
	    stream << "m7, r" << dec << reg;
	}
	else
	{
	    stream << "m" << dec << mode;
	}

	offset = 0;
    }

    return offset;
}

auto m68kdis_unknown(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "undefined";
    return 0;
}

auto m68kdis_andi_to_sr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint16_t bitmask_val = extension<Word>(pc);
    stream << "andi #" << hex << int(bitmask_val) << ", SR";
    return 4;
}

auto m68kdis_eori_to_ccr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint8_t bitmask_val = extension<Byte>(pc);
    stream << "eori #" << hex << int(bitmask_val) << ", CCR";
    return 4;
}

auto m68kdis_eori_to_sr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint16_t bitmask_val = extension<Word>(pc);
    stream << "eori #" << hex << int(bitmask_val) << ", SR";
    return 4;
}

auto m68kdis_ori_to_ccr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint8_t bitmask_val = extension<Byte>(pc);
    stream << "ori #" << hex << int(bitmask_val) << ", CCR";
    return 4;
}

auto m68kdis_ori_to_sr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint16_t bitmask_val = extension<Word>(pc);
    stream << "ori #" << hex << int(bitmask_val) << ", SR";
    return 4;
}

auto m68kdis_andi_to_ccr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint8_t bitmask_val = extension<Byte>(pc);
    stream << "andi #" << hex << int(bitmask_val) << ", CCR";
    return 4;
}

auto m68kdis_move_from_sr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream src_str;

    size_t offset = 2;
    offset += dstmodedasm<Word, DataAltAddr>(srcmode, srcreg, src_str, pc);

    stream << "move SR, " << src_str.str();
    return offset;
}

auto m68kdis_move_to_ccr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream src_str;

    size_t offset = 2;
    offset += srcmodedasm<Word, DataAddr>(srcmode, srcreg, src_str, pc);

    stream << "move " << src_str.str() << ", CCR";
    return offset;
}

auto m68kdis_move_to_sr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream src_str;

    size_t offset = 2;
    offset += srcmodedasm<Word, DataAddr>(srcmode, srcreg, src_str, pc);

    stream << "move " << src_str.str() << ", SR";
    return offset;
}

auto m68kdis_move_usp(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    bool is_reg = testbit(instr, 3);
    int reg = getsrcreg(instr);

    if (is_reg)
    {
	stream << "move usp, a" << dec << reg;
    }
    else
    {
	stream << "move a" << dec << reg << ", usp";
    }

    return 2;
}

template<int Size>
auto m68kdis_move(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    int dstmode = getdstmode(instr);
    int dstreg = getdstreg(instr);

    stream << "move";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream src_str;
    stringstream dst_str;

    size_t offset = 2;
    offset += srcmodedasm<Size>(srcmode, srcreg, src_str, pc);
    offset += dstmodedasm<Size>(dstmode, dstreg, dst_str, pc);

    stream << " " << src_str.str() << ", " << dst_str.str();

    return offset;
}

auto m68kdis_movem(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint16_t reglist = extension<Word>(pc);
    (void)reglist;
    stream << "movem";
    return 0;
}

auto m68kdis_moveq(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    stream << "moveq #$" << hex << int(instr & 0xFF) << ", d" << getdstreg(instr);
    return 2;
}

auto m68kdis_abcd(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "abcd";
    return 2;
}

auto m68kdis_sbcd(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "sbcd";
    return 2;
}

template<int Size>
auto m68kdis_add(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_add_internal<Size, false>(stream, pc, instr);
}

template<int Size>
auto m68kdis_addr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_add_internal<Size, true>(stream, pc, instr);
}

template<int Size, bool is_rev = false>
auto m68kdis_add_internal(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "add";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, addr_mode_mask>(srcmode, srcreg, mode_str, pc);

    if (is_rev)
    {
	stream << " d" << dec << dstreg << ", " << mode_str.str();
    }
    else
    {
	stream << " " << mode_str.str() << ", d" << dec << dstreg;
    }

    return offset;
}

template<int Size>
auto m68kdis_adda(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "adda";

    switch (Size)
    {
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size>(srcmode, srcreg, mode_str, pc);

    stream << " " << mode_str.str() << ", a" << dec << dstreg;

    return offset;
}

template<int Size>
auto m68kdis_addq(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    stream << "addq";

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    int imm_val = getdstreg(instr);

    // The immediate value of 0 represents a value of 8
    if (imm_val == 0)
    {
	imm_val = 8;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, AlterableAddr>(srcmode, srcreg, mode_str, pc);

    stream << " #" << dec << imm_val << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_sub(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_sub_internal<Size, false>(stream, pc, instr);
}

template<int Size>
auto m68kdis_subr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_sub_internal<Size, true>(stream, pc, instr);
}

template<int Size, bool is_rev = false>
auto m68kdis_sub_internal(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "sub";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, addr_mode_mask>(srcmode, srcreg, mode_str, pc);

    if (is_rev)
    {
	stream << " d" << dec << dstreg << ", " << mode_str.str();
    }
    else
    {
	stream << " " << mode_str.str() << ", d" << dec << dstreg;
    }

    return offset;
}

template<int Size>
auto m68kdis_suba(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "suba";

    switch (Size)
    {
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size>(srcmode, srcreg, mode_str, pc);

    stream << " " << mode_str.str() << ", a" << dec << dstreg;

    return offset;
}

template<int Size>
auto m68kdis_subq(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    stream << "subq";

    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    int imm_val = getdstreg(instr);

    // The immediate value of 0 represents a value of 8
    if (imm_val == 0)
    {
	imm_val = 8;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, AlterableAddr>(srcmode, srcreg, mode_str, pc);

    stream << " #" << dec << imm_val << ", " << mode_str.str();

    return offset;
}

auto m68kdis_ext(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    int opmode = getdstmode(instr);

    stream << "ext";

    switch (opmode)
    {
	case 0b010: stream << ".w"; break;
	case 0b011: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stream << " d" << getsrcreg(instr);
    return 2;
}

auto m68kdis_exgdreg(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    stream << "exg.l d" << getdstreg(instr) << ", d" << getsrcreg(instr);
    return 2;
}

auto m68kdis_exgareg(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    stream << "exg.l a" << getdstreg(instr) << ", a" << getsrcreg(instr);
    return 2;
}

auto m68kdis_exgdareg(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    stream << "exg.l d" << getdstreg(instr) << ", a" << getsrcreg(instr);
    return 2;
}

auto m68kdis_lea(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream addr_str;

    size_t offset = 2;
    offset += rawmodedasm<Long, ControlAddr>(srcmode, srcreg, addr_str, pc);

    stream << "lea " << addr_str.str() << ", a" << dec << dstreg;
    return offset;
}

auto m68kdis_pea(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream addr_str;

    size_t offset = 2;
    offset += rawmodedasm<Long, ControlAddr>(srcmode, srcreg, addr_str, pc);

    stream << "pea " << addr_str.str();
    return offset;
}

auto m68kdis_jsr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream addr_str;

    size_t offset = 2;
    offset += rawmodedasm<Long, ControlAddr>(srcmode, srcreg, addr_str, pc);

    stream << "jsr " << addr_str.str();
    return offset;
}

auto m68kdis_jmp(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stringstream addr_str;

    size_t offset = 2;
    offset += rawmodedasm<Long, ControlAddr>(srcmode, srcreg, addr_str, pc);

    stream << "jmp " << addr_str.str();
    return offset;
}

auto m68kdis_bra(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    uint8_t byte_dis = (instr & 0xFF);

    uint32_t pc_val = pc;

    if (byte_dis == 0)
    {
	uint16_t word_dis = extension<Word>(pc);
	pc_val += int16_t(word_dis);
    }
    else
    {
	pc_val += int8_t(byte_dis);
    }

    stream << "bra $" << hex << pc_val;
    return 0;
}

auto m68kdis_bsr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    uint8_t byte_dis = (instr & 0xFF);

    uint32_t pc_val = pc;

    if (byte_dis == 0)
    {
	uint16_t word_dis = extension<Word>(pc);
	pc_val += int16_t(word_dis);
    }
    else
    {
	pc_val += int8_t(byte_dis);
    }

    stream << "bsr $" << hex << pc_val;
    return 0;
}

auto m68kdis_bcc(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    uint8_t byte_dis = (instr & 0xFF);

    uint32_t pc_val = pc;

    if (byte_dis == 0)
    {
	uint16_t word_dis = extension<Word>(pc);
	pc_val += int16_t(word_dis);
    }
    else
    {
	pc_val += int8_t(byte_dis);
    }

    stream << "bcc $" << hex << pc_val;
    return 0;
}

auto m68kdis_dbcc(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "dbcc";
    return 0;
}

auto m68kdis_rts(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "rts";
    return 2;
}

auto m68kdis_rtr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "rtr";
    return 2;
}

auto m68kdis_rte(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "rte";
    return 2;
}

template<int Size>
auto m68kdis_and(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_and_internal<Size, false>(stream, pc, instr);
}

template<int Size>
auto m68kdis_andr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_and_internal<Size, true>(stream, pc, instr);
}

template<int Size, bool is_rev = false>
auto m68kdis_and_internal(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "and";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, addr_mode_mask>(srcmode, srcreg, mode_str, pc);

    if (is_rev)
    {
	stream << " d" << dec << dstreg << ", " << mode_str.str();
    }
    else
    {
	stream << " " << mode_str.str() << ", d" << dec << dstreg;
    }

    return offset;
}

template<int Size>
auto m68kdis_or(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_or_internal<Size, false>(stream, pc, instr);
}

template<int Size>
auto m68kdis_orr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    return m68kdis_or_internal<Size, true>(stream, pc, instr);
}

template<int Size, bool is_rev>
auto m68kdis_or_internal(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    constexpr uint16_t addr_mode_mask = is_rev ? MemAltAddr : DataAddr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "or";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, addr_mode_mask>(srcmode, srcreg, mode_str, pc);

    if (is_rev)
    {
	stream << " d" << dec << dstreg << ", " << mode_str.str();
    }
    else
    {
	stream << " " << mode_str.str() << ", d" << dec << dstreg;
    }

    return offset;
}

template<int Size>
auto m68kdis_cmp(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "cmp";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, DataAddr>(srcmode, srcreg, mode_str, pc);
    stream << " " << mode_str.str() << ", d" << dec << dstreg;
    return offset;
}

template<int Size>
auto m68kdis_cmpa(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "cmpa";

    switch (Size)
    {
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size>(srcmode, srcreg, mode_str, pc);

    stream << " " << mode_str.str() << ", a" << dec << dstreg;

    return offset;
}

template<int Size>
auto m68kdis_tst(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "tst";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, DataAddr>(srcmode, srcreg, mode_str, pc);
    stream << " " << mode_str.str();
    return offset;
}

template<int Size>
auto m68kdis_neg(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    stream << "neg";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;

    size_t offset = 2;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);
    
    stream << " " << mode_str.str();
    return offset;
};

template<int Size>
auto m68kdis_not(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    stream << "not";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;

    size_t offset = 2;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);
    
    stream << " " << mode_str.str();
    return offset;
};

template<int Size>
auto m68kdis_addi(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stream << "addi";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = (Size == Long) ? 6 : 4;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " #$" << hex << int(imm_val) << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_subi(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stream << "subi";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = (Size == Long) ? 6 : 4;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " #$" << hex << int(imm_val) << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_cmpi(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stream << "cmpi";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = (Size == Long) ? 6 : 4;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " #$" << hex << int(imm_val) << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_andi(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stream << "andi";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = (Size == Long) ? 6 : 4;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " #$" << hex << int(imm_val) << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_ori(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stream << "ori";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = (Size == Long) ? 6 : 4;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " #$" << hex << int(imm_val) << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_eor(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    stream << "eor";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Size, DataAltAddr>(srcmode, srcreg, mode_str, pc);

    stream << " d" << dec << dstreg << ", " << mode_str.str();
    return offset;
}

template<int Size>
auto m68kdis_eori(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stream << "eori";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;
    size_t offset = (Size == Long) ? 6 : 4;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " #$" << hex << int(imm_val) << ", " << mode_str.str();

    return offset;
}

template<int Size>
auto m68kdis_asl(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "asl";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_asr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "asr";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_lsl(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "lsl";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_lsr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "lsr";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_rol(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "rol";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_ror(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "ror";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_roxl(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "roxl";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

template<int Size>
auto m68kdis_roxr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "roxr";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    return 0;
}

auto m68kdis_swap(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    stream << "swap d" << getsrcreg(instr);
    return 2;
}

auto m68kdis_scc(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    stream << "scc";

    stringstream mode_str;

    size_t offset = 2;
    offset += dstmodedasm<Byte, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " " << mode_str.str();
    return offset;
}

template<int Size>
auto m68kdis_clear(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);

    stream << "clr";

    switch (Size)
    {
	case Byte: stream << ".b"; break;
	case Word: stream << ".w"; break;
	case Long: stream << ".l"; break;
	default: stream << ".u"; break;
    }

    stringstream mode_str;

    size_t offset = 2;
    offset += dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, mode_str, pc);

    stream << " " << mode_str.str();
    return offset;
}

auto m68kdis_bchg(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "bchg Dn, <ea>";
    return 0;
}

auto m68kdis_bset(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "bset Dn, <ea>";
    return 0;
}

auto m68kdis_bclr(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "bclr Dn, <ea>";
    return 0;
}

auto m68kdis_btst(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "btst Dn, <ea>";
    return 0;
}

auto m68kdis_bchgimm(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    stream << "bchg";

    stringstream mode_str;

    size_t offset = 4;
    offset += dstmodedasm<Byte, DataAddr>(dstmode, dstreg, mode_str, pc);

    stream << ((dstmode == 0) ? ".l" : ".b");
    stream << " #" << dec << bit_num;
    stream << ", " << mode_str.str();
    return offset;
}

auto m68kdis_bsetimm(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    stream << "bset";

    stringstream mode_str;

    size_t offset = 4;
    offset += dstmodedasm<Byte, DataAddr>(dstmode, dstreg, mode_str, pc);

    stream << ((dstmode == 0) ? ".l" : ".b");
    stream << " #" << dec << bit_num;
    stream << ", " << mode_str.str();
    return offset;
}

auto m68kdis_bclrimm(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    stream << "bclr";

    stringstream mode_str;

    size_t offset = 4;
    offset += dstmodedasm<Byte, DataAddr>(dstmode, dstreg, mode_str, pc);

    stream << ((dstmode == 0) ? ".l" : ".b");
    stream << " #" << dec << bit_num;
    stream << ", " << mode_str.str();
    return offset;
}

auto m68kdis_btstimm(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    stream << "btst";

    stringstream mode_str;

    size_t offset = 4;
    offset += dstmodedasm<Byte, DataAddr>(dstmode, dstreg, mode_str, pc);

    stream << ((dstmode == 0) ? ".l" : ".b");
    stream << " #" << dec << bit_num;
    stream << ", " << mode_str.str();
    return offset;
}

auto m68kdis_mulu(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stream << "mulu.w";

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Word, DataAddr>(srcmode, srcreg, mode_str, pc);
    stream << " " << mode_str.str() << ", d" << dec << dstreg;
    return offset;
}

auto m68kdis_divu(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stream << "divu.w";

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Word, DataAddr>(srcmode, srcreg, mode_str, pc);
    stream << " " << mode_str.str() << ", d" << dec << dstreg;
    return offset;
}

auto m68kdis_divs(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    stream << "divs.w";

    stringstream mode_str;
    size_t offset = 2;
    offset += srcmodedasm<Word, DataAddr>(srcmode, srcreg, mode_str, pc);
    stream << " " << mode_str.str() << ", d" << dec << dstreg;
    return offset;
}

auto m68kdis_trap(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    stream << "trap #" << dec << int(instr & 0xF);
    return 2;
}

auto m68kdis_nop(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)pc;
    (void)instr;
    stream << "nop";
    return 2;
}

auto m68kdis_stop(ostream &stream, uint32_t pc, uint16_t instr) -> size_t
{
    (void)instr;
    uint16_t temp = extension<Word>(pc);
    stream << "stop #$" << hex << int(temp);
    return 4;
}