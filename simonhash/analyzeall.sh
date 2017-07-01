#!/bin/bash
FILES=./hashfiles/*
let i=0
for f in $FILES
do
  echo "$i $f"
  bash ./analyzehash.sh $f
  let i=i+1
done
