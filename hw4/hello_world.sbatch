#!/bin/bash 
#SBATCH --nodes=3                        # requests 3 compute servers
#SBATCH --ntasks-per-node=2              # runs 2 tasks on each server
#SBATCH --cpus-per-task=1                # uses 1 compute core per task
#SBATCH --time=30:00
#SBATCH --mem=2GB
#SBATCH --job-name=hello_world
#SBATCH --output=hello_world.out

module purge
module load intel/19.1.2 openmpi/intel/4.0.5 

cd /scratch/npj226/MATH-GA-2012/hw4
mpirun ./hello_world
