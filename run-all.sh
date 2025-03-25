#! /bin/bash

gcc     seqms.c     -o seqms    -fopenmp
gcc     parms.c     -o parms    -fopenmp
mpicc   mpims.c     -o mpims
bash run-sequential-tests.sh  $1
bash run-parallel-tests.sh    $1
bash run-distributed-tests.sh $1
mkdir -p Pictures
python3 plot-graphs.py