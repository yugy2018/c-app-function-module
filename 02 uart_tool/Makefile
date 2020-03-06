
CC := gcc

FLAGS := -I./

RM := rm -rf

WFLAG := -w

LIBS := 	-lpthread



CONFIG_C_SRCS += \
./uart_tool.c	\




# target
uart_tool.x86: $(CONFIG_C_SRCS)
	$(CC) $(WFLAG) -o $@ $(FLAGS) $(CONFIG_C_SRCS) $(LIBS)


# clean Targets
clean:
	-$(RM) *.o *.x86  *.exe
