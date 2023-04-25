#!/bin/bash 
#SBATCH --nodes=2            # requests 2 compute server
#SBATCH --ntasks-per-node=1  # runs 1 tasks on each server
#SBATCH --cpus-per-task=1    # uses 1 compute core per task
#SBATCH --time=10:00
#SBATCH --mem=2GB
#SBATCH --job-name=pingpong
#SBATCH --output=pingpong.out

# setup 
cd /scratch/npj226/MATH-GA-2012/hw4
module purge
module load intel/19.1.2 openmpi/intel/4.0.5 

# run
mpirun ./hello_world   # print processor names and ranks
mpirun ./pingpong 0 1  # run program
