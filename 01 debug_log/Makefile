
#CC := mips-openwrt-linux-gcc
CC := gcc

FLAGS := -I./

RM := rm -rf

WFLAG := -w

LIBS := 	-lpthread



CONFIG_C_SRCS += \
./main.c	\
./debug.c




# target
debug.x86: $(CONFIG_C_SRCS)
	$(CC) $(WFLAG) -o $@ $(FLAGS) $(CONFIG_C_SRCS) $(LIBS)


# clean Targets
clean:
	-$(RM) *.o *.debug.x86  *.exe
