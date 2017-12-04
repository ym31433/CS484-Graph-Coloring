RM := rm -rf

HEADERS = util.h
OBJS = main.o util.o
CCFLAG = -std=c99 -fopenmp #\
-DCHECKER#\
-DDEBUG
INCLUDE = #-I../lib/

all: clean graphColoring

graphColoring: $(OBJS)
	@echo 'Building target: $@'
	mpicc -O2 -o "./graphColoring" $(OBJS) $(CCFLAG) $(INCLUDE)
	@echo 'Finished building target: $@'
	@echo ' '

%.o: %.c $(HEADERS)
	mpicc -O2 -c $< -o $@ $(CCFLAG) $(INCLUDE)

xeon:
	icc -mmic -qopenmp main.c -o main.mic

clean:
	-$(RM) $(OBJS)
	-$(RM) ./graphColoring ./main.mic
	-@echo ' '

.PHONY: all clean dependents directories
.SECONDARY:

