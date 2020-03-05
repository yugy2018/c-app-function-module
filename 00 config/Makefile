
CC := gcc

FLAGS := -I./

RM := rm -rf

WFLAG := -w

LIBS := 



CONFIG_C_SRCS += \
./main.c	\
./conf.c




# target
config.x86: $(CONFIG_C_SRCS)
	$(CC) $(WFLAG) -o $@ $(FLAGS) $(CONFIG_C_SRCS) $(LIBS)


# clean Targets
clean:
	-$(RM) *.o *.d  *.exe
