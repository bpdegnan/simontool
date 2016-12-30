/*
Copyright (C) 2015-2017  Brian Degnan http://degnan68k.blogspot.com/

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "headers.h"

#define FMT_BUF_SIZE (8*sizeof(uintmax_t)+1)

#define SETFLAG(status,flag)({status=status | flag;})
#define GETFLAG(status,flag)(status&flag)

//there was not stricmp in the string.h lib.
i32 simon_stricmp(const char *a, const char *b);

u32 util_validroundsize(u32 u_power);
u32 util_roundpower2(u32 u_power);
i32 validate_hex_string(const char *p_string);
u8 hexchar_to_nibble(u8 r_hex);
char *binary_fmt(uintmax_t x, char buf[static FMT_BUF_SIZE]);
const char* byte_to_binary( u32 x );

#endif
