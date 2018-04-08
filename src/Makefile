CPP       = GNU_G++
CPP_FLAGS = -g -Wall -O2 -m64 -DLINUX

APP_DIR = ..
BIN_DIR = $(APP_DIR)/bin

APP_INC   = -I$(APP_DIR)/include
XDBO_INCL = -I$(XDBO2_HOME)/include
INCL      = $(APP_INC) $(XDBO_INCL)

XDBO_LIB  = -L$(XDBO2_HOME)/lib -lxdbo2
LIBS      = $(XDBO_LIB)

des   = $(BIN_DIR)/check_delay
des_t = $(des)_test
srcs  = $(wildcard *.cpp)
objs  = $(srcs:.cpp=.o)

.PHONY: all test clean

all: $(des)
	@echo ALL MADE!

test: CPP_FLAGS += -DTEST
test: $(des_t)
	@echo TEST MADE!

$(des): $(objs)
	$(CPP) $(CPP_FLAGS) -o $@ $^ $(LIBS)

$(des_t): $(objs)
	$(CPP) $(CPP_FLAGS) -o $@ $^ $(LIBS)

.cpp.o:
	$(CPP) $(CPP_FLAGS) $(INCL) -c $<

clean:
	@echo Deleting $(des)
	@rm -f $(des)
	@echo Deleting $(objs)
	@rm -f $(objs)

tclean:
	@echo Deleting $(des_t)
	@rm -f $(des_t)
	@echo Deleting $(objs)
	@rm -f $(objs)

