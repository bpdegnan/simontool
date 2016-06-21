/*
Copyright (C) 2015  Brian Degnan (http://users.ece.gatech.edu/~degs)
The Georgia Institute of Technology

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
#include "utilities.h"

u32 util_validroundsize(u32 u_power)
{
	if((u_power%8==0) && ((u_power<=256)&&u_power>=32))
	{  //assume that we have a valid number
	  ;;
	}else
	{  //find the closest bit.
	  u_power = util_roundpower2(u_power);
	}
  return(u_power);
}

/*
**  u32 util_roundpower2(u32 u_power)
**  The initial purpose of this function was to be sure that 127 became 128; however,
**  the SIMON spec has 96 as a value, which is not a power of 2.  
*/
u32 util_roundpower2(u32 u_power)
{
  u_power = u_power-1;
  u_power |= u_power >> 1;
  u_power |= u_power >> 2;
  u_power |= u_power >> 4;
  u_power |= u_power >> 8;
  u_power |= u_power >> 16;
  u_power = u_power+1;
  return(u_power);
}

/*
**  i32 validate_hex_string(u8 *p_string)
**  Take a string in hex, such as 3021A4 and confirm it's valid
**  0 is success, and other values are a count of bad values
*/
i32 validate_hex_string(const char *p_string)
{
	if (p_string[strspn(p_string, "0123456789abcdefABCDEF")] == 0)
	{
    	return(0);
	}
	return(-1);
}

/*
**  u8 hexchar_to_nibble(u8 r_hex)
**  From a char input of hex, return the value of the hex
*/

u8 hexchar_to_nibble(u8 r_hex)
{
	u8 u8_nibble = 0;
	switch(r_hex)
	{       
       case('1'):  u8_nibble = 0x01;  break;
       case('2'):  u8_nibble = 0x02;  break;
       case('3'):  u8_nibble = 0x03;  break;
       case('4'):  u8_nibble = 0x04;  break;
       case('5'):  u8_nibble = 0x05;  break;
       case('6'):  u8_nibble = 0x06;  break;
       case('7'):  u8_nibble = 0x07;  break;
       case('8'):  u8_nibble = 0x08;  break;
       case('9'):  u8_nibble = 0x09;  break;
       case('a'):  u8_nibble = 0x0a;  break;
       case('A'):  u8_nibble = 0x0A;  break;
       case('b'):  u8_nibble = 0x0b;  break;
       case('B'):  u8_nibble = 0x0B;  break;
       case('c'):  u8_nibble = 0x0c;  break;
       case('C'):  u8_nibble = 0x0C;  break;
       case('d'):  u8_nibble = 0x0d;  break;
       case('D'):  u8_nibble = 0x0D;  break;
       case('e'):  u8_nibble = 0x0e;  break;
       case('E'):  u8_nibble = 0x0E;  break;
       case('f'):  u8_nibble = 0x0f;  break;
       case('F'):  u8_nibble = 0x0F;  break;
       default:    u8_nibble = 0x00;  break;	
	}
	return(u8_nibble);
}
/*
**  char *binary_fmt(uintmax_t x, char buf[static FMT_BUF_SIZE])
**  prints out binary as a string for the fprintf(stdout, "%s", binary_fmt(x, tmp))
**  where tmp is char tmp[FMT_BUF_SIZE]; and x is 2 or 10 depending on the format
*/
char *binary_fmt(uintmax_t x, char buf[static FMT_BUF_SIZE])
{
    char *s = buf + FMT_BUF_SIZE;
    *--s = 0;
    if (!x) *--s = '0';
    for(; x; x/=2) *--s = '0' + x%2;
    return s;
}
const char* byte_to_binary( u32 x )
{
    static char b[sizeof(u32)*8+1] = {0};
    u32 y;
    long long z;
    for (z=(1LL<<sizeof(u32)*8)-1,y=0; z>0; z>>=1,y++)
    {  b[y] = ( ((x & z) == z) ? '1' : '0'); }
    b[y] = 0;
    return b;
}
