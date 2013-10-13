CC = arm-linux-gcc
STRIP = arm-linux-strip
EXEC = buzzer
SRC = buzzer.c

all:	$(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(SRC)
	$(STRIP) $(EXEC)

clean:
	rm -f $(EXEC)
