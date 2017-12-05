#!/bin/bash
export OMP_NUM_THREADS=8
mpirun -np 16 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
1000_10000.txt /home/yhtseng2/CS484-Graph-Coloring/code/parallel/output/1000_10000_colored.txt
mpirun -np 16 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
1000_100000.txt /home/yhtseng2/CS484-Graph-Coloring/code/parallel/output/1000_100000_colored.txt
mpirun -np 16 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
10000_100000.txt /home/yhtseng2/CS484-Graph-Coloring/code/parallel/output/10000_100000_colored.txt
mpirun -np 16 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
10000_1000000.txt /home/yhtseng2/CS484-Graph-Coloring/code/parallel/output/10000_1000000_colored.txt
#mpirun -np 2 ./graphColoring /home/yhtseng2/CS484-Graph-Coloring/generation/\
#1000000_10000000.txt /home/yhtseng2/CS484-Graph-Coloring/code/parallel/output/10000_1000000_colored.txt
