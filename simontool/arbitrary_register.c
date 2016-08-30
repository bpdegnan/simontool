/*
Copyright (C) 2015-2016  Brian Degnan http://degnan68k.blogspot.com/

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
#include "headers.h"
#include "arbitrary_register.h"

void *arbreg_create(u32 bitwidth)
{
	i32 l_status = 0;
	u32 l_bitwidth = bitwidth;
	u32 l_bytecount = 0;
	//firstly, check to be sure that the bitwidth is less than the maximum limit
	if(l_bitwidth/8 > ARBREG_INTERNAL)
	{
		l_bitwidth = ARBREG_INTERNAL*8;
		l_status = ARBREG_ERROR_BITWIDTHOVERFLOW;
	}
	//create other initialization values
	l_bytecount=l_bitwidth/8;
	if(l_bitwidth%8!=0)   
	{
		l_bytecount=l_bytecount+1;
	}
	arbitrary_register* p_arbreg = calloc(1,sizeof(arbitrary_register));  //init and zero the memory
	
	if(p_arbreg != NULL)
	{	//set the initial config array vales
		p_arbreg->error_ref = l_status;
		p_arbreg->bit_count = l_bitwidth;
		p_arbreg->intern_version =ARBREG_VERSION;
		p_arbreg->intern_bytecount = l_bytecount;
		memset(p_arbreg->intern_register, 0,ARBREG_INTERNAL);		
	}
	return(p_arbreg);
}

void arbreg_destory(void *p_arbreg)
{
	free(p_arbreg);
}

/*
**  u32 arbreg_getMSBindex(void *p_arbreg)
**  Return the bit index of the MSB.  This is the bit_count - 1.
**  If you have 9 bits, the max index value for the bit is 8.
*/
u32 arbreg_getMSBindex(arbitrary_register *p_arbreg)
{
	u32 bit = p_arbreg->bit_count-1;
	return(bit);
}

/*
**  arbreg_getbyte(void *p_arbreg, u32 r_byteindex)
** retrieves a byte from the internal register array of bytes
*/
u8 arbreg_getbyte(arbitrary_register *p_arbreg, u32 r_byteindex)
{
	u8 l_returnbyte = 0;
	
	if(r_byteindex>=p_arbreg->intern_bytecount)
	{
		#ifdef DEBUG_ARBREG
		fprintf(stdout,"arbreg_getbyte:  error!  requested byte invalid %u >= %u\n" ,r_byteindex, p_arbreg->intern_bytecount);
		#endif
		l_returnbyte =0;
	}else
	{	
		l_returnbyte = p_arbreg->intern_register[r_byteindex];
	}
	return(l_returnbyte);
}
/*
** arbreg_setbyte(arbitrary_register *p_arbreg, u32 r_byteindex, u8 r_byte)
** sets a byte in the array at index of value.
*/

u8 arbreg_setbyte(arbitrary_register *p_arbreg, u32 r_byteindex, u8 r_byte)
{
	u8 l_returnbyte = 0;
	
	if(r_byteindex>=p_arbreg->intern_bytecount)
	{
		#ifdef DEBUG_ARBREG
		fprintf(stdout,"arbreg_setbyte:  error!  requested byte invalid %u >= %u\n" ,r_byteindex, p_arbreg->intern_bytecount);
		#endif
		l_returnbyte =0;
	}else
	{	
		p_arbreg->intern_register[r_byteindex] = r_byte;
	}
	return(l_returnbyte);
}

