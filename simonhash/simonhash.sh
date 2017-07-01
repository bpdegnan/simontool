#!/bin/bash
#Before anything else, set the PATH_SCRIPT variable
	pushd `dirname $0` > /dev/null; PATH_SCRIPT=`pwd -P`; popd > /dev/null
	PROGNAME=${0##*/}; PROGVERSION=0.1.0 

# This is the *new* simonhash.sh file, which takes some arguments
#  We need to know if:
# -c the counter is used
# -a the AND modification is used
# -k the key width.
# -f full or 1/2 width, so -f is full

#the internal variables are:
#simon configurations
# SIMONBITS=32
# SIMONKEY=64
#tweaks to behavior
# HASHMODEAND=0     #this is the -u option that breaks the simon spec
# HASHMODECOUNTER=0 #iterative counter
# HASHMODEWIDTH=0   #half or full width.  1=full width
#xxd configuration
# XXDCONFWIDTH


# Required programs:
#
SIMONEXEC=simontool
XXDEXEC=xxd
BIGNUMBERS=bashbignumbers.sh
URL_SIMONTOOL="https://github.com/bpdegnan/simontool/archive/master.zip"
URL_BIGNUMBERS="https://raw.githubusercontent.com/bpdegnan/bashbignumbers/master/bashbignumbers.sh"
FLAG_FETCH=0
#for reporting of progress
MODDIV=1024  #the number to report progress


#check for bignumbers
if [ ! -f $BIGNUMBERS ]; then
    printf 'File, %s, not found!  Attempting to fetch...\n' "$BIGNUMBERS"
    #bash has hash, otherwise, I'd use command -v
    if hash wget 2>/dev/null; then
      FLAG_FETCH=1
      wget $URL_BIGNUMBERS
      printf '\nUsed wget to fetch %s\n' "$BIGNUMBERS"
    elif hash curl 2>/dev/null; then
      FLAG_FETCH=2
      curl -O $URL_BIGNUMBERS
      printf '\nUsed curl to fetch %s \n'"$BIGNUMBERS"
    else
      echo "File, $BIGNUMBERS, could not be acquired!"
      echo "Furthermore, wget and curl not found to get it from:"
      echo "$URL_BIGNUMBERS"
      exit
    fi
fi

#check for simontool
if hash $SIMONEXEC 2>/dev/null; then
   echo "$SIMONEXEC found!"
else
      echo "File, $SIMONEXEC, could not be acquired!"
      echo "please download and compile from:"
      echo "$URL_SIMONTOOL"
      exit
fi

#check for xxd
if hash $XXDEXEC 2>/dev/null; then
     echo
else
      echo "File, $XXDEXEC, could not be found!"
      echo "please install it."
      exit
fi

#hopefully, we have it but if we don't, check again.
if [ ! -f $BIGNUMBERS ]; then
  echo "File, $BIGNUMBERS, not found!"
  exit
else
  #load big numbers
  source "$BIGNUMBERS"
fi



SIMONBITS=32
SIMONKEY=64
HASHMODEAND=0 
HASHMODECOUNTER=0
HASHMODEWIDTH=0
let counter=0;
XXDCONFWIDTH=4;

#SHORTOPTS="k:caf" 
#ARGS=$(getopt $SHORTOPTS "$@" ) 
#eval set -- "$ARGS" 

OPTIND=1         # Reset in case getopts has been used previously in the shell.
while getopts "k:caf" opt; do 
 # echo "opt: $opt"
   case $opt in 
       "k") 
         #shift 
         #echo "$1"
         SIMONKEY=$OPTARG
       #  echo "-k is $SIMONKEY"
         ;; 
		"c")
        HASHMODECOUNTER=1
		;;
		"f")
		  #full width
		HASHMODEWIDTH=1
		;;
		"a")
		HASHMODEAND=1         
		;;
       *)
      	break;
         ;; 
   esac  
done 
shift $((OPTIND-1))

#[ "$1" = "--" ] && shift

