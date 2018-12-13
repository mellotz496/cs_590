#!/bin/bash
BIN=new_basic
SRC=$BIN.c

rm output.txt
clang -w -o $BIN $SRC >& /dev/null || exit 1
input=input.txt
while IFS= read -r line
do
    gtimeout 0.1 ./$BIN $line >> output.txt
done <"$input"
diff -q expected.txt output.txt > diff || exit 1
#diff -q expected.txt output.txt >& /dev/null || exit 1
rm $BIN
