#!/bin/bash

JOBID="$(echo $@)"
if [[ -z "$JOBID" ]]
then
    echo invalid jobid: $@
    exit 1
fi

UPDATE_SECS=3
while qstat "$JOBID"
do
    sleep $UPDATE_SECS
done
echo DONE