/*
** arbreg_setbit(arbitrary_register *p_arbreg, u32 r_bitindex, u32 r_bitvalue)
** sets a bit at the index specified by bitvalue
** Even if you set an invalid bit, it does not matter because the functions that
** handle operations on the register ignore out of range bits
*/
i32 arbreg_setbit(arbitrary_register *p_arbreg, u32 r_bitindex, u32 r_bitvalue)
{	
	u32 l_byteindex = r_bitindex/8;
	u8  l_bitoffset = r_bitindex%8;
	u8  l_byte = 0;
	if((p_arbreg->bit_count)<r_bitindex)
	{
		return(ARBREG_ERROR_BITWIDTHOVERFLOW);
	}
	#ifdef DEBUG_ARBREG
	//fprintf(stdout,"r_bitindex: %u, l_byteindex: %u, l_bitoffset: %u\n",r_bitindex,l_byteindex, l_bitoffset);
	#endif
	l_byte=arbreg_getbyte(p_arbreg,l_byteindex);
	if(r_bitvalue==0)
	{
	  l_byte = l_byte & ~(1 << l_bitoffset);  //this makes the 0 mask
	}else{
	  l_byte = l_byte | (1 << l_bitoffset);  //this makes the 1 mask via 
	}
	arbreg_setbyte(p_arbreg,l_byteindex,l_byte);
	return(l_byte);
}
/*
** arbreg_getbit(arbitrary_register *p_arbreg, u32 r_bitindex)
** retrieves a bit at the specified index.  The value is 
** a 0 if the mask shows 0.
*/
i32 arbreg_getbit(arbitrary_register *p_arbreg, u32 r_bitindex)
{	
	u32 l_byteindex = r_bitindex/8;
	u8  l_bitoffset = r_bitindex%8;
	u8  l_byte = 0;
	if((p_arbreg->bit_count)<r_bitindex)
	{
		return(ARBREG_ERROR_BITWIDTHOVERFLOW);
	}
	#ifdef DEBUG_ARBREG
	//fprintf(stdout,"r_bitindex: %u, l_byteindex: %u, l_bitoffset: %u\n",r_bitindex,l_byteindex, l_bitoffset);
	#endif
	l_byte=arbreg_getbyte(p_arbreg,l_byteindex);  //acquire byte
	l_byte = l_byte & (1 << l_bitoffset);  //AND with the offset as a mask.
	if(l_byte!=0)
	{ return(1); }  //should be all zeros if no bit set, otherwise make it 1.
	return(0);
}

/*
**  arbreg_clear(void *p_arbreg)
**  sets all bits to zero
*/
void arbreg_clear(arbitrary_register *p_arbreg)
{
	u8 l_counter = 0;
	for(l_counter=0;l_counter<p_arbreg->intern_bytecount;l_counter++)
	{
	  p_arbreg->intern_register[l_counter] = 0;
	}
}


/*
**  u8 arbreg_getMSB(arbitrary_register *p_arbreg)
**  returns the MSB (the highest bit) from the register.  This is useful if you do not
**  know how wide the register is as far as bit width.
*/

u8 arbreg_getMSB(arbitrary_register *p_arbreg)
{
  u8 bit = arbreg_getbit(p_arbreg, (p_arbreg->bit_count)-1);
  //fprintf(stdout,"(p_arbreg->bit_count)-1: %i resut %i\n",(p_arbreg->bit_count)-1,bit);
  return(bit);
}


/*
** u8 arbreg_internal_inst_ror(arbitrary_register *p_arbreg)
** rolls the register right, which is not a shift because it takes the bit
** that falls off and puts it back.  This function is meant to be
** internal to other functions.
*/
u8 arbreg_internal_inst_ror(arbitrary_register *p_arbreg)
{
	u8 bit_falloff = 0;
	u8 bit_fallin = 0;
	u8  l_arbbyte = 0;
	u8  l_mask = 0;
	u32 l_byteindex = 0;
	u32 l_counter1 =0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u32 l_bitthreshold =p_arbreg->bit_count % 8; 
	//loop through each byte from high to low.
	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
	{
		l_byteindex=(p_arbreg->intern_bytecount -1)-l_counter1;
	  	l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	    bit_falloff = l_arbbyte & 0x01;  // save the bit that will fall off
	    l_arbbyte = l_arbbyte >> 1;
	    if(bit_fallin == 0)
	    {
	     	l_arbbyte = l_arbbyte & 0x7F;
	    }else
	    {
	    	l_arbbyte = l_arbbyte | 0x80;
	    }
	    arbreg_setbyte(p_arbreg,l_byteindex,l_arbbyte);
	   	bit_fallin = bit_falloff;
	}
		//finally, put the last bit back
		l_byteindex=(p_arbreg->intern_bytecount -1);
	  	l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	  	if(l_bitthreshold==0)
	  	{
	  		l_bitthreshold = 7;
	  	}else
	  	{
	  		l_bitthreshold = l_bitthreshold-1;
	  	}
	  	if(bit_fallin==0)
	  	{
	  		l_mask= (1 << l_bitthreshold);
	  		l_arbbyte = l_arbbyte & ~l_mask;
	  		
	  	}else
	  	{
	  		l_mask= (1 << l_bitthreshold);
	  		l_arbbyte = l_arbbyte | l_mask;
	  	}
	  	arbreg_setbyte(p_arbreg,l_byteindex,l_arbbyte);
	  	
	  	return(bit_fallin);
	  	
}


