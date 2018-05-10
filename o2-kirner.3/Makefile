CC = gcc
CFLAGS = -std=gnu99
OBJS1 = master1.o
OBJS2 = child.o
PROG1 = master
PROG2 = child

all: $(PROG1) $(PROG2)

$(PROG1) : $(OBJS1)
	$(CC) $(CFLAGS) $(OBJS1) -o $@
$(PROG2) : $(OBJS2)
	$(CC) $(CFLAGS) $(OBJS2) -o $@
clean:
	rm -f *~ *.o $(PROG1)
	rm -f *~ *.o $(PROG2)

