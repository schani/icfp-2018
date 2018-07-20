#!/bin/bash

# this demo script adds all numbers passed to it

i=0

while (( $# > 0 ))
do
    i=$((i + $1))
    shift
done

echo $i