u8 arbreg_internal_inst_rol(arbitrary_register *p_arbreg)
{
	u8 bit_falloff = 0;
	u8 bit_fallin = 0;
	u8  l_arbbyte = 0;
	u8  l_mask = 0;
	u32 l_counter1 =0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u32 l_bitthreshold =p_arbreg->bit_count % 8; 
	//loop through each byte from high to low.
	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
	{	//first, find the bit that will fall off, going left
		l_arbbyte = arbreg_getbyte(p_arbreg,l_counter1);
		if(l_bytecounter-1==l_counter1)
		{  //if the last bit, get the correct highest bit.
			if(l_bitthreshold==0)
			{l_bitthreshold = 7;
			}else
			{l_bitthreshold = l_bitthreshold-1;
			}
			l_mask= (1 << l_bitthreshold);
			bit_falloff = l_mask & l_arbbyte;	
		}else{
			bit_falloff = l_arbbyte & 0x80;  // save the bit that will fall off
		}
		//now that we have the bit that will fall off, we shift the byte
		 l_arbbyte = l_arbbyte << 1;   //shift the byte left
		 if(bit_fallin == 0){
			 l_arbbyte = l_arbbyte & 0xFE;
		 }else{
			 l_arbbyte = l_arbbyte | 0x01;
		 }		
		arbreg_setbyte(p_arbreg,l_counter1,l_arbbyte);
		bit_fallin = bit_falloff; //save the last bit that fell off
	}
	l_arbbyte = arbreg_getbyte(p_arbreg,0);
	if(bit_fallin==0)
	{
		l_arbbyte =l_arbbyte& 0xFE;
	}else{
		l_arbbyte =l_arbbyte| 0x01;
	}
	arbreg_setbyte(p_arbreg,0,l_arbbyte);

	return(bit_fallin);
}
/*
** u8 arbreg_internal_inst_insertbitl(arbitrary_register *p_arbreg, u8 p_bit)
** this is identical to the ROL function, except it inserts a bit as selected
** byte the p_bit value.  It then returns a 0 or 1 that represents the pushed
** out bit.  This might have been called shift left, insert bit on right.
** basically, this function allows you to fill the register in a serial
** fashion.
*/
u8 arbreg_internal_inst_insertbitl(arbitrary_register *p_arbreg, u8 p_bit)
{
	u8 bit_falloff = 0;
	u8 bit_fallin = 0;
	u8  l_arbbyte = 0;
	u8  l_mask = 0;
	u32 l_counter1 =0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u32 l_bitthreshold =p_arbreg->bit_count % 8; 
	//loop through each byte from high to low.
	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
	{	//first, find the bit that will fall off, going left
		l_arbbyte = arbreg_getbyte(p_arbreg,l_counter1);
		if(l_bytecounter-1==l_counter1)
		{  //if the last bit, get the correct highest bit.
			if(l_bitthreshold==0)
			{l_bitthreshold = 7;
			}else
			{l_bitthreshold = l_bitthreshold-1;
			}
			l_mask= (1 << l_bitthreshold);
			bit_falloff = l_mask & l_arbbyte;	
		}else{
			bit_falloff = l_arbbyte & 0x80;  // save the bit that will fall off
		}
		//now that we have the bit that will fall off, we shift the byte
		 l_arbbyte = l_arbbyte << 1;   //shift the byte left
		 if(bit_fallin == 0){
			 l_arbbyte = l_arbbyte & 0xFE;
		 }else{
			 l_arbbyte = l_arbbyte | 0x01;
		 }		
		arbreg_setbyte(p_arbreg,l_counter1,l_arbbyte);
		bit_fallin = bit_falloff; //save the last bit that fell off
	}
	//now replace the last bit with the value specified by the function.
	l_arbbyte = arbreg_getbyte(p_arbreg,0);
	if(p_bit==0)
	{
		l_arbbyte =l_arbbyte& 0xFE;
	}else{
		l_arbbyte =l_arbbyte| 0x01;
	}
	arbreg_setbyte(p_arbreg,0,l_arbbyte);
	return(bit_fallin);  //return the bit that would fall into the next one.
}

