#!/bin/bash

TESTFILE="$(echo $@)"
if [[ -z "$TESTFILE" ]]
then
    TESTFILE="tests/5.in"
fi

sudo perf record -F 40000 -g -- build/parallel "$TESTFILE" > /dev/null

sudo perf script \
    | stackcollapse-perf.pl --all \
    | flamegraph.pl --width 1920 > flamegraph.svg

sudo rm perf.data perf.data.old
