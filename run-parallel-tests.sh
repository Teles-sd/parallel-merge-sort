#! /bin/bash

# Run the program multiple times and register the result.

SCRIPT_DIRNAME=$(dirname "$0")
SAVE_NAME=parallel-tests.csv
SAVE_PATH=$SCRIPT_DIRNAME/$SAVE_NAME

PROGRAM_FILE=parms
ARG_ARRAY_SIZES=(10 100 1000 10000 100000 1000000)
# ARG_THREAD_NUMS=(2 4 6 8 12)
ARG_THREAD_NUMS=(2 4 8)

# ls "$SCRIPT_DIRNAME"
# echo "'$SCRIPT_DIRNAME'"
# echo "'$SAVE_PATH'"
# ls "$SCRIPT_DIRNAME/$PROGRAM_FILE"
# "$SCRIPT_DIRNAME/$PROGRAM_FILE" -h

# Not empty and is a number
NUMBER_RE='^[0-9]+$'
if [[ -n "$1" && "$1" =~ $NUMBER_RE ]]
then
    RUN_REPETITIONS=$1
else
    echo    ""
    echo    "USAGE"
    echo    "       run-parallel-tests.sh RUN_REPETITIONS"
    echo    ""
    echo    "DESCRIPTION"
    echo    "       Runs parms repeatedly and stores the results in sequential-tests.csv."
    echo    ""
    echo    "       For each of the arraySize 10, 100, 1000, 10000, 100000 and 1000000; runs parms RUN_REPETITIONS times."
    echo    "       Also repeats each run with the numThreads 2, 4, 6, 8 and 12 (if available)."
    echo    "       RUN_REPETITIONS must be an integer value bigger than 0."
    echo    "       The time saved is in milliseconds."
    echo    ""
    exit
fi

# for n in ${ARG_THREAD_NUMS[@]}
# do
#     if "$SCRIPT_DIRNAME/$PROGRAM_FILE" -N $n
#     then
#         echo "Valid ($n)"
#     else
#         echo "Invalid ($n)"
#     fi
# done


# Initialize the csv file
SAVE_HEADER="Run,Num_threads,Array_size,Time_ms"
echo "$SAVE_HEADER"
echo "$SAVE_HEADER" > "$SAVE_PATH"

# Saving some data
RUN_INDEX=1;
for n in ${ARG_THREAD_NUMS[@]}
do
    if "$SCRIPT_DIRNAME/$PROGRAM_FILE" -N $n
    then
        
        for s in ${ARG_ARRAY_SIZES[@]}
        do

            for ((i = 1; i < $RUN_REPETITIONS+1; i++))
            do
                # echo "$i,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" 10 -qqm)"
                # echo "$i,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" 1000000 -qqm)"
                # echo "$RUN_INDEX,$s,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" $s -qqm)"
                echo "$RUN_INDEX,$n,$s,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" $s -n $n -qqm)" | tee --append "$SAVE_PATH"

                ((RUN_INDEX+=1))
            done

        done
    
    fi

done


