#!/bin/bash

for (( i = 0; i < 10; i++ )); do
    echo $i
    for (( j = 0; j < 1000; j++ )); do
        ./client &
    done
    sleep 0.1
done

