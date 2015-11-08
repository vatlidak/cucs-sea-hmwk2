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

## Notes
* Files and directories are created atomically with the use of umask

## Installation
* The installation needs root priviledges to:
  - Create a user "print_spoller"
  - Create a directory "/var/print_spooler"
  - Change owner and enable setuid bit to command executables

 * TODO: check copy works properly
 * TODO: check how copying files and permission of source work because
 * 		caller will have another uid of his file when permissions
 * 		are escalated.
 * TODO: Assign ids and stuff..



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
