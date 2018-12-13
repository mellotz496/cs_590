#!/bin/bash
BIN=new_basic
SRC=$BIN.c

clang -w -o $BIN $SRC >& /dev/null || exit 1
input=tests.csv
outfile=out
while IFS=',' read inp exp;
do
    #echo "$inp, $exp"
    gtimeout 0.1 ./$BIN $inp > $outfile
    if [ -e $outfile ] && [ -s $outfile ];
    then
	read -r outval < $outfile
	#echo "$outval"
	if (("$outval" != "$exp"));
	then
	    #echo "UNEXPECTED OUTPUT-$inp, $outval"
	    printf '%d,%d\n' "$inp" "$exp" > failed_cases
	    rm out
	    exit 1
	fi
    else
	#echo "NO OUTPUT"
	exit 1
    fi
done <"$input"
#echo "SUCCESS"
rm out
