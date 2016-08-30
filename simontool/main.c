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
#include "main.h"
#include "simon.h"

#define _BUILD_MAIN
static const u32 prog_version=0x00010002;

/*

Simon32/64
Key: 1918 1110 0908 0100
Plaintext: 6565 6877
Ciphertext: c69b e9bb
Simon48/72
Key: 121110 0a0908 020100
Plaintext: 612067 6e696c
Ciphertext: dae5ac 292cac
Simon48/96
Key: 1a1918 121110 0a0908 020100
Plaintext: 726963 20646e
Ciphertext: 6e06a5 acf156
Simon64/96
Key: 13121110 0b0a0908 03020100
Plaintext: 6f722067 6e696c63
Ciphertext: 5ca2e27f 111a8fc8
Simon64/128
Key: 1b1a1918 13121110 0b0a0908 03020100
Plaintext: 656b696c 20646e75
Ciphertext: 44c8fc20 b9dfa07a
Simon96/96
Key: 0d0c0b0a0908 050403020100
Plaintext: 2072616c6c69 702065687420
Ciphertext: 602807a462b4 69063d8ff082
Simon96/144
Key: 151413121110 0d0c0b0a0908 050403020100
Plaintext: 746168742074 73756420666f
Ciphertext: ecad1c6c451e 3f59c5db1ae9
Simon128/128
Key: 0f0e0d0c0b0a0908 0706050403020100
Plaintext: 6373656420737265 6c6c657661727420
Ciphertext: 49681b1e1e54fe3f 65aa832af84e0bbc
Simon128/192
Key: 1716151413121110 0f0e0d0c0b0a0908 0706050403020100
Plaintext: 206572656874206e 6568772065626972
Ciphertext: c4ac61effcdc0d4f 6c9c8d6e2597b85b
Simon128/256
Key: 1f1e1d1c1b1a1918 1716151413121110 0f0e0d0c0b0a0908 0706050403020100
Plaintext: 74206e69206d6f6f 6d69732061207369
Ciphertext: 8d2b5579afc8a3a0 3bf72a87efe7b868


*/

int version()
{	
	fprintf(stdout,"simontool\nversion: %i.%i.%i\n", (prog_version >> 16)&0x0000ffff, (prog_version >> 8)&0x000000ff,(prog_version >> 0)&0x000000ff);
    #ifdef SVNREVISION
     fprintf(stdout,"revision: %d\n",SVNREVISION);
    #endif
     fprintf(stdout, "\n");
	return(prog_version);
}

int usage()
{
	version();
fprintf(stdout,"This program is licensed under the GPLv2.\n"); 
fprintf(stdout,"(c) 2015-2016 Brian Degnan, and the Georgia Institute of Technology\n\n");
fprintf(stdout,"Simontool is a program that is intended to aid in the development of hardware implementations of the Simon Cipher.  The program creates Piece-Wise-Linear wave forms that are of an appropriate format for SPICE simulators. \n");	
fprintf(stdout,"\n   Options for simontool:\n");
fprintf(stdout,"    -a       output all PWL files\n");
fprintf(stdout,"    -b <arg> configure the block size in bits\n");
fprintf(stdout,"    -c <arg> limit clock cycles\n");
fprintf(stdout,"    -d       decryption\n");
fprintf(stdout,"    -e       encryption\n");
fprintf(stdout,"    -h       help (this page)\n");
fprintf(stdout,"    -i <arg> input filename to encrypt/decrypt\n");
fprintf(stdout,"    -k <arg> key size in bits\n");
fprintf(stdout,"    -l <arg> include logfile with name\n");
fprintf(stdout,"    -o <arg> output filename from encrypt/decrypt\n");
fprintf(stdout,"    -r <arg> number of rounds (experimental)\n");
fprintf(stdout,"    -s <arg> key hex in ASCII\n");
fprintf(stdout,"    -t <arg> text hex in ASCII\n");
fprintf(stdout,"    -u       hash ASCII (experimental) \n");
fprintf(stdout,"    -v <arg> set voltage for PWL data\n");
fprintf(stdout,"    -x <arg> (experimental) LaTeX code output where \n");
fprintf(stdout,"             <arg> becomes the prefix for -key.tex and -data.tex files \n");
fprintf(stdout,"    -y       (experimental) printout expanded data and key to stdout  \n");
fprintf(stdout,"    \n");
fprintf(stdout,"An example of encryption for Simon 128/128: \n");   
fprintf(stdout,"simontool -e -b 128 -k 128 -s 0f0e0d0c0b0a09080706050403020100 -t 63736564207372656c6c657661727420\n");
fprintf(stdout,"\n");
fprintf(stdout,"An example of decryption for Simon 32/64 that creates strobe files and logs the iterative outputs to the file d.txt:\n");
fprintf(stdout,"simontool -a -d -b 32 -k 64 -s 4d837db932f2fa04  -t c69be9bb -l d.txt\n");
fprintf(stdout,"\n");
fprintf(stdout,"An example of decryption for Simon 32/64 that outputs the final key and data to stdout:\n");
fprintf(stdout,"simontool -d -b 32 -k 64 -s 4d837db932f2fa04  -t c69be9bb -y\n");
fprintf(stdout,"\n");
	
	return(0);
}

