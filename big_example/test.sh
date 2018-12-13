#!/bin/bash
BIN=getLPF
SRC=$BIN.c

clang -w -o $BIN $SRC >& /dev/null || exit 1
input=tests.csv
counter=1
outfile=out
while IFS=',' read inp exp;
do
   # echo $inp
   # echo $exp
    gtimeout 0.5 ./$BIN $inp > $outfile
    if [ -e $outfile ] && [ -s $outfile ];
    then
	read -r outval < $outfile
	#echo $outval
	if (("$outval" != "$exp"));
	then
	    sed -i -e "${counter}d" $input
	    echo "$inp,$exp" | cat - $input > temp && mv temp $input
	    rm $outfile 
	    exit 1
	fi
    else
	rm $outfile
	exit 1
    fi
    let "counter+=1"
done <"$input"
rm $outfile
#rm *-e
#echo "SUCCESS"
