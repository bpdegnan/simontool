#!/bin/bash
#Before anything else, set the PATH_SCRIPT variable
	pushd `dirname $0` > /dev/null; PATH_SCRIPT=`pwd -P`; popd > /dev/null
	PROGNAME=${0##*/}; PROGVERSION=0.1.0 

DIR_HASHLOCATION=.
FILE_TMP=/tmp/hash.sort
#remove the header
FILE_RAW=/tmp/hash.raw
FILE_COLLISION=/tmp/hash.hits

#get the counts of lines
sort $DIR_HASHLOCATION/$1 | uniq -c | grep -v '^ *1 ' | sort -nr > $FILE_TMP
COUNT_COLLISIONS=$(awk '{ sum+=$1} END {print sum}' $FILE_TMP | sed -e 's/  *$//')
COUNT_TOTALLINES=$(cat $DIR_HASHLOCATION/$1 | sed '/^#/ d' | wc -l | sed -e 's/  *$//')

case "$COUNT_COLLISIONS" in
  "") COUNT_COLLISIONS=0;;
esac

echo "COUNT_COLLISIONS $COUNT_COLLISIONS"
echo "COUNT_TOTALLINES $COUNT_TOTALLINES"


#find the hits if Collision exists

if [ $COUNT_COLLISIONS -ne 0 ]; then
  cat $DIR_HASHLOCATION/$1 | sed '/^#/ d' > $FILE_RAW
  awk '{print $2}' $FILE_TMP > $FILE_COLLISION

  VAL_FIRSTHIT=$(awk 'NR==FNR{a[$1]++;next}{if(a[$1]>=1){print $1;exit}}' $FILE_COLLISION $FILE_RAW)

  echo "hit: $VAL_FIRSTHIT"

 COUNT_COLLOC=$(grep -n "$VAL_FIRSTHIT" $FILE_RAW | head -n 1 | awk -F':' '{print $1}')
#if there's a collsions, there must be two.
 COUNT_COLLOC2=$(grep -n "$VAL_FIRSTHIT" $FILE_RAW | head -n 2 | tail -n 1 | awk -F':' '{print $1}')
 
 GAP=$((COUNT_COLLOC2-COUNT_COLLOC))

  echo "col loc: $COUNT_COLLOC"
  echo "col2loc: $COUNT_COLLOC2"
  echo "GAP: $GAP"
  
else
  echo "hit: 0"
  echo "col loc: 0"
  echo "col2loc: 0"
  echo "GAP: 0"
fi

