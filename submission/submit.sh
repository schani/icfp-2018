#!/bin/bash

if [ -z $1 ]
  then
    echo "Private ID required!"
    echo "$0 [Private ID]"
    exit 1
fi


REV=$(git rev-parse --short HEAD)
FILE=submission_$REV.zip
ID=$1
DIR=submission_$REV
URL=http://tilab.tuwien.ac.at/icfp2018

rm -rf $DIR
mkdir $DIR

zip --password $ID $DIR/$FILE -j Traces/*
SHA=$(shasum -a 256 $DIR/$FILE | awk '{ print $1 }')
touch $DIR/$SHA

##### copy zipfile to server
## TODO: heinz please copy file to URL
## copy $DIR/$FILE to $URL


## TODO: remove echo
echo "\
curl -L \
  --data-urlencode action=submit \
  --data-urlencode privateID=$ID \
  --data-urlencode submissionURL=$URL/$FILE \
  --data-urlencode submissionSHA=$SHA \
  https://script.google.com/macros/s/AKfycbzQ7Etsj7NXCN5thGthCvApancl5vni5SFsb1UoKgZQwTzXlrH7/exec
"
