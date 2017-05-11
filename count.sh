#!/bin/sh
a=`find "$1" -maxdepth $(($2+1)) -type f | wc -l`
b=`find "$1" -maxdepth $(($2+1)) -type d | wc -l`
b=$(($b-1))
echo nr fisiere $a nr foldere $b
