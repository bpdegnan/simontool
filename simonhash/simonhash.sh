#!/bin/sh
#Before anything else, set the PATH_SCRIPT variable
	pushd `dirname $0` > /dev/null; PATH_SCRIPT=`pwd -P`; popd > /dev/null
	PROGNAME=${0##*/}; PROGVERSION=0.1.0 

# this program is a HASH wrapper for SIMONTOOL
# The HASH program takes the SIMONTOOL program and runs the key schedule on the passed
# data, and then the key schedule result is then XOR'd.  You need to check the 
# simonkeyasahash directory in the repository for all of the information.

# Required programs:
#
SIMONEXEC=simontool
BIGNUMBERS=bashbignumbers.sh
URL_SIMONTOOL="https://github.com/bpdegnan/simontool/archive/master.zip"
URL_BIGNUMBERS="https://raw.githubusercontent.com/bpdegnan/bashbignumbers/master/bashbignumbers.sh"
FLAG_FETCH=0

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

#hopefully, we have it but if we don't, check again.
if [ ! -f $BIGNUMBERS ]; then
  echo "File, $BIGNUMBERS, not found!"
  exit
else
  #load big numbers
  source "$BIGNUMBERS"
fi

#: <<'END'

SIMON64CHAIN="00000000"

declare -a SIMON64INPUTARRAY=(
"00000000" 
"00000000"
"00000000"
"00000000"
)

#loop through 
for i in "${SIMON64INPUTARRAY[@]}"
do
   #generate input of last round and current input.
   CMDARGKEY="$SIMON64CHAIN$i" #concat the chaining var and the input
   #echo "$CMDARGKEY"
   #feed the key into simontool
   CMDRESULT=$($SIMONEXEC -e -b 32 -k 64 -s $CMDARGKEY  -t 00000000 -y -u | tail -2 | head -1)
   CMDRESULT=$(echo "${CMDRESULT//[[:space:]]/}" | sed 's/.*://')  #remove the formatting
   #echo "$CMDRESULT" 
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
   #echo "$RESULTXORHEX"
   echo "input: $CMDARGKEY output: $CMDRESULT hash: $RESULTXORHEX"
   #at this point, we have the XOR which is 1/2 of the size of the input.
   SIMON64CHAIN=$RESULTXORHEX
done

#END

: <<'END'
SIMON256CHAIN="00000000000000000000000000000000"

declare -a SIMON256INPUTARRAY=(
"00000000000000000000000000000000" 
"00000000000000000000000000000000"
"00000000000000000000000000000000"
"00000000000000000000000000000000"
)

#loop through 
for i in "${SIMON256INPUTARRAY[@]}"
do
   #generate input of last round and current input.
   CMDARGKEY="$SIMON256CHAIN$i" #concat the chaining var and the input
   #echo "$CMDARGKEY"
   #feed the key into simontool
   CMDRESULT=$($SIMONEXEC -e -b 128 -k 256 -s $CMDARGKEY  -t 00000000000000000000000000000000 -y -u | tail -2 | head -1)
   CMDRESULT=$(echo "${CMDRESULT//[[:space:]]/}" | sed 's/.*://')  #remove the formatting
   #echo "$CMDRESULT" 
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
   #echo "$RESULTXORHEX"
   echo "input: $CMDARGKEY output: $CMDRESULT hash: $RESULTXORHEX"
   #at this point, we have the XOR which is 1/2 of the size of the input.
   SIMON256CHAIN=$RESULTXORHEX
done

END