int file_readsimon(FILE *f_ioread,FILE *f_iowrite)
{
	char *charbuffer = (char*) calloc(512,sizeof(char));
	u8  l_singlebyte[1];
	u8  l_offset;
	i32 l_result = 0;
	u32 l_blocksize_bits=simon_get_blocksize();
	u32 l_blocksize_bytes=l_blocksize_bits/8;
	u64 l_bytecounter=0;
	u64 l_ioreadlength=0;

	fseek(f_ioread, 0, SEEK_END); // seek to end of file
	l_ioreadlength = ftell(f_ioread); // get current file pointer
	fseek(f_ioread, 0, SEEK_SET); // seek back to beginning of file
	
	fseek(f_ioread,23,SEEK_SET);  //skip to the offset
	l_result=fread(l_singlebyte, 1, 1, f_ioread);
	l_offset=l_singlebyte[0]-0x30;  //this is the amount of data to remove from the end of the file.
	fseek(f_ioread,32,SEEK_SET);  //skip the header.
	
    do{	
    	l_result=fread(l_singlebyte, 1, 1, f_ioread);   //read a byte from the file
    	if(l_result!=0) //we have a byte, so add it to the queue
    	{
    	//	fprintf(stdout, "%02x",l_singlebyte[0]);
    	   simon_hardware_blockaddbyte(l_singlebyte[0]);
    	   l_bytecounter++;
    	}else
    	{
    		//finish off the block
    		while(l_bytecounter<l_blocksize_bytes)
    		{
    			simon_hardware_blockaddbyte(0);
    		    l_bytecounter++;
    		}
    	}	
    	if(l_bytecounter>=l_blocksize_bytes)  //encrypt and reset the byte counter
    	{  
    	   simon_decryptcore_serial(); 
    	   //now get the bytes from the file.
    	   
    	   for(l_bytecounter=0;l_bytecounter<l_blocksize_bytes;l_bytecounter++)
    		{
    			l_singlebyte[0]=simon_hardware_getdataoutatbyte(l_bytecounter);
    			fwrite(l_singlebyte,1,1,f_iowrite);  //write the decrypted data to a file.
    		}
    	   simon_hardware_blocksetindex(0);  //reset the buffer
    	   l_bytecounter=0;  
    	} 
		
	}while(l_result!=0);
	
	//now to chop a few bytes off the end
	fflush(f_iowrite); 
//	l_ioreadlength = ftell(f_iowrite); // get current file pointer
   // remove 32 due to the header on the original file, and then the offset.
	ftruncate(fileno(f_iowrite),(off_t)(l_ioreadlength-l_offset-32)); //chop the end off.

	free(charbuffer);
	return(0);
}


