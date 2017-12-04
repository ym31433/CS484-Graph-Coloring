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
	gcc -O2 -o "./graphColoring" $(OBJS) $(CCFLAG) $(INCLUDE)
	@echo 'Finished building target: $@'
	@echo ' '

%.o: %.c $(HEADERS)
	gcc -O2 -c $< -o $@ $(CCFLAG) $(INCLUDE)

clean:
	-$(RM) $(OBJS)
	-$(RM) ./graphColoring
	-@echo ' '

.PHONY: all clean dependents directories
.SECONDARY:

