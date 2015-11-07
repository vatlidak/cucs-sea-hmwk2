CC := gcc
CFLAGS := -Wall -Iinclude
LDFLAGS :=

BIN := bin
OBJECTS := addqueue.o rmqueue.o showqueue.o
EXECUTABLES := $(BIN)/$(OBJECTS:.o=)

.PHONY: build addqueue rmqueue showqueue clean

build: addqueue rmqueue showqueue

addqueue: $(OBJECTS)
	@[ -d $(BIN) ] || mkdir $(BIN)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN)/addqueue addqueue.o
	@rm addqueue.o

rmqueue: $(OBJECTS)
	@[ -d $(BIN) ] || mkdir $(BIN)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN)/rmqueue rmqueue.o
	@rm rmqueue.o

showqueue: $(OBJECTS)
	@[ -d $(BIN) ] || mkdir $(BIN)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN)/showqueue showqueue.o
	@rm showqueue.o

%.o: src/%.c
	@$(CC) $(CFLAGS) -c $^

install:
	@id -u print_spooler 2>/dev/null 1>/dev/null || sudo useradd print_spooler &&\
		sudo chown print_spooler $(BIN)/addqueue &&\
		sudo chown print_spooler $(BIN)/rmqueue &&\
	  sudo chown print_spooler $(BIN)/showqueue &&\
		sudo chmod u+s $(BIN)/addqueue &&\
		sudo chmod u+s $(BIN)/rmqueue &&\
		sudo chmod u+s $(BIN)/showqueue
	@echo "---------------------------"
	@echo "Installation Successful ;-)"
	@echo "---------------------------"

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
	rm -rf $(BIN)
	rm -f $(OBJECTS)
