#!/bin/sh
#Before anything else, set the PATH_SCRIPT variable
	pushd `dirname $0` > /dev/null; PATH_SCRIPT=`pwd -P`; popd > /dev/null
	PROGNAME=${0##*/}; PROGVERSION=0.1.0 

###############
# configuration values 
DIR_RESTRICTED=./restricted
FILE_OUTPUT=./public/restricted.php


function ip2long() {
 echo "$1" | awk -F\. '{print ($4)+($3*256)+($2*256*256)+($1*256*256*256)}'
}

if [ ! -d "$DIR_RESTRICTED" ]; then
  echo "$DIR_RESTRICTED not found."
  exit
fi

#delete the PHP file if it exists
if [[ -f $FILE_OUTPUT ]]; then
    read -p "File $FILE_OUTPUT exists. Do you want to delete and recreate? [y/n] " l_keystroke
    if [[ $l_keystroke == [yY] ]]; then  ## Only delete the file if y or Y is pressed
        rm "$FILE_OUTPUT" && echo "File deleted."  ## Only echo "File deleted." if it was actually deleted and no error has happened. rm probably would send its own error info if it fails.
    fi
fi
#create the start of the phpfile
echo "<?php" >> $FILE_OUTPUT
echo "\$a_restrictedip=array();" >> $FILE_OUTPUT


#loop through the restricted directory and create a huge array of IP addresses
for l_filename in "$DIR_RESTRICTED"/*
do
#  echo "$l_filename"
	while read -r line
	do
	  i_strlen=${#line}   #string length
	  if [ "$i_strlen" -gt "0" ]; then  #make sure that we do not have a zero length line
		char_start=${line:0:1} #get the first character of the line
		if [ $char_start != '#' ]  # '#' is used as my comment character
		then
		  arg_startip=$(echo $line|awk '{print $1;}')  #IP start range
		  arg_endip=$(echo $line|awk '{print $2;}')  #IP end range
		  arg_startip=$(ip2long $arg_startip);
		  arg_endip=$(ip2long $arg_endip);
		  echo "\$a_restrictedip[]=array($arg_startip,$arg_endip);" >> $FILE_OUTPUT
		    
		fi
	  fi  
	done < "$l_filename"
done

#echo "?>" >> $FILE_OUTPUT


