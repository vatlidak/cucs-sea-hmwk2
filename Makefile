CC := gcc
CFLAGS := -Wall -Iinclude
LDFLAGS :=
ARGS :=

OBJECTS := addqueue.o rmqueue.o showqueue.o
EXECUTABLES := $(BIN)/$(OBJECTS:.o=)

BIN := ./bin
TEST := ./tests
USR_BIN := /bin

ROOT_UNAME := "root"
PRINT_SPOOLER_PATH := "/var/spool/print"
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

install: build
	@id -u $(PRINT_SPOOLER_UNAME) >/dev/null 2>&1 ||\
		sudo useradd $(PRINT_SPOOLER_UNAME)
	@[ -d $(PRINT_SPOOLER_PATH) ] ||\
		(umask 0077 &&\
		 sudo mkdir $(PRINT_SPOOLER_PATH) &&\
		 sudo chown $(PRINT_SPOOLER_UNAME):$(PRINT_SPOOLER_UNAME) $(PRINT_SPOOLER_PATH))
	@[ ! -d $(BIN) ] ||\
		(sudo chown $(PRINT_SPOOLER_UNAME):$(ROOT_UNAME) $(BIN)/addqueue &&\
		 sudo chmod u+s $(BIN)/addqueue &&\
		 sudo chown $(PRINT_SPOOLER_UNAME):$(ROOT_UNAME) $(BIN)/rmqueue &&\
		 sudo chmod u+s $(BIN)/rmqueue &&\
		 sudo chown $(PRINT_SPOOLER_UNAME):$(ROOT_UNAME) $(BIN)/showqueue &&\
		 sudo chmod u+s $(BIN)/showqueue)
	@[ ! -d $(USR_BIN) ] ||\
		(sudo cp $(BIN)/addqueue $(USR_BIN) &&\
		 sudo chown $(PRINT_SPOOLER_UNAME):$(ROOT_UNAME) $(USR_BIN)/addqueue &&\
		 sudo chmod u+s $(USR_BIN)/addqueue &&\
		 sudo cp $(BIN)/rmqueue $(USR_BIN) &&\
		 sudo chown $(PRINT_SPOOLER_UNAME):$(ROOT_UNAME) $(USR_BIN)/rmqueue &&\
		 sudo chmod u+s $(USR_BIN)/rmqueue &&\
		 sudo cp $(BIN)/showqueue $(USR_BIN) &&\
		 sudo chown $(PRINT_SPOOLER_UNAME):$(ROOT_UNAME) $(USR_BIN)/showqueue &&\
		 sudo chmod u+s $(USR_BIN)/showqueue)
	@echo "-------------------------------------------------------"&&\
	echo "Installation Succesful -- unless you saw any errors ;-)"&&\
	echo "-------------------------------------------------------"

uninstall:
	@([ -d $(PRINT_SPOOLER_PATH) ] && sudo rm -rf $(PRINT_SPOOLER_PATH)) || true
	@(id -u $(PRINT_SPOOLER_UNAME) >/dev/null 2>&1 &&\
		sudo userdel $(PRINT_SPOOLER_UNAME) || true)
	@[ ! -f $(USR_BIN)/addqueue ]  || sudo rm $(USR_BIN)/addqueue
	@[ ! -f $(USR_BIN)/rmqueue ]   || sudo rm $(USR_BIN)/rmqueue
	@[ ! -f $(USR_BIN)/showqueue ] || sudo rm $(USR_BIN)/showqueue
	@echo "--------------------------------------------------------" &&\
	echo "Uninstalling Successful -- unless you saw any errors ;-)" &&\
	echo "--------------------------------------------------------"

test: 
	addqueue $(TEST)/*
	showqueue
	addqueue $(TEST)/*
	showqueue

exec_addqueue:
ifeq ($(ARGS),)
	@echo "Usage: make ARGS=filename1...filenameN"
else
	@addqueue $(ARGS) 2>/dev/null
endif

exec_rmqueue:
ifeq ($(ARGS),)
	@echo "Usage: make ARGS=filename1...filenameN"
else
	@rmqueue $(ARGS) 2>/dev/null
endif

exec_showqueue:
	@showqueue $(ARGS) 2>/dev/null

checkpatch:
	scripts/checkpatch.pl --no-tree -f src/*

clean:
	rm -rf $(BIN)
	rm -f $(OBJECTS)
