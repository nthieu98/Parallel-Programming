mpicc DLA.c -lm
mpirun -np 4 ./a.out
python3 visual.py