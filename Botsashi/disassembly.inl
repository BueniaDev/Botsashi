template<int Size, uint16_t mask = AllAddr>
auto srcmodedasm(int mode, int reg, uint32_t &pc) -> string
{
    mode &= 7;
    reg &= 7;

    stringstream ss;

    bool is_inst_legal = false;

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
	return ss.str();
    }
    else
    {
	ss << "und ";

	if (mode == 7)
	{
	    ss << "m7, r" << dec << reg;
	}
	else
	{
	    ss << "m" << dec << mode;
	}

	return ss.str();
    }
}

template<int Size, uint16_t mask = AllAddr>
auto dstmodedasm(int mode, int reg, uint32_t &pc) -> string
{
    mode &= 7;
    reg &= 7;

    stringstream ss;

    bool is_inst_legal = false;

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
	return ss.str();
    }
    else
    {
	ss << "und ";

	if (mode == 7)
	{
	    ss << "m7, r" << dec << reg;
	}
	else
	{
	    ss << "m" << dec << mode;
	}

	return ss.str();
    }
}

auto m68kdis_unknown(uint32_t pc, uint16_t instr) -> string
{
    return "undefined";
}

template<int Size>
auto m68kdis_move(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;

    if (getdstmode(instr) == 1)
    {
	ss << "movea";
    }
    else
    {
	ss << "move";
    }
    
    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " " << srcmodedasm<Size>(getsrcmode(instr), getsrcreg(instr), pc);
    ss << ", " << dstmodedasm<Size>(getdstmode(instr), getdstreg(instr), pc);

    return ss.str();
}

auto m68kdis_moveq(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "moveq #$" << hex << int(instr & 0xFF) << ", d" << getdstreg(instr);
    return ss.str();
}

template<int Size>
auto m68kdis_add(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "add";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " " << srcmodedasm<Size>(getsrcmode(instr), getsrcreg(instr), pc);
    ss << ", d" << getdstreg(instr);

    return ss.str();
}

template<int Size>
auto m68kdis_addr(uint32_t pc, uint16_t instr) -> string
{
    return "addr";
}

template<int Size>
auto m68kdis_addq(uint32_t pc, uint16_t instr) -> string
{
    return "addq";
}

template<int Size>
auto m68kdis_sub(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "sub";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " " << srcmodedasm<Size>(getsrcmode(instr), getsrcreg(instr), pc);
    ss << ", d" << getdstreg(instr);

    return ss.str();
}

template<int Size>
auto m68kdis_subr(uint32_t pc, uint16_t instr) -> string
{
    return "subr";
}

auto m68kdis_exgdreg(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "exg.l d" << getdstreg(instr) << ", d" << getsrcreg(instr);
    return ss.str();
}

auto m68kdis_lea(uint32_t pc, uint16_t instr) -> string
{
    return "lea";
}

auto m68kdis_jsr(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    ss << "jsr " << dstmodedasm<Long, ControlAddr>(srcmode, srcreg, pc);
    return ss.str();
}

auto m68kdis_jmp(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);
    ss << "jmp " << dstmodedasm<Long, ControlAddr>(srcmode, srcreg, pc);
    return ss.str();
}

auto m68kdis_rts(uint32_t pc, uint16_t instr) -> string
{
    return "rts";
}

template<int Size>
auto m68kdis_and(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "and";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    return ss.str();
}

template<int Size>
auto m68kdis_or(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "or";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    return ss.str();
}

template<int Size>
auto m68kdis_not(uint32_t pc, uint16_t instr) -> string
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    stringstream ss;
    ss << "not";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " " << dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, pc);

    return ss.str();
};

template<int Size>
auto m68kdis_addi(uint32_t pc, uint16_t instr) -> string
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stringstream ss;
    ss << "addi";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " #$" << hex << int(imm_val) << ", " << dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, pc);

    return ss.str();
}


template<int Size>
auto m68kdis_andi(uint32_t pc, uint16_t instr) -> string
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stringstream ss;
    ss << "andi";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " #$" << hex << int(imm_val) << ", " << dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, pc);

    return ss.str();
}

template<int Size>
auto m68kdis_ori(uint32_t pc, uint16_t instr) -> string
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stringstream ss;
    ss << "ori";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " #$" << hex << int(imm_val) << ", " << dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, pc);

    return ss.str();
}

template<int Size>
auto m68kdis_eori(uint32_t pc, uint16_t instr) -> string
{
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    auto imm_val = extension<Size>(pc);

    stringstream ss;
    ss << "eori";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " #$" << hex << int(imm_val) << ", " << dstmodedasm<Size, DataAltAddr>(dstmode, dstreg, pc);

    return ss.str();
}

auto m68kdis_swap(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "swap d" << getsrcreg(instr);
    return ss.str();
}

template<int Size>
auto m68kdis_clear(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "clr";

    switch (Size)
    {
	case Byte: ss << ".b"; break;
	case Word: ss << ".w"; break;
	case Long: ss << ".l"; break;
	default: ss << ".u"; break;
    }

    ss << " " << dstmodedasm<Size, DataAltAddr>(getsrcmode(instr), getsrcreg(instr), pc);
    return ss.str();
}

auto m68kdis_bchg(uint32_t pc, uint16_t instr) -> string
{
    return "bchg Dn, <ea>";
}

auto m68kdis_bset(uint32_t pc, uint16_t instr) -> string
{
    return "bset Dn, <ea>";
}

auto m68kdis_bclr(uint32_t pc, uint16_t instr) -> string
{
    return "bclr Dn, <ea>";
}

auto m68kdis_bchgimm(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    ss << "bchg";

    ss << ((dstmode == 0) ? ".l" : ".b");
    ss << " #" << bit_num;
    ss << ", " << dstmodedasm<Byte, DataAddr>(dstmode, dstreg, pc);

    return ss.str();
}

auto m68kdis_bsetimm(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    ss << "bset";

    ss << ((dstmode == 0) ? ".l" : ".b");
    ss << " #" << bit_num;
    ss << ", " << dstmodedasm<Byte, DataAddr>(dstmode, dstreg, pc);

    return ss.str();
}

auto m68kdis_bclrimm(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    int dstmode = getsrcmode(instr);
    int dstreg = getsrcreg(instr);
    int bit_num = extension<Byte>(pc);
    bit_num &= (dstmode == 0) ? 31 : 7; // Mask to 32-bits for EA mode 0, and to 8-bits for the others
    ss << "bclr";

    ss << ((dstmode == 0) ? ".l" : ".b");
    ss << " #" << bit_num;
    ss << ", " << dstmodedasm<Byte, DataAddr>(dstmode, dstreg, pc);

    return ss.str();
}

auto m68kdis_mulu(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "mulu.w";
    return ss.str();
}

auto m68kdis_trap(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "trap #" << dec << int(instr & 0xF);
    return ss.str();
}

auto m68kdis_stop(uint32_t pc, uint16_t instr) -> string
{
    uint16_t temp = extension<Word>(pc);
    stringstream ss;
    
    ss << "stop #$" << hex << int(temp);
    return ss.str();
}