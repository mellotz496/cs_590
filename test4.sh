#!/bin/bash
BIN=from_paper
SRC=$BIN.c

clang -w -o $BIN $SRC >& /dev/null || exit 1
