RM := rm -rf

HEADERS = 
OBJS = main.o
CCFLAG = -std=c99 \
-DCHECKER#\
-DDEBUG

all: clean graphColoring

graphColoring: $(OBJS)
	@echo 'Building target: $@'
	mpicc -O2 -o "./graphColoring" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

%.o: %.c $(HEADERS)
	mpicc -O2 -c $< -o $@ $(CCFLAG)

xeon:
	icc -mmic -qopenmp main.c -o main.mic

clean:
	-$(RM) $(OBJS)
	-$(RM) ./graphColoring ./main.mic
	-@echo ' '

.PHONY: all clean dependents directories
.SECONDARY:

