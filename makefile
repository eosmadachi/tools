PRG    = toolsBinaryDump
INC    = include

CFLAGS = -g -Wall -I $(INC)
CC     = gcc $(CFLAGS)
LFLAGS = -lm
OBJ    = toolsBinaryDump.o


$(PRG) : $(OBJ)
	$(CC) -o $@ $(LFLAGS) $^

.c.o:
	$(CC) -c $<

clean:
	rm -rf $(PRG) $(OBJ)
