
CC := gcc

FLAGS := -I./

RM := rm -rf

WFLAG := -w

LIBS :=



CONFIG_C_SRCS += \
./output_top_k.c	\




# target
output_top_k.x86: $(CONFIG_C_SRCS)
	$(CC) $(WFLAG) -o $@ $(FLAGS) $(CONFIG_C_SRCS) $(LIBS)


# clean Targets
clean:
	-$(RM) *.o *.x86  *.exe
