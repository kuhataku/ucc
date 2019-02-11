#!/bin/bash
try() {
  expected="$1"
  input="$2"

  echo "./ucc "$input" > tmp.s"
  ./ucc "$input" > tmp.s
  gcc -g -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "[P] $input => $actual"
  else
    echo "[F] $expected expected, but got $actual"
    exit 1
  fi
}

try 0 "0;"
try 42 "42;"
try 21 '5+20-4;'
try 21 ' 5    + 20 - 4;'
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 7 "(3+5)/2; 1+6;"
try 3 "a=3;b=a;"
try 7 "aaa=3; bbb=aaa+4;"
try 1 "3==3;"
try 1 "3!=4;"
try 0 "3!=3;"
try 0 "3==4;"

echo OK
