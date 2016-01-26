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
#ifndef _ARBITRARY_REGISTER_H
#define _ARBITRARY_REGISTER_H

#include "byteconfiguration.h"
#include "config.h"

#ifndef ARBREG_INTERNAL
  #define ARBREG_INTERNAL 512
  #define ARBREG_NAMELEN 8
  #define ARBREG_VERSION 0x0000100
  
  #define ARBREG_DEBUG_MAKEZLEFT
#endif 

#define ARBREG_ERROR_BITWIDTHOVERFLOW -1024


typedef struct {
i32 error_ref;
u32 bit_count;
u32 intern_version;
u32	intern_bytecount; 
u8 intern_name[ARBREG_NAMELEN];
u8 intern_register[ARBREG_INTERNAL];
} arbitrary_register;


//create the register
void *arbreg_create(u32 bitwidth);
void arbreg_destory(void *p_arbreg);
u32 arbreg_getMSBindex(arbitrary_register *p_arbreg);  //return the index of the MSB.
u8 arbreg_getbyte(arbitrary_register *p_arbreg, u32 r_byteindex);
u8 arbreg_setbyte(arbitrary_register *p_arbreg, u32 r_byteindex, u8 r_byte);

i32 arbreg_setbit(arbitrary_register *p_arbreg, u32 r_bitindex, u32 r_bitvalue);
i32 arbreg_getbit(arbitrary_register *p_arbreg, u32 r_bitindex);
void arbreg_clear(arbitrary_register *p_arbreg);
u8 arbreg_getMSB(arbitrary_register *p_arbreg);

u8 arbreg_internal_inst_ror(arbitrary_register *p_arbreg);
u8 arbreg_internal_inst_rol(arbitrary_register *p_arbreg);
u8 arbreg_internal_inst_insertbitl(arbitrary_register *p_arbreg, u8 p_bit);

u8 arbreg_ror(arbitrary_register *p_arbreg, u32 r_shift);
u8 arbreg_rol(arbitrary_register *p_arbreg, u32 r_shift);
u8 arbreg_push_nibble(arbitrary_register *p_arbreg, u8 r_byte);
u8 arbreg_shiftr_insertMSB(arbitrary_register *p_arbreg, u8 r_bit);


void arbreg_debug_mergedumphex(arbitrary_register *p_arbreg0, arbitrary_register *p_arbreg1); //merge two registers and print result in hex.
void arbreg_debug_dumpbits(FILE *p_stream, arbitrary_register *p_arbreg, u8 terminator);
void arbreg_debug_dumphexmod(FILE *fp_stream,arbitrary_register *p_arbreg, u32 r_modulus);
void arbreg_debug_dumphex(arbitrary_register *p_arbreg);

void arbreg_printhex(FILE *p_fileref,arbitrary_register *p_arbreg);
i32 arbreg_arraycopy(arbitrary_register *p_arbreg, u8 *p_array);  


void arbreg_debugdump(arbitrary_register *p_arbreg);
//operations on the register


#endif
