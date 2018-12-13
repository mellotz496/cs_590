#!/bin/bash
BIN=getLPF
SRC=$BIN.c

clang -w -o $BIN $SRC >& /dev/null || exit 1
output=tests.csv
outfile=outfile.txt
for num in 12 56 135 278 393 428 511 621 739 812 911 1029 1204 1365 1477 1538 1722 14890 23789 578900
do
    ./$BIN $num > $outfile
    read -r outval < $outfile
    printf '%lld,%lld\n' "$num" "$outval" >> $output
done
rm $outfile
rm $BIN
    
    
