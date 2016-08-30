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
#ifndef _SIMON_H
#define _SIMON_H

#include "headers.h"
#include "config.h"
#include "arbitrary_register.h"


#define _SIMON_ARRAY_LENGTH 256 
#define _SIMON_COMMAND_LENGTH 1024
#define _SIMON_DEFAULT_VOLTAGE "3.3"
#define SIMON_STROBE_OFF 0
#define SIMON_STROBE_ON 0x00000001

#define SIMON_ENCRYPT 0x00000001


typedef struct{
	i32 debug;
	i32 debug_zvalue;
	i32 debug_writetolog;  //if > 0, write to log.  if <=0, error index.
	i32 debug_strobes;
	
	i32 filectl_latexoutput;  //to show if we generate a LaTeX output.
	i32 experimental_keyschedulehash;   
	u32 simon_rounds;  //specify the number of rounds to run, even if it violate specification.
	i32 experimental_printoutputs;
	
	//configuration 
	u32 simon_configuration;
	
	u32 clock_number;
	u32 clock_count;
	i32 clock_magnitude;  //this will make the clock values for PWLs.

	//these are a derived list of values based on the cipher
	u32 derived_n;
	u32 derived_m;

    //these are for hardware representation
    u32 generated_key_bit;      //bit that is passed when 
    u32 generated_crypto_bit;  
    u32 generated_crypto_mux0;  //when the clock is 0
    u32 generated_crypto_mux1;  //when the clock is 1
    u32 generated_crypto_mux8;  //when the clock is < 8 but not 1 and 0
    u32 generated_key_mux3;     //when maxclock is > bitmax-4
    u32 generated_key_mux4;     //when maxclock is > bitmax-5
    u32 generated_key_mux1;     //only used for the 4-word key version

	//these are for the actual cipher
	u32 block_bit_length;
	u32 key_bit_length;
	u32 rounds_T;
	u32 seq_Z;
	u32 status_key_length;
	u8 key_ascii[_SIMON_ARRAY_LENGTH];  //ascii representation of the key (later copied as bits arbreg)
	u8 key_ascii_initial[_SIMON_ARRAY_LENGTH];  //ascii representation of the key (later copied as bits arbreg)
	u8 key_ascii_final[_SIMON_ARRAY_LENGTH]; 
	u8 crypto_ascii[_SIMON_ARRAY_LENGTH];  //crypto in ascii
	u8 crypto_ascii_initial[_SIMON_ARRAY_LENGTH]; 
	u8 logfile_ascii[_SIMON_ARRAY_LENGTH];
	u8 latexfile_key_ascii[_SIMON_ARRAY_LENGTH];
	u8 latexfile_data_ascii[_SIMON_ARRAY_LENGTH];
	u8 voltage_ascii[_SIMON_ARRAY_LENGTH];
	u8 command_ascii[_SIMON_COMMAND_LENGTH];
		
}simoncipherconfig;


/*
**  The simondata structure is used to pass data in and out of the encryption core.
*/
typedef struct{
   i32 simon_bytecount;  //this represents the number of bytes in a chunk to give to the encryption engine.  You need to "pad" things if you are not aligned.  For instance, if you give 2 bytes to a 8 byte encryption chunk
   u8 data_in[_SIMON_ARRAY_LENGTH];  
   u8 data_out[_SIMON_ARRAY_LENGTH];

}simondata;


//simon.c has a global pointer to the address of the configuration structure
simoncipherconfig* simonconfigptr;   //the encryption engine configuration
simondata* simondataptr;                //the data for the SIMON engine

//SIMON hardware emulation primitives
void simon_hardwarecreate();
void simon_hardwaredestroy();
u32 simon_hardware_getLFSR();  //the current LFSR bit
u32 simon_hardware_getZ();  //Z is the appropriate combination of LFSR and toggle
u32 simon_hardware_getK();  //K is the current key bit 
void simon_hardware_muxcontrolencrypt(u32 clk_count);//control the hardware mux of encryption register
i32 simon_hardware_clock_lfsr_encrypt(arbitrary_register *p_arbreg);
i32 simon_hardware_clock_lfsr_decrypt(arbitrary_register *p_arbreg);
u32 simon_hardware_gettoggle();
u32 simon_hardware_counter_inc();  //increment the hardware counter
void simon_hardware_counter_reset();  //reset the hardware counter

//this file uses the global pointer simonconfigptr, so you must create/destroy the memory
simoncipherconfig* simon_create();
u32 simon_destroy();

void simon_debug_set_z(i32 p_zindex);
u32 simon_debug_setdebug(i32 p_debuglevel);
void simon_debug_printstructure(FILE *p_stream);
void simon_debug_simondata(FILE *p_stream);
void simon_debug_setstrobes(u32 p_debuglevel);
void simon_hardware_test_loadcrypto();
void simon_hardware_test_printcrypto();

//these functions control the structure.
void simon_set_key_ascii(const char *p_keystring);
void simon_set_crypto_ascii(const char *p_cryptostring);
void simon_set_logfile_ascii(const char *p_logfile);
void simon_set_latexfile_ascii(const char *p_latexfile);
void simon_set_voltage_ascii(const char *p_voltage);

//--Experimental or logging
void simon_experimental_keyhash();  
u32 simon_set_roundcount(u32 p_rounds);  //this is for experimentation
void simon_set_cmdarg(const char *p_commandstring);
i32 simon_set_printoutput();  //to modify the output format

void simon_hardware_loadregfromASCII(arbitrary_register *p_arbreg,u8 *p_asciistring,u32 r_asciilen);


void simon_hardware_blocksetindex(u8 l_index);  //set the block counter
i32 simon_hardware_blockaddbyte(u8 r_blockbyte); //set block value
i32 simon_hardware_blocksubtractbyte();
u8 simon_hardware_getdataoutatbyte(u8 l_index);

u32 simon_set_blocksize(u32 p_blocklength);
u32 simon_get_blocksize();
u32 simon_set_keysize(u32 p_keylength);
u32 simon_get_keysize();
void simon_set_clocknumber(u32 p_clock);
//i32 simon_init_lfsr(arbitrary_register *p_arbreg);
//void simon_init_lfsr(u8 r_initialvalue);
void simon_init_lfsr(u8 r_initialvalue,u8 r_mode);

void simon_encryptcore_serial();
void simon_decryptcore_serial();


#endif
