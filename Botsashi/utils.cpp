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

bool Botsashi::iscarry()
{
    return m68kreg.statusreg.test(0);
}

void Botsashi::setcarry(bool val)
{
    m68kreg.statusreg.set(0, val);
}

void Botsashi::setoverflow(bool val)
{
    m68kreg.statusreg.set(1, val);
}

void Botsashi::setzero(bool val)
{
    m68kreg.statusreg.set(2, val);
}

void Botsashi::setsign(bool val)
{
    m68kreg.statusreg.set(3, val);
}

void Botsashi::setextend(bool val)
{
    m68kreg.statusreg.set(4, val);
}

template<int Size> auto Botsashi::getZero(uint32_t temp) -> bool
{
    return (clip<Size>(temp) == 0);
}

template<int Size> auto Botsashi::getSign(uint32_t temp) -> bool
{
    return (sign<Size>(temp) < 0);
}