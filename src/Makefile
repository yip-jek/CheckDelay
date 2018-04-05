CPP = g++
CPP_FLAGS = -g -w -O2 -m64 -DLINUX

APP_DIR = ..
BIN_DIR = $(APP_DIR)/bin

APP_INC   = -I$(APP_DIR)/include
INCL = $(APP_INC)

des = $(BIN_DIR)/check_delay
srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)

.PHONY: all test clean

all: $(des)
	@echo ALL MADE!

test: CPP_FLAGS += -DTEST
test: all

$(des): $(objs)
	$(CPP) $(CPP_FLAGS) -o $@ $^

.cpp.o:
	$(CPP) $(CPP_FLAGS) $(INCL) -c $<

clean:
	@echo Deleting $(des)
	@rm -f $(des)
	@echo Deleting $(objs)
	@rm -f $(objs)