/*
**  
**
*/
int file_writesimon(FILE *f_ioread,FILE *f_iowrite)
{
	u8  l_singlebyte[1];
	i32 l_result = 0;
	u32 l_blocksize_bits=simon_get_blocksize();
	u32 l_blocksize_bytes=l_blocksize_bits/8;
	u64 l_ioreadlength=0;  //the length of the file in bytes
	u64 l_bytecounter=0;

/*
**  here is my tentative file header in ASCII:
**  32 bytes, 8 for the word SIMON
**            8 for nothing
**			  4 for nothing
**			  4 for offset padding (most likely a value < 8)
**            4 for key size information <optional>
**            4 for block size information <optional>
** An example
** SIMON
**  
**        2
**  128 128
** <binary start>
*/	 
	
	//read the file, encrypt, write it out
	fseek(f_ioread, 0, SEEK_END); // seek to end of file
	l_ioreadlength = ftell(f_ioread); // get current file pointer
	fseek(f_ioread, 0, SEEK_SET); // seek back to beginning of file
	
	//write header
	fprintf(f_iowrite,"SIMON   ");
	fprintf(f_iowrite,"        ");
	fprintf(f_iowrite,"    ");
	fprintf(f_iowrite,"%4i",(u8)(l_blocksize_bytes-(u8)(l_ioreadlength%(l_blocksize_bytes))));
	fprintf(f_iowrite,"%4i",l_blocksize_bits);
	fprintf(f_iowrite,"%4i",simon_get_keysize());
	
	simon_hardware_blocksetindex(0); //make sure that the block index is reset
	
	//loop through the file, I'm using a for loop so I can stop it if I need to examine things
	//for(l_bytecounter = 0; l_bytecounter < l_ioreadlength; l_bytecounter++) {
    //loop getting bytes
    //add them to the block buffer
    //encrypt when full, then save	
    do{	
    	l_result=fread(l_singlebyte, 1, 1, f_ioread);   //read a byte from the file
    	if(l_result!=0) //we have a byte, so add it to the queue
    	{
    	   simon_hardware_blockaddbyte(l_singlebyte[0]);
    	   l_bytecounter++;
    	}else
    	{
    		//finish off the block
    		while(l_bytecounter<l_blocksize_bytes)
    		{
    			simon_hardware_blockaddbyte(0);
    		    l_bytecounter++;
    		}
    	}	
    	if(l_bytecounter>=l_blocksize_bytes)  //encrypt and reset the byte counter
    	{  
    	   simon_encryptcore_serial(); 
    	   //now get the bytes from the file.
    	   
    	   for(l_bytecounter=0;l_bytecounter<l_blocksize_bytes;l_bytecounter++)
    		{
    			l_singlebyte[0]=simon_hardware_getdataoutatbyte(l_bytecounter);
    			fwrite(l_singlebyte,1,1,f_iowrite);  //write the encrypted data to a file.
    		}
    	   simon_hardware_blocksetindex(0);  //reset the buffer
    	   l_bytecounter=0;  
    	} 
		
		}while(l_result!=0);
		
	//}
	
	
	
	return(0);
}

#ifndef DEBUG_MODE

