Simulate print spoller
==
Copyright (C) 2015 V. Atlidakis

COMS W4187 Fall 2015, Columbia University

## Project structure

* Makefile: Makefile
* include/: Header file for XXX
* src/addqueue.c: Implements addqueue command
* src/rmqueue.c: Implements rmqueue command
* src/showqueue.c: Implements showqueue command
* scripts/checkpatch.pl: Format checking script
* tests/test.txt: A demo set of test commands

## Notes - Conventions
XXX

## Error Handling
XXX

## Run
* make build
* make test (small demo using tests/test.txt)
* cat custom_test_file | make exec (test using custom file)
* make valgrid (memory leak checks)

## Debug
* make DEBUG=1
* make demo (small demo using tests/test.txt)
