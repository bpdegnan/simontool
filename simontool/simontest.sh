#!/bin/sh
#Before anything else, set the PATH_SCRIPT variable
	pushd `dirname $0` > /dev/null; PATH_SCRIPT=`pwd -P`; popd > /dev/null
	PROGNAME=${0##*/}; PROGVERSION=0.1.0 

# one might need to update the location of the exe and the test data
simonexec=./bin/simontool.elf
filename="simontest.data"

i_counter=1 #start a counter.
while read -r line
do
  i_strlen=${#line}   #string length
  if [ "$i_strlen" -gt "0" ]; then  #make sure that we do not have a zero length line
    char_start=${line:0:1} #get the first character of the line
    if [ $char_start != '#' ]  # '#' is used as my comment character
    then
      arg_crypt=$(echo $line|awk '{print $1;}')  #the intent of the crypto directory
      arg_block=$(echo $line|awk '{print $2;}')  #the data block size
        arg_key=$(echo $line|awk '{print $3;}')  #the key block size
   arg_keyvalue=$(echo $line|awk '{print $4;}')  #the key value 
  arg_datavalue=$(echo $line|awk '{print $5;}')  #the test data value
result_response=$(echo $line|awk '{print $6;}')  #correct result
  
     # echo "$simonexec -$arg_crypt -b $arg_block -k $arg_key -s $arg_keyvalue -t $arg_datavalue -l log$arg_block_$arg_key.txt"
      # This line runs only the comparison 
      #result=$($simonexec -$arg_crypt -b $arg_block -k $arg_key -s $arg_keyvalue -t $arg_datavalue)
      #this line creates log outputs
      result=$($simonexec -$arg_crypt -b $arg_block -k $arg_key -s $arg_keyvalue -t $arg_datavalue -l log.$arg_crypt.$arg_block.$arg_key.txt -x simon-$arg_crypt-$arg_block-$arg_key)
      #$result has the returned value from the simontool
      #echo $result
      if [ $result == $result_response ]; then
         printf "[%02i] %s %i/%i passed\n" $i_counter $arg_crypt $arg_block $arg_key
      else
         printf "[%02i] %i/%i failed %s!=%s\n" $i_counter $arg_block $arg_key $result $result_response
      fi
      
      i_counter=$((i_counter+1))
    fi
  fi  
done < "$filename"
