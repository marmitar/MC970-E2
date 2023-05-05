#!/bin/bash
../bench/wait.sh $(qsub -N openmp-01-matmul ../bench/cenapad.pbs)
