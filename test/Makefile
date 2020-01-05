include ../Makefile.in
SRC = $(wildcard *.cc)
INC = $(wildcard *.hh)
OBJ = $(addsuffix .o, $(basename $(SRC)))

all: test.a

test.a: ${OBJ}
	- ${AR} r $@ $^
	- ${RANLIB} $@
	- cp $@ ../bin

%.o: %.cc %.hh
	${CXX} -c $< -o $@ ${CPPFLAGS}

clean:
	rm -rf *.o *.a

.PHONY: clean