#select the correct data size for the key

   case $SIMONKEY in 
       64) 
         SIMONBITS=32
         XXDCONFWIDTH=4
         ;; 
		96) 
         SIMONBITS=48
         XXDCONFWIDTH=6
         ;; 
		128) 
         SIMONBITS=64
         XXDCONFWIDTH=8
         ;; 
		256) 
         SIMONBITS=128
         XXDCONFWIDTH=16
         ;; 
      *) #sane state
		SIMONBITS=32
		SIMONKEY=64
		XXDCONFWIDTH=4
         ;; 
   esac 

#echo "      SIMONBITS: $SIMONBITS"
#echo "       SIMONKEY: $SIMONKEY"
#echo "    HASHMODEAND: $HASHMODEAND"
#echo "HASHMODECOUNTER: $HASHMODECOUNTER"
#echo "  HASHMODEWIDTH: $HASHMODEWIDTH"
#echo "        COUNTER: $counter"
#echo "   XXDCONFWIDTH: $XXDCONFWIDTH"


#random handle for this run
HANDLE=$RANDOM  


#echo "operate on: $@"
for ifile in "$@"
do
	if [ ! -f "$ifile" ]; then
		break;
	fi

	#calculate run width
    HASHWIDTH=$SIMONKEY
    #if the HASHMODECOUNTER is 1, use full width
    if [ $HASHMODEWIDTH -eq 0 ]
    then
       HASHWIDTH=$SIMONBITS;
    fi

   #create the SIMONTOOL arguments
   SIMONARGS="-y"
    if [ $HASHMODEAND -eq 1 ]
    then
       SIMONARGS="-y -u"
    fi
   

	#convert into bytes
	xxd -c$XXDCONFWIDTH -g$XXDCONFWIDTH -p $ifile > /tmp/$ifile.$HANDLE.nopad

	#pad with zeros to be uniform padding
	awk '{cur=length()} NR==FNR{max=(cur>max?cur:max);next} {printf "%s%0*s\n", $0, max-cur, ""}' /tmp/$ifile.$HANDLE.nopad /tmp/$ifile.$HANDLE.nopad > /tmp/$ifile.$HANDLE.hex

   #filename
	FILEOUT=$ifile.$SIMONKEY.$HASHWIDTH.$HASHMODECOUNTER.$HASHMODEAND.hash
   # or do whatever with individual element of the array

   #create the IV, which is zero
   
   RESULT=$(bashUTILzerowidth $HASHWIDTH)
   SIMONIV=$(bashUTILbin2hex $RESULT)
 #  echo "SIMONIV $SIMONIV"
   SIMONCHAIN=$SIMONIV
 #  echo "SIMONCHAIN $SIMONCHAIN"
   RESULT=$(bashUTILzerowidth $SIMONBITS)
   SIMONNULLARG=$(bashUTILbin2hex $RESULT)
   
   
	if [ -f "$FILEOUT" ]; then
		#echo "rm $FILEOUT"
		rm $FILEOUT
	fi
	#make a header
	echo "## $FILEOUT" >> $FILEOUT 
	echo "## xxd -c$XXDCONFWIDTH -g$XXDCONFWIDTH -p $ifile > /tmp/$ifile.$HANDLE.nopad" >> $FILEOUT
	echo "## SIMONIV: $SIMONIV"  >> $FILEOUT
	echo "## $SIMONEXEC -e -b $SIMONBITS -k $SIMONKEY -s $SIMONIV  -t $SIMONNULLARG $SIMONARGS " >> $FILEOUT
    
    HASHCOUNTER=$(bashUTILzerowidth $SIMONKEY)
        
   #loop through  the file
	let j=0
	IFS=$'\n'          # make newlines the only separator
	for i in $(cat /tmp/$ifile.$HANDLE.hex)  # the argument is a file  
	do
	#two very different architectures depending on if it is half or full widht.
	if [ $HASHMODEWIDTH -eq 0 ]
    then
	   #generate input of last round and current input.
	   #echo "SIMONCHAIN $SIMONCHAIN"
	   #echo "SIMONCHAIN $SIMONCHAIN"
	   #echo "i $i"
	   CMDARGKEY="$SIMONCHAIN$i" #concat the chaining var and the input
	   #echo "CMDARGKEY $CMDARGKEY"
	   #if we are in counter mode, xor in the counter
	    if [ $HASHMODECOUNTER -eq 1 ]
    	then
       	  BINARG1=$(bashUTILhex2bin $CMDARGKEY)  #turn the hex value from the file into binary  
   		  HASHARG=$(bashXORbinstring $BINARG1 $HASHCOUNTER) #XOR with the counter  
         #echo "HASHARG: $HASHARG ($HASHCOUNTER)"
          CMDARGKEY=$(bashUTILbin2hex $HASHARG) #convert it back
          HASHCOUNTER=$(bashINCbinstring $HASHCOUNTER) #increment it for the next time

    	fi
   	   
	   #feed the key into simontool
	   CMDSTRING="$SIMONEXEC -e -b $SIMONBITS -k $SIMONKEY -s $CMDARGKEY -t $SIMONNULLARG $SIMONARGS"
	   CMDRESULT=$(eval exec $CMDSTRING | tail -2 | head -1)
	   #echo "CMDRESULT $CMDRESULT"
	   CMDRESULT=$(echo "${CMDRESULT//[[:space:]]/}" | sed 's/.*://')  #remove the formatting
	   STRSIZE=${#CMDRESULT}  #get the string size
	   STRHALFSIZE=$(($STRSIZE / 2)) #divide by two
	   #now use BASH to split the string
	   XORARG1=${CMDRESULT:0:$STRHALFSIZE}
	   XORARG2=${CMDRESULT:$STRHALFSIZE:STRHALFSIZE}
	   #convert from HEX to bin to process the numbers
	   BINARG1=$(bashUTILhex2bin $XORARG1)
	   BINARG2=$(bashUTILhex2bin $XORARG2)
	   RESULTXOR=$(bashXORbinstring $BINARG1 $BINARG2)
	   RESULTXORHEX=$(bashUTILbin2hex $RESULTXOR)
	   printf "%s\n" "$RESULTXORHEX" >> "$FILEOUT"
	   if (( $j % $MODDIV ==0 ))           # no need for brackets
		then
		 echo "mod: $j"  # to keep track of iteratons
		fi
		let "j++"
	   SIMONCHAIN=$RESULTXORHEX
	   
	 else  #full width hash
	 
	   BINARG1=$(bashUTILhex2bin $SIMONCHAIN)  #get the last result
	   BINARG2=$(bashUTILhex2bin $i)             #get the file value and convert to binary
	   BINARG2=$(bashPADbinstring $BINARG2 $SIMONKEY)   #extend the binary word the key width
	   #echo "CMDARGKEY=(bashXORbinstring $BINARG1 $BINARG2)"
	   CMDARGKEY=$(bashXORbinstring $BINARG1 $BINARG2)  #xor to make the new word
	    if [ $HASHMODECOUNTER -eq 1 ]
    	then

   		  CMDARGKEY=$(bashXORbinstring $CMDARGKEY $HASHCOUNTER) #XOR with the counter  
         #echo "HASHARG: $HASHARG ($HASHCOUNTER)"
          HASHCOUNTER=$(bashINCbinstring $HASHCOUNTER) #increment it for the next time

    	fi	   
	      
	   CMDARGKEY=$(bashUTILbin2hex $CMDARGKEY)
	   #echo "1: $BINARG1"
	   #echo "2: $BINARG2"
	   #echo "x: $CMDARGKEY"


	   #feed the key into simontool
	   CMDSTRING="$SIMONEXEC -e -b $SIMONBITS -k $SIMONKEY -s $CMDARGKEY -t $SIMONNULLARG $SIMONARGS"
	   #echo "CMDSTRING: $CMDSTRING"
	   CMDRESULT=$(eval exec $CMDSTRING | tail -2 | head -1)
       CMDRESULT=$(echo "${CMDRESULT//[[:space:]]/}" | sed 's/.*://')  #remove the formatting
	   
	   printf "%s\n" "$CMDRESULT" >> "$FILEOUT"
	   if (( $j % $MODDIV ==0 ))           # no need for brackets
		then
		 echo "mod: $j"  # to keep track of iteratons
		fi
		let "j++"
	   SIMONCHAIN=$CMDRESULT
	 fi
	   
	done 
   
   
done



