PRG    = toolsDataDump
INC    = include

CFLAGS = -g -Wall -I $(INC)
CC     = gcc $(CFLAGS)
LFLAGS = -lm
OBJ    = toolsDataDump.o


$(PRG) : $(OBJ)
	$(CC) -o $@ $(LFLAGS) $^

.c.o:
	$(CC) -c $<

clean:
	rm -rf $(PRG) $(OBJ)
