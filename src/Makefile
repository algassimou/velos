EXEC = faire 
CLIENT = client
SERVEUR = serveur
PARK = park
CLIENT = client
LIBNAME = libcycle.a

SRC1 = $(wildcard *.c)
OBJ1 = $(SRC1:.c=.o)

all : archive  $(EXEC)

$(EXEC) : 
	@(cd $(PARK) && $(MAKE))
	@(cd $(CLIENT) && $(MAKE))
	@(cd $(SERVEUR) && $(MAKE))

archive : $(OBJ1)
	@(mv $(LIBNAME) $(LIB))
%.o : %.c
	@($(CC) -I$(INCLUDE) $(CFLAGS) $^ -o $@)
	@(ar -r $(LIBNAME) $@)

.PHONY : clean mrproper $(EXEC)

clean :
	@(rm -f *.o)
	@(cd $(PARK) && $(MAKE) $@)
	@(cd $(CLIENT) && $(MAKE) $@)
	@(cd $(SERVEUR) && $(MAKE) $@)

mrproper : 
	@(rm -f $(LIB)/$(LIBNAME))
	@(cd $(PARK) && $(MAKE) $@)
	@(cd $(CLIENT) && $(MAKE) $@)
	@(cd $(SERVEUR) && $(MAKE) $@)

