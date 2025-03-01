#! /bin/bash

# Run the program multiple times and register the result.

SCRIPT_DIRNAME=$(dirname "$0")
SAVE_NAME=sequential-tests.csv
SAVE_PATH=$SCRIPT_DIRNAME/$SAVE_NAME

PROGRAM_FILE=seqms
ARG_ARRAY_SIZES=(100 10000 1000000)


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
    # echo "String is not empty: '$RUN_REPETITIONS'"
# elif [[ -z "$1" ]]
    # echo    "String is empty: '$1'"
    # RUN_REPETITIONS=5
else
    echo    ""
    echo    "USAGE"
    echo    "       run-sequential-tests RUN_REPETITIONS"
    echo    ""
    echo    "DESCRIPTION"
    echo    "       Runs seqms repeatedly and stores the results in sequential-tests.csv."
    echo    ""
    echo    "       For each of the arraySize 100, 10000 and 1000000; runs seqms RUN_REPETITIONS times."
    echo    "       RUN_REPETITIONS must be an integer value bigger than 0."
    echo    "       The time saved is in milliseconds."
    echo    ""
    exit
fi


# Initialize the csv file
SAVE_HEADER="Run,Array_size,Time_ms"
echo "$SAVE_HEADER"
echo "$SAVE_HEADER" > "$SAVE_PATH"

# Saving some data
RUN_INDEX=1;
for s in ${ARG_ARRAY_SIZES[@]}; do

    for ((i = 1; i < $RUN_REPETITIONS+1; i++))
    do
        # echo "$i,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" 10 -qqm)"
        # echo "$i,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" 1000000 -qqm)"
        # echo "$RUN_INDEX,$s,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" $s -qqm)"
        echo "$RUN_INDEX,$s,$("$SCRIPT_DIRNAME/$PROGRAM_FILE" $s -qqm)" | tee --append "$SAVE_PATH"

        ((RUN_INDEX+=1))
    done

done


