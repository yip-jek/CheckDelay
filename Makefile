.PHONY: all clean

all:
	@make -C src

test:
	@make -C src test

clean:
	@make -C src clean

tclean:
	@make -C src tclean

