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

    
#ifndef BOTSASHI_API_H
#define BOTSASHI_API_H

#if defined(_MSC_VER) && !defined(BOTSASHI_STATIC)
    #ifdef BOTSASHI_EXPORTS
        #define BOTSASHI_API __declspec(dllexport)
    #else
        #define BOTSASHI_API __declspec(dllimport)
    #endif // BOTSASHI_EXPORTS
#else
    #define BOTSASHI_API
#endif // _MSC_VER

#endif // BOTSASHI_API_H
