#!/bin/bash

exec 2>errors.txt

directories=`find $1 -type d`

for directory in $directories; do
	files=`find $directory -maxdepth 1 -type f -size +$2c -size -$3c -print` 
	amount=`du -bc $files | wc -l`
	sizes=`du -bc $files | tail -1`
	sizes=${sizes%total} 
	echo $directory $(($amount - 1)) $sizes	
done

exec 2>&1

while read p; do
  echo "$(basename "$0"): $p" >$2
done <errors.txt


