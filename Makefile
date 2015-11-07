CC := gcc
CFLAGS := -Wall -Iinclude
LDFLAGS :=

BIN := bin
OBJECTS := addqueue.o rmqueue.o showqueue.o
EXECUTABLES := $(BIN)/$(OBJECTS:.o=)

PRINT_SPOOLER_PATH := "/var/print_spooler"
PRINT_SPOOLER_UNAME := "print_spooler"

.PHONY: build addqueue rmqueue showqueue install clean

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
	@id -u $(PRINT_SPOOLER_UNAME) 2>/dev/null 1>/dev/null ||\
		sudo useradd print_spooler &&\
	[ -d $(PRINT_SPOOLER_PATH) ] || (sudo mkdir $(PRINT_SPOOLER_PATH) &&\
		sudo chown $(PRINT_SPOOLER_UNAME) $(PRINT_SPOOLER_PATH)) &&\
	sudo chown $(PRINT_SPOOLER_UNAME) $(BIN)/addqueue &&\
	sudo chown $(PRINT_SPOOLER_UNAME) $(BIN)/rmqueue &&\
  sudo chown $(PRINT_SPOOLER_UNAME) $(BIN)/showqueue &&\
	sudo chmod u+s $(BIN)/addqueue &&\
	sudo chmod u+s $(BIN)/rmqueue &&\
	sudo chmod u+s $(BIN)/showqueue &&\
	echo "-------------------------------------------------------"&&\
	echo "Installation Succesful -- unless you saw any errors ;-)"&&\
	echo "-------------------------------------------------------"

uninstall:
	@[ -d $(PRINT_SPOOLER_PATH) ] &&\
		sudo rm -rf $(PRINT_SPOOLER_PATH) &&\
	id -u $(PRINT_SPOOLER_UNAME) 2>/dev/null 1>/dev/null &&\
		sudo userdel $(PRINT_SPOOLER_UNAME) &&\
	echo "--------------------------------------------------------" &&\
	echo "Uninstalling Successful -- unless you saw any errors ;-)" &&\
	echo "--------------------------------------------------------"

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