/*
** u8 arbreg_shiftr_insertMSB(arbitrary_register *p_arbreg, u8 r_bit)
** this is identical to the ROR function, except it inserts a bit as selected
** byte the p_bit value.  This is useful for the LFSR and other shifting
** 
*/
u8 arbreg_shiftr_insertMSB(arbitrary_register *p_arbreg, u8 r_bit)
{
	u8 bit_falloff = 0;
	u8 bit_fallin = 0;
	u8  l_arbbyte = 0;
	u8  l_mask = 0;
	u32 l_byteindex = 0;
	u32 l_counter1 =0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u32 l_bitthreshold =p_arbreg->bit_count % 8; 
	//loop through each byte from high to low.
	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
	{
		l_byteindex=(p_arbreg->intern_bytecount -1)-l_counter1;
	  	l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	    bit_falloff = l_arbbyte & 0x01;  // save the bit that will fall off
	    l_arbbyte = l_arbbyte >> 1;
	    if(bit_fallin == 0)
	    {
	     	l_arbbyte = l_arbbyte & 0x7F;
	    }else
	    {
	    	l_arbbyte = l_arbbyte | 0x80;
	    }
	    arbreg_setbyte(p_arbreg,l_byteindex,l_arbbyte);
	   	bit_fallin = bit_falloff;
	}
		//finally, put the last bit back
		l_byteindex=(p_arbreg->intern_bytecount -1);
	  	l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	  	if(l_bitthreshold==0)  //check for the bit alignment.
	  	{
	  		l_bitthreshold = 7;
	  	}else
	  	{
	  		l_bitthreshold = l_bitthreshold-1;
	  	}
	  	if((r_bit&0x01)==0)  //bit mask and compare
	  	{
	  		l_mask= (1 << l_bitthreshold);
	  		l_arbbyte = l_arbbyte & ~l_mask;
	  	}else
	  	{
	  		l_mask= (1 << l_bitthreshold);
	  		l_arbbyte = l_arbbyte | l_mask;
	  	}
	  	arbreg_setbyte(p_arbreg,l_byteindex,l_arbbyte);
	  	
	  	return(bit_fallin);
	  	
}
/*
**  arbreg_ror(arbitrary_register *p_arbreg, u32 r_shift)
**  Roll the register (p_arbreg) right by the number of bits, r_shift
*/
u8 arbreg_ror(arbitrary_register *p_arbreg, u32 r_shift)
{
	u32 i_counter = 0;
	u8 i_roll = 0;
	for(i_counter = 0; i_counter<r_shift;i_counter++)
	{
		i_roll=arbreg_internal_inst_ror(p_arbreg);
	}
	return(i_roll);
}
/*
**  arbreg_rol(arbitrary_register *p_arbreg, u32 r_shift)
**  Roll the register (p_arbreg) left by the number of bits, r_shift
*/

u8 arbreg_rol(arbitrary_register *p_arbreg, u32 r_shift)
{
	u32 i_counter = 0;
	u8 i_roll = 0;
	for(i_counter = 0; i_counter<r_shift;i_counter++)
	{
		i_roll=arbreg_internal_inst_rol(p_arbreg);
	}
	return(i_roll);
}

/*
** arbreg_push_nibble(arbitrary_register *p_arbreg, u8 r_byte) 
** push a nibble on the left of the register 
*/

