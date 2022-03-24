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

auto Botsashi::getsrcmode(uint16_t instr) -> int
{
    return ((instr >> 3) & 0x7);
}

auto Botsashi::getsrcreg(uint16_t instr) -> int
{
    return (instr & 0x7);
}

auto Botsashi::getdstmode(uint16_t instr) -> int
{
    return ((instr >> 6) & 0x7);
}

auto Botsashi::getdstreg(uint16_t instr) -> int
{
    return ((instr >> 9) & 0x7);
}

auto Botsashi::getopcond(uint16_t instr) -> int
{
    return ((instr >> 8) & 0xF);
}

bool Botsashi::ismodesupervisor()
{
    return testbit(m68kreg.statusreg, 13);
}

void Botsashi::set_supervisor_flag(bool is_set)
{
    setStatusReg(changebit(m68kreg.statusreg, 13, is_set));
}

void Botsashi::set_trace_flag(bool is_set)
{
    setStatusReg(changebit(m68kreg.statusreg, 15, is_set));
}

int Botsashi::get_irq_mask()
{
    return ((m68kreg.statusreg >> 8) & 0x7);
}

void Botsashi::set_irq_mask(int value)
{
    uint16_t new_status_reg = ((m68kreg.statusreg & ~0x700) | ((value & 0x7) << 8));
    setStatusReg(new_status_reg);
}

bool Botsashi::iscarry()
{
    return testbit(m68kreg.statusreg, 0);
}

bool Botsashi::isoverflow()
{
    return testbit(m68kreg.statusreg, 1);
}

bool Botsashi::iszero()
{
    return testbit(m68kreg.statusreg, 2);
}

bool Botsashi::issign()
{
    return testbit(m68kreg.statusreg, 3);
}

bool Botsashi::isextend()
{
    return testbit(m68kreg.statusreg, 4);
}

void Botsashi::setcarry(bool val)
{
    m68kreg.statusreg = changebit(m68kreg.statusreg, 0, val);
}

void Botsashi::setoverflow(bool val)
{
    m68kreg.statusreg = changebit(m68kreg.statusreg, 1, val);
}

void Botsashi::setzero(bool val)
{
    m68kreg.statusreg = changebit(m68kreg.statusreg, 2, val);
}

void Botsashi::setsign(bool val)
{
    m68kreg.statusreg = changebit(m68kreg.statusreg, 3, val);
}

void Botsashi::setextend(bool val)
{
    m68kreg.statusreg = changebit(m68kreg.statusreg, 4, val);
}

auto Botsashi::count_bits(uint64_t source) -> uint32_t
{
    uint32_t count = 0;
    while (source > 0)
    {
	source = resetbit(source, count);
	count += 1;
    }

    return count;
}

template<int Size>
auto Botsashi::getZero(uint32_t temp, bool is_extend) -> bool
{
    bool is_val_zero = (clip<Size>(temp) == 0);

    if (is_extend && is_val_zero)
    {
	// Zero flag is unchanged in ADDX/SUBX mode
	return iszero();
    }
    else
    {
	return is_val_zero;
    }
}

template<int Size>
auto Botsashi::getSign(uint32_t temp) -> bool
{
    return (sign<Size>(temp) < 0);
}

template<typename T>
bool Botsashi::testbit(T reg, int bit)
{
    return ((reg >> bit) & 1) ? true : false;
}

template<typename T>
T Botsashi::setbit(T reg, int bit)
{
    return (reg | (1 << bit));
}

template<typename T>
T Botsashi::resetbit(T reg, int bit)
{
    return (reg & ~(1 << bit));
}

template<typename T>
T Botsashi::changebit(T reg, int bit, bool val)
{
    return (val) ? setbit(reg, bit) : resetbit(reg, bit);
}

template<typename T>
T Botsashi::togglebit(T reg, int bit)
{
    return testbit(reg, bit) ? resetbit(reg, bit) : setbit(reg, bit);
}