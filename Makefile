CC := gcc
CFLAGS := -Wall -Iinclude
LDFLAGS :=

ifeq ($(DEBUG),1)
CFLAGS += -D_DEBUG
endif

OBJECTS := addqueue.o rmqueue.o showqueue.o
EXECUTABLES := $(OBJECTS:.o=)

.PHONY: build addqueue rmqueue showqueue clean

build: addqueue rmqueue showqueue

addqueue: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o addqueue addqueue.o
	@rm addqueue.o

rmqueue: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o rmqueue rmqueue.o
	@rm rmqueue.o

showqueue: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o showqueue showqueue.o
	@rm showqueue.o

%.o: src/%.c
	@$(CC) $(CFLAGS) -c $^

#test: clean build
#	cat $(TEST) | $(EXECUTABLE) 2>/dev/null
#
#exec: clean build
#	$(EXECUTABLE) 2>/dev/null
#
#valgrid: clean build
#	/usr/bin/valgrind -v --input-fd=3 < $(TEST) $(EXECUTABLE)

checkpatch:
	scripts/checkpatch.pl --no-tree -f src/*

clean:
	rm -f $(EXECUTABLES)
	rm -f $(OBJECTS)
