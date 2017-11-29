#!/bin/bash

mpirun -np 4 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
1000_10000.txt
#1000_10000.txt
#1000_5000.txt
#100_1000.txt
#40_400.txt
mpirun -np 4 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
1000_100000.txt
mpirun -np 4 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
10000_100000.txt
mpirun -np 4 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
10000_1000000.txt
