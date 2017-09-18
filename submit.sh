#!/bin/bash
### script to run an mpi job using 12-core or less (using only one 12 -core node)
### Set the job name
#PBS -N dakre_ds_job
### Specify the PI group for this job
### List of PI groups available to each user can be found with "va" command
#PBS -W group_list=ece677fall17
### Request email when job begins and ends
#PBS -m bea
### Specify email address to use for notification
#PBS -M dakre@email.arizona.edu
### Set the queue for this job as windfall
#PBS -q windfall
### Set the number of nodes, cores, and memory that will be used for this job
#PBS -l select=4:ncpus=28:mem=168gb
### Specify "wallclock time" required for this job, hhh:mm:ss
#PBS -l walltime=0:5:0
### Specify total cpu time required for this job, hhh:mm:ss
### total cputime = walltime * ncpus
#PBS -l cput=0:28:0
#PBS -l place=free:shared
### Load required modules/libraries if needed (openmpi example)
### Use "module avail" command to list all available modules

# Check input arguments
if [[ -z ${marg} ]]; then
    echo "Invalid input arguments"
    echo "For example 'qsub -v marg=1 submit.sh'"
    exit 0
fi

module load openmpi
### set directory for job execution
cd /extra/dakre/mpi_matrix
### run your executable program with begin and end date and time output
date
mpirun -n 4 matrix ${marg}
date
