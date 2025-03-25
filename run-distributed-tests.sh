#! /bin/bash

# Run the program multiple times and register the result.

SCRIPT_DIRNAME=$(dirname "$0")
SAVE_NAME=distributed-tests.csv
SAVE_PATH=$SCRIPT_DIRNAME/$SAVE_NAME

PROGRAM_FILE=mpims
ARG_ARRAY_SIZES=(10 100 1000 10000 100000 1000000)
ARG_PROCS_NUMS=(2 4 8)


# Not empty and is a number
NUMBER_RE='^[0-9]+$'
if [[ -n "$1" && "$1" =~ $NUMBER_RE ]]
then
    RUN_REPETITIONS=$1
else
    echo    ""
    echo    "USAGE"
    echo    "       run-distributed-tests.sh RUN_REPETITIONS"
    echo    ""
    echo    "DESCRIPTION"
    echo    "       Runs mpims repeatedly and stores the results in sequential-tests.csv."
    echo    ""
    echo    "       For each of the arraySize 10, 100, 1000, 10000, 100000 and 1000000; runs parms RUN_REPETITIONS times."
    echo    "       Also repeats each run with the mpiCommSize 2, 4, and 8 (if available)."
    echo    "       RUN_REPETITIONS must be an integer value bigger than 0."
    echo    "       The time saved is in milliseconds."
    echo    ""
    exit
fi



# Initialize the csv file
SAVE_HEADER="Run,Num_procs,Array_size,Time_ms"
echo "$SAVE_HEADER"
echo "$SAVE_HEADER" > "$SAVE_PATH"

# Saving some data
RUN_INDEX=1;
WARNING_PROCS=()
for n in ${ARG_PROCS_NUMS[@]}
do
    # if mpirun -n 1 true 2> /dev/null; then echo "yay"; else echo "nay"; fi;
    if mpirun -n $n true 2> /dev/null
    then
        
        for s in ${ARG_ARRAY_SIZES[@]}
        do

            for ((i = 1; i < $RUN_REPETITIONS+1; i++))
            do
                echo "$RUN_INDEX,$n,$s,$(mpirun -n $n "$SCRIPT_DIRNAME/$PROGRAM_FILE" $s -qqm)" | tee --append "$SAVE_PATH"

                ((RUN_INDEX+=1))
            done

        done
    
    elif mpirun --use-hwthread-cpus -n $n true 2> /dev/null
    then

        WARNING_PROCS+=("$n")
        
        for s in ${ARG_ARRAY_SIZES[@]}
        do

            for ((i = 1; i < $RUN_REPETITIONS+1; i++))
            do
                echo "$RUN_INDEX,$n,$s,$(mpirun --use-hwthread-cpus -n $n "$SCRIPT_DIRNAME/$PROGRAM_FILE" $s -qqm)" | tee --append "$SAVE_PATH"

                ((RUN_INDEX+=1))
            done

        done
    
    fi

done

for warn in "${WARNING_PROCS[@]}"
do
    echo "Warning: ARG_PROCS_NUMS=$warn ran with '--use-hwthread-cpus'."
done




