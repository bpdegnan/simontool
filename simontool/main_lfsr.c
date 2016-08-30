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
#include "main.h"
#include "simon.h"

static arbitrary_register *p_hardware_LFSR; 

void lsfr_up_u(arbitrary_register *p_arbreg)
{
	u8 bit0=0;  //right most bit
	u8 bit1=0;
	u8 bit2=0;
	u8 bit4=0;
	u8 opres0=0;
	u8 opres1=0;
   // u:  c+e  a    b    c    d+e
   //x^5 + x^4 + x^2 + x^1 + 1		  
   bit4=arbreg_getbit(p_arbreg, 4);  	
   bit2=(arbreg_getbit(p_arbreg, 2));
   opres0= bit2^bit4;
   bit0=arbreg_getbit(p_arbreg, 4); 	
   bit1=(arbreg_getbit(p_arbreg, 3));
   opres1= bit0^bit1;
   //fprintf(stdout,"%x^%x=%x\n",bit1, bit0,opres);	
   arbreg_rol(p_arbreg,1); //roll left, but then insert right bit.
   arbreg_setbit(p_arbreg, 0,opres0);
   arbreg_setbit(p_arbreg, 4,opres1);
}
void lsfr_down_u(arbitrary_register *p_arbreg)
{
  //x^5 + x^4 + x^3 + x^1 + 1
  	u8 bit0=0;  //right most bit
	u8 bit1=0;
	u8 bit2=0;
	u8 bit3=0;
	u8 opres4=0;
	u8 opres1=0;
	bit3=arbreg_getbit(p_arbreg, 3);  
	bit0=(arbreg_getbit(p_arbreg, 0));
	opres4= bit3^bit0;
	bit1=arbreg_getbit(p_arbreg, 4);  
	//bit2=(arbreg_getbit(p_arbreg, 3));
	opres1= opres4^bit1;
	arbreg_ror(p_arbreg,1);
	arbreg_setbit(p_arbreg, 4,opres4);
	arbreg_setbit(p_arbreg, 3,opres1);

}
void lsfr_up_v(arbitrary_register *p_arbreg)
{
	u8 bit0=0;  //right most bit
	u8 bit1=0;
	u8 bit2=0;
	u8 bit4=0;
	u8 opres0=0;
	u8 opres1=0;
		  bit4=arbreg_getbit(p_arbreg, 4);  //x^5	
		  bit2=(arbreg_getbit(p_arbreg, 2));//x^3
		  opres0= bit2^bit4;
		  bit0=arbreg_getbit(p_arbreg, 0);  //x^1	
		  bit1=(arbreg_getbit(p_arbreg, 1));//x^2
		  opres1= bit0^bit1;
		  //fprintf(stdout,"%x^%x=%x\n",bit1, bit0,opres);	
		  arbreg_rol(p_arbreg,1); //roll left, but then insert right bit.
		  arbreg_setbit(p_arbreg, 0,opres0);
		  arbreg_setbit(p_arbreg, 2,opres1);
}

void lsfr_down_v(arbitrary_register *p_arbreg)
{
	u8 bit0=0;  //right most bit
	u8 bit1=0;
	u8 bit2=0;
	u8 bit3=0;
	u8 opres0=0;
	u8 opres1=0;
	bit3=arbreg_getbit(p_arbreg, 3);  
	bit0=(arbreg_getbit(p_arbreg, 0));
	opres0= bit3^bit0;
	bit1=arbreg_getbit(p_arbreg, 1);  
	bit2=(arbreg_getbit(p_arbreg, 2));
	opres1= bit2^bit1;
	arbreg_ror(p_arbreg,1);
	arbreg_setbit(p_arbreg, 4,opres0);
	arbreg_setbit(p_arbreg, 1,opres1);

}

void lsfr_up_w(arbitrary_register *p_arbreg)
{
   //x^5 + x^2 + 1
   u8 bit2=0;
   u8 bit4=0;
   u8 opres0=0;
//w is  c+e  a    b    c    d
   bit4=arbreg_getbit(p_arbreg, 4);  //x^5	
   bit2=(arbreg_getbit(p_arbreg, 2));//x^3
   opres0= bit2^bit4;
   arbreg_rol(p_arbreg,1); //roll left, but then insert right bit.
   arbreg_setbit(p_arbreg, 0,opres0);
}

void lsfr_down_w(arbitrary_register *p_arbreg)
{
  // x^5 + x^3 + 1
   u8 bit3=0;
   u8 bit0=0;
   u8 opres0=0;
   bit3=arbreg_getbit(p_arbreg, 3);  	
   bit0=(arbreg_getbit(p_arbreg, 0));
   opres0= bit3^bit0;
   arbreg_ror(p_arbreg,1); //roll left, but then insert right bit.
   arbreg_setbit(p_arbreg, 4,opres0);
}

//int main (int argc, char **argv) {
int main () {
  int i;
  char c_sel='w';

  p_hardware_LFSR = arbreg_create(5);  //hardware representation of LFSR
  arbreg_setbit(p_hardware_LFSR, 4,1);
  for(i=0;i<32;i++)
  {
    fprintf(stdout,"%02i ",i);
    arbreg_debug_dumpbits(stdout, p_hardware_LFSR, 1);
    switch(c_sel)
    { case 'w':
      lsfr_up_w(p_hardware_LFSR);
      break;
      case 'v':
      lsfr_up_v(p_hardware_LFSR);
      break;
      default:
      lsfr_up_u(p_hardware_LFSR);
      break;
    }
  }
  arbreg_clear(p_hardware_LFSR);
  arbreg_setbit(p_hardware_LFSR, 4,1);
  for(i=31;i>=0;i--)
  {
    fprintf(stdout,"%02i ",i);
    arbreg_debug_dumpbits(stdout, p_hardware_LFSR, 1);
    switch(c_sel)
    { case 'w':
        lsfr_down_w(p_hardware_LFSR);
      break;
      case 'v':
        lsfr_down_v(p_hardware_LFSR);
      break;
      default:
        lsfr_down_u(p_hardware_LFSR);
      break;
    }
  
  }
  
  arbreg_destory(p_hardware_LFSR);
}
