#!/bin/bash
BIN=new_basic
SRC=$BIN.c

clang -w -o $BIN $SRC >& /dev/null || exit 1
input=tests.csv
counter=1
outfile=out
while IFS=',' read inp exp;
do
    gtimeout 0.1 ./$BIN $inp > $outfile
    if [ -e $outfile ] && [ -s $outfile ];
    then
	read -r outval < $outfile
	if (("$outval" != "$exp"));
	then
	    sed -i -e "${counter}d" $input
	    echo "$inp,$exp" | cat - $input > temp && mv temp $input
	    rm $outfile *.-e
	    exit 1
	fi
    else
	rm $outfile
	exit 1
    fi
    let "counter+=1"
done <"$input"
rm $outfile
