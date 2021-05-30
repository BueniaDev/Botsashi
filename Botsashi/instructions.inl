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
	case 0: return clip<Size>(m68kreg.datareg[reg]); break;
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

auto m68k_unknown() -> void
{
    unrecognizedinstr(currentinstr);
}

template<int Size> 
auto m68k_move() -> void
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
}

template<int Size> 
auto m68k_add() -> void
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
}

template<int Size> 
auto m68k_addr() -> void
{
    // uint16_t instr = currentinstr;
    cout << "Reversed operands" << endl;
    exit(0);
}

template<int Size> 
auto m68k_sub() -> void
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
}

template<int Size> 
auto m68k_subr() -> void
{
    // uint16_t instr = currentinstr;
    cout << "Reversed operands" << endl;
    exit(0);
}

auto m68k_lea() -> void
{
    uint16_t instr = currentinstr;
    int dstreg = getdstreg(instr);
    int srcmode = getsrcmode(instr);
    int srcreg = getsrcreg(instr);

    uint32_t temp = loadaddrmode(srcmode, srcreg);
    m68kreg.addrreg[dstreg] = temp;
}

auto m68k_swap() -> void
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
}

auto m68k_exgdreg() -> void
{
    uint16_t instr = currentinstr;
    int regxnum = getdstreg(instr);
    int regynum = getsrcreg(instr);

    uint32_t oldregx = m68kreg.datareg[regxnum];
    m68kreg.datareg[regxnum] = m68kreg.datareg[regynum];
    m68kreg.datareg[regynum] = oldregx;
}

template<int Size>
auto m68k_clear() -> void
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
    dstaddrmode<Size, Clear>(dstmode, dstreg, 0);
}

auto m68k_trap() -> void
{
    uint16_t instr = currentinstr;
    int val = (instr & 0xF);

    if (istrapOverride(val))
    {
	trapException(val);
	return;
    }

    cout << "Unimplemented: Low-level trap exception handling" << endl;
    exit(0);
}

auto m68k_stop() -> void
{
    uint16_t stopimm = extension<Word>(m68kreg.pc);
    // Set status register to value of stopimm
    m68kreg.statusreg = bitset<16>(stopimm);
    // Stop program execution
    stopFunction();
}