u8 arbreg_push_nibble(arbitrary_register *p_arbreg, u8 r_byte)
{
	u8 nibble = hexchar_to_nibble(r_byte); //return ASCII as hex 
	u8  l_arbbyte = 0;
	u32 i_counter = 0;
    for (i_counter=0;i_counter<4;i_counter++)
	{
	  arbreg_internal_inst_insertbitl(p_arbreg,0);  // shift the register left by 4 spaces, fill with zeros
	}
	l_arbbyte = arbreg_getbyte(p_arbreg,0);  //get the lowest byte
	l_arbbyte = l_arbbyte + nibble;
	arbreg_setbyte(p_arbreg,0,l_arbbyte);  
	return(l_arbbyte);
}
/*
**  void arbreg_debug_dumpbits(FILE *p_stream,arbitrary_register *p_arbreg, u8 terminator)
**  prints a string of 1,0s that represent the current bits in the arbitrary register 
**  with - representing a bit that is unused.  This is because the internal storage is 
*/
void arbreg_debug_dumpbits(FILE *p_stream,arbitrary_register *p_arbreg, u8 terminator)
{
	u32 l_counter0 =0;
	u32 l_counter1 =0;
	u32 l_bitthreshold =0;
	u32 l_byteindex = 0;
	
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u8  l_arbbyte = 0;//arbreg_getbyte(p_arbreg,l_bytecounter-1);
	l_bitthreshold=p_arbreg->bit_count % 8;  

	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
	{ //a bit of strangeness here, as you need to loop through the values of the
	  //register but skip a byte if you don't have any remainder.  
	  l_byteindex=(p_arbreg->intern_bytecount -1)-l_counter1;
	  l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	  if((l_counter1==0)&&l_bitthreshold!=0)
	  {
		for(l_counter0=8; l_counter0 > 0;l_counter0--)
		{  //partial word printing 
			if(l_counter0 > l_bitthreshold)
			{
				fprintf(p_stream,"-");  // if a bit is unused, we put a - to show it.
			}else
			{
				if((l_arbbyte&0x80) == 0)
				{ fprintf(p_stream,"0");
				}else{ fprintf(p_stream,"1");
				}
			}
			l_arbbyte = l_arbbyte << 1;
		}
	  }else
	  {  //this prints the binary stream for the non-partial words
	  	for(l_counter0=8; l_counter0 > 0;l_counter0--)
		{
		  if((l_arbbyte&0x80) == 0)
		  { fprintf(p_stream,"0");
		  }else{ fprintf(p_stream,"1");
		  }
		l_arbbyte = l_arbbyte << 1;
		}
	  }
		fprintf(p_stream," "); //space between words 
	}
	if(terminator!=0)
	{  fprintf(p_stream,"\n");  }
}
/*
**  void arbreg_debug_dumpbits_latexarray(FILE *p_stream,arbitrary_register *p_arbreg, u8 terminator)
**  almost identical to arbreg_debug_dumpbits, but the format is for a LaTeX array
*/
void arbreg_debug_dumpbits_latexarray(FILE *p_stream,arbitrary_register *p_arbreg, u8 terminator)
{
	u32 l_counter0 =0;
	u32 l_counter1 =0;
	u32 l_bitthreshold =0;
	u32 l_byteindex = 0;
	
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u8  l_arbbyte = 0;//arbreg_getbyte(p_arbreg,l_bytecounter-1);
	l_bitthreshold=p_arbreg->bit_count % 8;  
	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
//	for(l_counter1=(l_bytecounter-1);l_counter1<=0;l_counter1--)
	{ //a bit of strangeness here, as you need to loop through the values of the
	  //register but skip a byte if you don't have any remainder.  
	  if(l_counter1==0){fprintf(p_stream,"{");}
	  l_byteindex=(p_arbreg->intern_bytecount -1)-l_counter1;
	  l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	   //this prints the binary stream for the non-partial words
	  	for(l_counter0=8; l_counter0 > 0;l_counter0--)
	  	//for(l_counter0=0; l_counter0 < 8;l_counter0++)
		{
		  if((l_arbbyte&0x80) == 0)
		  { fprintf(p_stream,"0");
		  }else{ fprintf(p_stream,"1");
		  }
		  //add the ,
		
		//fprintf(p_stream,"%i : %i\n",l_counter1,l_counter0);
		
		//fprintf(p_stream,",");
		if((l_counter0)<=1 && (l_counter1)==(l_bytecounter-1))
		{
		
		}else
		{
			 fprintf(p_stream,",");
		}
		
		/*  if((l_counter1 -2)==l_bytecounter)
		  {
		     fprintf(p_stream,",");
		  }
		 */ 
		l_arbbyte = l_arbbyte << 1;
		}	  
	}
	if(terminator!=0)
	{  fprintf(p_stream,"},\n");  }
	else
    {  fprintf(p_stream,"}%%\n");  }
}