int main (int argc, char **argv) {

	i32 c;
	u32 u32_tmp =0;
	i32 i32_tmp =0;
	u32 flag_infileset=0;
	u32 flag_outfileset=0;
	u32 l_control_flags = 0;
	i32 l_cmdarglen = 0;
	
	
	FILE *f_ioread;
	FILE *f_iowrite;
	//NAME_MAX is the maximum file name length
	char *filenamein = (char*) malloc(NAME_MAX*sizeof(char));  //allocate space for the filename
	char *filenameout = (char*) malloc(NAME_MAX*sizeof(char));  //allocate space for the filename
	
	simon_create();  //create the hardware framework, but not the "hardware".

	//i wish there was a better way to handle this, but I'm getting the arguments for the sake of documentation
    for (i32_tmp=1; i32_tmp<argc; i32_tmp++) {
        l_cmdarglen += strlen(argv[i32_tmp]);
        if (argc > i32_tmp+1)
            l_cmdarglen++;
    }
   // printf("l_cmdarglen: %d\n", l_cmdarglen);

    char *cmdstring = malloc(l_cmdarglen);
    cmdstring[0] = '\0';

    for (i32_tmp=1; i32_tmp<argc; i32_tmp++) {
        strcat(cmdstring, argv[i32_tmp]);
        if (argc > i32_tmp+1)
            strcat(cmdstring, " ");
    }
   // printf("cmdstring: %s\n", cmdstring);
    simon_set_cmdarg(cmdstring);
    free(cmdstring);
    
	
	while ( (c = getopt(argc, argv, "ab:c:k:i:o:s:w:t:l:?hv:dex:ur:y")) != -1) {
        switch (c) {
            case 'a':  //a for show all strobes
               simon_debug_setstrobes(SIMON_STROBE_ON);
            break;
        	case 'b':  //the block size
              u32_tmp = atoi(optarg);  // the b option gives the block size
              simon_set_blocksize(u32_tmp);  //set the keysize
            break;
       /*     case 'z':  //the SIMON spec has 5 Z-values
            	i32_tmp = atoi(optarg);
            	simon_debug_set_z(i32_tmp);  //set the Z value to use.
       */
            break;
            case 'c':  //the "count" of how many clocks to force.
                //the system should clock forever is 0, until it hits a EOF
            	i32_tmp = atoi(optarg);
            	simon_set_clocknumber(i32_tmp);  
            	
            break;
            case 'k':  //the keysize option
              u32_tmp = atoi(optarg);  // the b option gives the block size
              simon_set_keysize(u32_tmp);  //set the keysize
              
            break;
            case 'i':
            	flag_infileset=1;
            	strcpy(filenamein, optarg); 
            break;
            case 'o':
            	flag_outfileset=1;
            	strcpy(filenameout, optarg); 
            break;
            case 'r':  //write the key steps to a log file.
              u32_tmp = atoi(optarg);  // the r option sets the number of rounds.
             // fprintf(stdout, "simon_set_roundcount(%i)\n",u32_tmp);
              simon_set_roundcount(u32_tmp);  //set the round number 
            break;           
            case 's':  //secret key

            	i32_tmp=validate_hex_string(optarg);
            	if(i32_tmp==0)  //valid secret key
            	{
            		if(strlen(optarg)<=_SIMON_ARRAY_LENGTH)
            		{
            			simon_set_key_ascii(optarg);  //set the ASCII key into the array
            		}else
            		{
            		  printf("error: \n");
            		  printf("-s %s\n", optarg);
            		  printf("is greater than allowable length: %i\n",_SIMON_ARRAY_LENGTH);
            		  exit(-1);
            		}
            	}else
            	{
            		printf("error: \n");
            		printf("-s %s\n", optarg);
            		printf("must be a HEX string\n");
            		exit(-1);
            	}
            	
            break;
            case 't':  //a "test" sequence.  Usually, you'd expect this as stdin
            	i32_tmp=validate_hex_string(optarg);
            	if(i32_tmp==0)  //valid test ASCII phrase
            	{
            		if(strlen(optarg)<=_SIMON_ARRAY_LENGTH)
            		{
            			simon_set_crypto_ascii(optarg); 
            		}else
            		{
            		  printf("error: \n");
            		  printf("-s %s\n", optarg);
            		  printf("is greater than allowable length: %i\n",_SIMON_ARRAY_LENGTH);
            		  exit(-1);
            		}
            	}else
            	{
            		printf("error: \n");
            		printf("-s %s\n", optarg);
            		printf("must be a HEX string\n");
            		exit(-1);
            	}
            	SETFLAG(l_control_flags,FLAG_TEST);  //show that we have a test argument
            	//  The "test argument" means that the behavior changes to return the
            	//  encrypted/decrypted result as human readable ASCII.
            	
            break;
            case 'u':  //experimental key hash option.
              simon_experimental_keyhash();
              
            break;            	
            case 'l':  //write the key steps to a log file.
            	simon_set_logfile_ascii(optarg); 
        
            break;
            case 'w':  //secret word
            	printf ("option w with value '%s'\n", optarg);
            	//printf("strlen: %lu\n",strlen(optarg));
            	//strcpy(configptr->key, optarg);
            	
            break;            
            case 'v':  //set the voltage
               simon_set_voltage_ascii(optarg); 
            break;
            case '?':
            case 'h':
			  usage();
			  exit(0);
            break;
            case 'e':
			  //encrypt
			//  if(GETFLAG(l_control_flags,FLAG_DECRYPT)!=0)  //precedence to the first operation
			//  {
			    SETFLAG(l_control_flags,FLAG_ENCRYPT); 
			//  }  
            break;
            case 'd':
             //decrypt
			//  if(GETFLAG(l_control_flags,FLAG_ENCRYPT)!=0)  //precedence to the first operation
			//  {
			    SETFLAG(l_control_flags,FLAG_DECRYPT); 
			 // } 
            
            /* 
            	// this is only if the DEBUG flag is set.
			  #ifdef DEBUG
			  	flags_debug = flags_debug | 0x00000001;
			  #endif  
			    simon_debug_setdebug(1);  //enable debugging of SIMON
            */
            break;
            case 'x':  //write to LaTeX file
            	simon_set_latexfile_ascii(optarg); 
        
            break;
            case 'y':
                simon_set_printoutput();  //select the printout.
            break;
            case ':':
                switch (optopt)
                {
                case 'd':
                    printf("option -%c with default argument value\n", optopt);
                    break;
                default:
                    fprintf(stderr, "option -%c is missing a required argument\n", optopt);
                    return EXIT_FAILURE;
                }
                break;
        	break;
    		default:
        /* invalid option */
        	fprintf(stderr, "%s: option '-%c' is invalid: ignored\n",argv[0], optopt);
        break;
        }
    }
    /*
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc){  printf ("%s ", argv[optind++]);  }
        printf ("\n");
    }
    */
    
    /*validate configuration*/

	simon_hardwarecreate();

	// There will eventually be a few modes here because I want to process a stream
	// in the current implementation, if there's the -t flag, we run a single 
	// encryption or decryption.

	if(GETFLAG(l_control_flags,FLAG_TEST)!=0)
	{  //this expects a test data
	   //simon_debug_printstructure(stdout);
	   //simon_debug_simondata(stdout);
	   if(GETFLAG(l_control_flags,FLAG_ENCRYPT)!=0)
	   {
	   	 simon_hardware_test_loadcrypto(); //load the "data_in" into the array
	   	// simon_debug_simondata(stdout);
	     simon_encryptcore_serial();
	     simon_hardware_test_printcrypto();
	     //simon_debug_simondata(stdout);
	     
	   }else
	   {
	     simon_hardware_test_loadcrypto();
	     //simon_debug_simondata(stdout);
	     simon_decryptcore_serial();
	     simon_hardware_test_printcrypto();
	   }
	}else
	{  //this expects a stream
       // THIS IS NOT COMPLETE
	   if(GETFLAG(l_control_flags,FLAG_ENCRYPT)!=0)
	   {
	     
		  if((flag_infileset==1)&&(flag_outfileset==1)) 
		  {  //if we have file name set
			 if( access( filenamein, F_OK ) != -1 )
			 {
			   
				  //printf("%s\n",filenamestring);
				  f_ioread=fopen(filenamein,"r");
				  f_iowrite=fopen(filenameout,"w+");
				  file_writesimon(f_ioread,f_iowrite);
   //			   printf("offset %llu\n",l_ioreadlength%(simon_get_blocksize()/8));  //divide block by 8 to get a byte boundary
			   
				  fclose(f_iowrite);
				  fclose(f_ioread);
			  } else {
				 fprintf(stdout, "The file, %s, does not exist or could not be read.\n",filenamein);
			  }
		   }else
		   {
		   	fprintf(stdout,"ERROR\nno file name(s) give, please use the -i or -o options\n");
		   	fprintf(stdout,"simontool -h for usage and examples");
		   }
		   
	   	  
	   }else
	   {
	     if((flag_infileset==1)&&(flag_outfileset==1))
	     {  
	     	if( access( filenamein, F_OK ) != -1 ) 
	     	{
				f_ioread=fopen(filenamein,"r");
		   		f_iowrite=fopen(filenameout,"w+");
		   		file_readsimon(f_ioread,f_iowrite);

			   fclose(f_iowrite);
			   fclose(f_ioread);


		   } else {
		      fprintf(stdout, "The file, %s, does not exist or could not be read.\n",filenamein);
		   }
		 }else
		 {
		   	fprintf(stdout,"ERROR\nno file name(s) give, please use the -i or -o options\n");
		   	fprintf(stdout,"simontool -h for usage and examples");
		 }   
	   }
	}
	


	//clean up memory
	free(filenameout);
	free(filenamein);
	simon_hardwaredestroy();
    simon_destroy();
    exit (0);
}

#endif



