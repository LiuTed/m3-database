#!/bin/sh
cat output.log | awk '{if ($6 > 0) print $6}' | sort -n | cat -n > error.txt
./get_first_five.py | sort -n | cat -n > error_near.txt

awk '{if ($1 > 0 && $1 < 10.1) print $1}' warning.log | sort -n | cat -n > warning.txt

gnuplot db_error.gp
