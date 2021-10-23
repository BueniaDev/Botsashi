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

template<int Size> 
auto msb() -> uint32_t
{
    switch (Size)
    {
	case Byte: return 0x80; break;
	case Word: return 0x8000; break;
	case Long: return 0x80000000; break;
    }
}

template<int Size>
auto clip(uint32_t data) -> uint32_t
{
    switch (Size)
    {
	case Byte: return uint8_t(data); break;
	case Word: return uint16_t(data); break;
	case Long: return uint32_t(data); break;
    }
}

template<int Size>
auto sign(uint32_t data) -> int32_t
{
    switch (Size)
    {
	case Byte: return int8_t(data); break;
	case Word: return int16_t(data); break;
	case Long: return int32_t(data); break;
    }
}

template<int Size>
auto mask() -> uint32_t
{
    switch (Size)
    {
	case Byte: return 0xFF; break;
	case Word: return 0xFFFF; break;
	case Long: return 0xFFFFFFFF; break;
    }
}