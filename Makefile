SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
CFLAGS=-Wall -g

.PHONY: run
run: a.out
	@./a.out

a.out: $(OBJS)
	@gcc $^

.PHONY: clean
clean:
	rm -f *.o *~ a.out

