#!/bin/bash
# (See https://arc-ts.umich.edu/greatlakes/user-guide/ for command details)
# Set up batch job settings

#SBATCH --job-name=submit
#SBATCH --mail-type=BEGIN,END
#SBATCH --nodes=1
#SBATCH --cpus-per-task=9
#SBATCH --exclusive
#SBATCH --mem-per-cpu=1g
#SBATCH --time=00:05:00
#SBATCH --account=eecs587f21_class
#SBATCH --partition=standard

# Run your program
# (">" redirects the print output of your pfogram,
# in this case to "output.txt")

g++ -std=c++11 -O0 -fopenmp -o s_9 hw3.cpp
./s_9