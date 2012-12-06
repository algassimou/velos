EXEC = client
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all : $(EXEC)

$(EXEC) : $(OBJ)
	$(CC) -L$(LIB) -o $@ $^ $(LDFLAGS)
	@(mv $(EXEC) $(BIN))

%.o : %.c 
	@($(CC) -I$(INCLUDE) $(CFLAGS) $< -o $@)

.PHONY : clean mrproper $(EXEC)

clean :
	@(rm -f *.o)

mrproper : 
	@(rm -f $(BIN)/$(EXEC))
