Simulate print spoller
==
Copyright (C) 2015 V. Atlidakis

COMS W4187 Fall 2015, Columbia University

## Project structure

* Makefile: Makefile
* include/: Header file for defines
* src/addqueue.c: Implements addqueue command
* src/rmqueue.c: Implements rmqueue command
* src/showqueue.c: Implements showqueue command
* scripts/checkpatch.pl: Format checking script
* tests: contain some test media files

## Notes
* Each file added in the queue is named after a 16-bytes unique
  file identifier randomly created from /dev/urandom.

## Build
* make [build]

## Installation
* make install
* The installation needs root priviledges to:
  - Create a user "print_spoller"
  - Create a directory "/var/print_spooler"
  - Change owner, enable setuid bit, and copy executables under /bin
