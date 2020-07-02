A small program to convert a CSV to JSON file in Go.


Install
==============

1. Clone repo.
2. Run `make`
3. csv2json binary should appear[1]

This should work if you're using a Unix-based system.

How to use
===============

csv2json can read from a file and print to stdout.

$ csv2json ./in.csv > out.json

You can read from a pipe...

$ cat ./in.csv | csv2json > out.json

..or a redirect

csv2json <./in.csv > out.json


Small benchmarks
================

Using Go-based program to convert 1,000-row data file.

`time ./csv2json ./test-data/data.csv`
---------------------------
real	0m0.039s
user	0m0.011s
sys	0m0.009s

Using the Node-based csvtojson:

`time csvtojson ./test-data/data.csv`
---------------------------
real	0m0.223s
user	0m0.166s
sys	0m0.055s