/*
**  void arbreg_debug_dumpbits_gridbox(FILE *p_stream,arbitrary_register *p_arbreg, u8 terminator)
**  almost identical to arbreg_debug_dumpbits, but the format is for a 
**  LaTeX gridbox macro.
*/
void arbreg_debug_dumpbits_gridbox(FILE *p_stream,arbitrary_register *p_arbreg, u32 p_rowindex)
{
	u32 l_counter0 =0;
	u32 l_counter1 =0;
	u32 l_bitthreshold =0;
	u32 l_byteindex = 0;
	u32 l_bitcounter = 1;  //this it for the LaTeX column
	
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	u8  l_arbbyte = 0;
	l_bitthreshold=p_arbreg->bit_count % 8;  

	for(l_counter1=0;l_counter1<l_bytecounter;l_counter1++)
	{ //a bit of strangeness here, as you need to loop through the values of the
	  //register but skip a byte if you don't have any remainder.  
	  l_byteindex=(p_arbreg->intern_bytecount -1)-l_counter1;
	  l_arbbyte = arbreg_getbyte(p_arbreg,l_byteindex);
	   //this prints the binary stream for the non-partial words
	  	for(l_counter0=8; l_counter0 > 0;l_counter0--)
		{
		  if((l_arbbyte&0x80) == 0)
		  { 
		  
		  fprintf(p_stream,"\\gridbox{%i}{%i};",l_bitcounter,p_rowindex);
		  }
		  l_bitcounter++;  //this is the bit counter, which is just a loop
		l_arbbyte = l_arbbyte << 1;
		}	  
	}

}

/*
**  void arbreg_debug_dumphex(arbitrary_register *p_arbreg, u8 *p_stringbuffer)
**  copy the arbitrary register to a string as hex values.  p_stringbuffer must be
**  preallocated
*/
//void arbreg_debug_dumphex(arbitrary_register *p_arbreg, u8 *p_stringbuffer)
void arbreg_debug_dumphex(arbitrary_register *p_arbreg)
{
/*	u32 l_counter1=0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	for(l_counter1=(l_bytecounter-1);l_counter1>0;l_counter1--)
	{   //print each value as a hex byte to the array
		//sprintf(*p_stringbuffer+l_counter1,"%02x",p_arbreg->intern_register[l_counter1]);
		fprintf(stdout,"%02x",p_arbreg->intern_register[l_counter1]);
		if((l_counter1)%8==0)
		{
			fprintf(stdout," ");
		}
	}
	//fprintf(stdout,"%02x\n",p_arbreg->intern_register[0]);
	fprintf(stdout,"%02x ",p_arbreg->intern_register[0]);
	*/
	
	arbreg_debug_dumphexmod(stdout,p_arbreg, 8);
	
}

void arbreg_debug_dumphexmod(FILE *fp_stream, arbitrary_register *p_arbreg, u32 r_modulus)
{
	u32 l_counter1=0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	for(l_counter1=(l_bytecounter-1);l_counter1>0;l_counter1--)
	{   //print each value as a hex byte to the array
		fprintf(fp_stream,"%02x",p_arbreg->intern_register[l_counter1]);
		if((l_counter1)%r_modulus==0)
		{  fprintf(fp_stream," "); }
	}
	fprintf(fp_stream,"%02x ",p_arbreg->intern_register[0]);
}

void arbreg_printhex(FILE *p_fileref,arbitrary_register *p_arbreg)
{
	u32 l_counter1=0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	for(l_counter1=(l_bytecounter-1);l_counter1>0;l_counter1--)
	{   //print each value as a hex byte to the array
		fprintf(p_fileref,"%02x",p_arbreg->intern_register[l_counter1]);
	}
	fprintf(p_fileref,"%02x",p_arbreg->intern_register[0]);  //use this line to format the output to include a space or newline
}

/*
**
**
**
*/
i32 arbreg_arraycopy(arbitrary_register *p_arbreg, u8 *p_array)
{
    u32 l_counter1=0;
    u32 l_counter2=0;
	u32 l_bytecounter = p_arbreg->intern_bytecount;
	for(l_counter1=(l_bytecounter-1);l_counter1>0;l_counter1--)
	{   //print each value as a hex byte to the array
		p_array[l_counter2]=p_arbreg->intern_register[l_counter1];
		l_counter2++;
	}
	p_array[l_counter2]=p_arbreg->intern_register[0];
	l_counter2++;
	return(l_counter2); //return the number of bytes written
}

