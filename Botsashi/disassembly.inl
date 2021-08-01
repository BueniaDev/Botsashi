template<int Size, int Flags = 0>
auto srcmodedasm(int mode, int reg, uint32_t &pc) -> string
{
    mode &= 7;
    reg &= 7;

    if ((Size == Byte) && (mode == 1))
    {
	return "und";
    }

    stringstream ss;
    switch (mode)
    {
	case 0: ss << "d" << dec << (int)(reg); break;
	case 2: ss << "(a" << dec << (int)(reg) << ")"; break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t temp = extension<Word>(pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(temp));

		    ss << "$" << setfill('0') << setw(8) << hex << (int)(addr);
		}
		break;
		case 4: ss << "#$" << setfill('0') << setw((Size << 1)) << hex << (int)(extension<Size>(pc)); break;
		default: ss << "m7und"; break;
	    }
	}
	break;
	default: ss << "und"; break;
    }

    return ss.str();
}

template<int Size, int Flags = 0>
auto dstmodedasm(int mode, int reg, uint32_t &pc) -> string
{
    mode &= 7;
    reg &= 7;

    if ((Size == Byte) && (mode == 1))
    {
	return "und";
    }

    stringstream ss;
    switch (mode)
    {
	case 0: ss << "d" << dec << (int)(reg); break;
	case 1: ss << "a" << dec << (int)(reg); break;
	case 2: ss << "(a" << dec << (int)(reg) << ")"; break;
	case 7:
	{
	    switch (reg)
	    {
		case 0:
		{
		    // Fetch extension word
		    uint16_t temp = extension<Word>(pc);

		    // Sign-extended word to 32-bits
		    uint32_t addr = clip<Long>(sign<Word>(temp));

		    ss << "$" << setfill('0') << setw(8) << hex << (int)(addr);
		}
		break;
		default: ss << "m7und"; break;
	    }
	}
	break;
	default: ss << "und"; break;
    }

    return ss.str();
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
    ss << "," << dstmodedasm<Size>(getdstmode(instr), getdstreg(instr), pc);

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
    ss << ",d" << dec << (int)getdstreg(instr);

    return ss.str();
}

template<int Size>
auto m68kdis_addr(uint32_t pc, uint16_t instr) -> string
{
    return "addr";
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
    ss << ",d" << dec << (int)getdstreg(instr);

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
    ss << "exg.l d" << getdstreg(instr) << ",d" << getsrcreg(instr);
    return ss.str();
}

auto m68kdis_lea(uint32_t pc, uint16_t instr) -> string
{
    return "lea";
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

    ss << " " << dstmodedasm<Size, Clear>(getsrcmode(instr), getsrcreg(instr), pc);
    return ss.str();
}

auto m68kdis_bclrimm(uint32_t pc, uint16_t instr) -> string
{
    return "bclr.imm";
}

auto m68kdis_trap(uint32_t pc, uint16_t instr) -> string
{
    stringstream ss;
    ss << "trap #" << dec << (int)(instr & 0xF);
    return ss.str();
}

auto m68kdis_stop(uint32_t pc, uint16_t instr) -> string
{
    uint16_t temp = extension<Word>(pc);
    stringstream ss;
    
    ss << "stop #$" << hex << (int)(temp);
    return ss.str();
}