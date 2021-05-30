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

auto Botsashi::interRead(bool upper, bool lower, uint32_t addr) -> uint16_t
{
    // Clip address to 24-bits, as on the real 68000
    addr &= 0xFFFFFF;
    if (inter)
    {
	return inter->readWord(upper, lower, addr);
    }

    return clip<Word>(-1);
}

auto Botsashi::interWrite(bool upper, bool lower, uint32_t addr, uint16_t val) -> void
{
    // Clip address to 24-bits, as on the real 68000
    addr &= 0xFFFFFF;
    if (inter)
    {
	return inter->writeWord(upper, lower, addr, val);
    }
}