/*
**  arbreg_debug_mergedumphex(arbitrary_register *p_arbreg0, arbitrary_register *p_arbreg1)
**  This is a strange one.  This function merges two registers of different sizes, and 
**  then prints the HEX output of the result.  
**
*/
void arbreg_debug_mergedumphex(arbitrary_register *p_arbreg0, arbitrary_register *p_arbreg1)
{
	u32 l_tmp=0;
	u32 mastercounter=0;
	u8  bit = 0;
	u32 l_count0=p_arbreg0->bit_count;
	u32 l_count1=p_arbreg1->bit_count;
	u32 l_count = l_count0+l_count1;
	arbitrary_register* p_arbreg_combined;  //temporary register
	p_arbreg_combined=arbreg_create(l_count);	
	for(l_tmp=0;l_tmp<l_count1;l_tmp++)
	{
		bit=arbreg_getbit(p_arbreg1, l_tmp);
		arbreg_setbit(p_arbreg_combined,mastercounter,bit);
		mastercounter++;
	//	arbreg_internal_inst_insertbitl(p_arbreg_combined,bit);
	}
	for(l_tmp=0;l_tmp<l_count0;l_tmp++)
	{
		bit=arbreg_getbit(p_arbreg0, l_tmp);
		arbreg_setbit(p_arbreg_combined,mastercounter,bit);
		mastercounter++;
	}
	
	arbreg_debug_dumphex(p_arbreg_combined);
	
	arbreg_destory(p_arbreg_combined);
}



void arbreg_debugdump(arbitrary_register *p_arbreg)
{
	fprintf(stdout,"error_ref: 0x%08x (%i)\n",p_arbreg->error_ref, p_arbreg->error_ref);
	fprintf(stdout,"bit_count: 0x%08x (%i)\n",p_arbreg->bit_count, p_arbreg->bit_count);	
	fprintf(stdout,"intern_bytecount: 0x%08x (%i)\n",p_arbreg->intern_bytecount, p_arbreg->intern_bytecount);	

}




#if defined(DEBUG_MODE) && defined(DEBUG_ARBREG)

//debug main
int main (int argc, char **argv) {
    
    char *bopt = 0;
	i32 c;
	u32 bits = 16;
	while ( (c = getopt(argc, argv, "b:")) != -1) {
        switch (c) {
        	case 'b':
            
            bopt = optarg;
            bits = atoi(bopt);
            break;
       
        }
    }

//fprintf (stdout,"bit width: '%u'\n", bits);

arbitrary_register* p_arbreg = arbreg_create(bits);

arbitrary_register* p_leftshift = arbreg_create(bits);
arbitrary_register* p_rightshift = arbreg_create(bits);
//arbreg_debugdump(p_arbreg);

//start test run
u32 scratch =0;
u32 scratch1 =0;
i32 printbyte = 0;

//in the paper, the LSFR shifts right, with the initial condition of 00001,
//as I've shifted things backwards, it needs to be 10000, where 1 is bit position 4

printbyte=makez_init(p_arbreg);

//fprintf(stdout,"%x",printbyte);
//fprintf(stdout,"\n");
//fprintf(stdout, "%02i: ",0);
//arbreg_debug_dumpbits(p_arbreg);
//arbreg_debug_dumpbits(p_arbreg);
arbreg_setbit(p_leftshift, 4,1);
arbreg_setbit(p_rightshift, 0,1);


for (scratch =0;scratch<31;scratch++)
{
	if(scratch==0)
  	{
  	  printbyte=arbreg_getbit(p_arbreg, 4);
  	  arbreg_debug_dumpbits(p_arbreg);
  	  //fprintf(stdout,"%x",printbyte);
  	}
  	printbyte=makez_u(p_arbreg);
  	arbreg_debug_dumpbits(p_arbreg);
	//fprintf(stdout,"%x",printbyte);  
}
fprintf(stdout,"\n");
arbreg_destory(p_leftshift);
arbreg_destory(p_rightshift);
arbreg_destory(p_arbreg);


}
#endif



