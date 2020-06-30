A small program to convert a CSV to JSON file in C, mainly to practice working

with memory management, pointer arrays and file I/O.


Install
========

1. Clone repo.
2. Run `make`
3. csv2json binary should appear[1]


Small benchmarks
================

Using C-based[2] program to convert 1,000-row data file.

`time ./csv2json ./data.csv`
---------------------------
real	0m0.011s
user	0m0.006s
sys	0m0.003s

Using the Node-based csvtojson:

`time csvtojson ./data.csv`
---------------------------
real	0m0.146s
user	0m0.105s
sys	0m0.040s


[1]: Will fix to send to proper binary folder
[2]: While C is normally faster, I've found the same speed results for a similar
program written in Golang.
