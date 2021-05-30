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

template<> auto Botsashi::msb<Byte>() -> uint32_t
{
    return 0x80;
}

template<> auto Botsashi::msb<Word>() -> uint32_t
{
    return 0x8000;
}

template<> auto Botsashi::msb<Long>() -> uint32_t
{
    return 0x80000000;
}

template<> auto Botsashi::clip<Byte>(uint32_t data) -> uint32_t
{
    return static_cast<uint8_t>(data);
}

template<> auto Botsashi::clip<Word>(uint32_t data) -> uint32_t
{
    return static_cast<uint16_t>(data);
}

template<> auto Botsashi::clip<Long>(uint32_t data) -> uint32_t
{
    return static_cast<uint32_t>(data);
}

template<> auto Botsashi::sign<Byte>(uint32_t data) -> int32_t
{
    return static_cast<int8_t>(data);
}

template<> auto Botsashi::sign<Word>(uint32_t data) -> int32_t
{
    return static_cast<int16_t>(data);
}

template<> auto Botsashi::sign<Long>(uint32_t data) -> int32_t
{
    return static_cast<int32_t>(data);
}

template<> auto Botsashi::mask<Byte>() -> uint32_t
{
    return 0xFF;
}

template<> auto Botsashi::mask<Word>() -> uint32_t
{
    return 0xFFFF;
}

template<> auto Botsashi::mask<Long>() -> uint32_t
{
    return 0xFFFFFFFF;
}