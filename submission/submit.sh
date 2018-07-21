#!/bin/bash

if [ -z "$1" ]
  then
    echo "Private ID required!"
    echo "$0 [Private ID]"
    exit 1
fi

GENTRACEDIR=../computing/result/

if [ -z "$(ls -A $GENTRACEDIR)" ]; then
    echo "no traces found"
    exit 1
fi

echo "getting traces from $GENTRACEDIR"
rm -rf /tmp/icfp/*
for filename in $GENTRACEDIR/*.tar.gz; do
    tar -C /tmp/icfp -xzf "$filename" ./result
done
rm -f Traces/*
cp /tmp/icfp/result/*.nbt Traces/.

REV=$(git rev-parse --short HEAD)
TS=$(date +%Y%m%d_%H%M%S)
FILE=submission_${TS}_$REV
#SRCFILE=source_${TS}_$REV
ID=$1
DIR=submission_${TS}_$REV
URL=http://www.tilab.tuwien.ac.at/icfp2018

rm -rf "$DIR"
mkdir "$DIR"

echo "packing"
zip --password "$ID" "$DIR/$FILE.zip" -j Traces/*
SHA=$(sha256sum "$DIR/$FILE.zip" | awk '{ print $1 }')
(cd "$DIR"; sha256sum "$FILE.zip" > "$FILE.sha256")

## generate source code zip and hash
#(cd ..; zip submission/$DIR/$SRCFILE.zip -r src)
#sha256sum $DIR/$SRCFILE.zip > $DIR/$SRCFILE.sha


##### copy zipfile to server
## TODO: heinz please copy file to URL
## copy $DIR/$FILE to $URL
# note: this will only work from inside tuwien
scp "$DIR/$FILE.*" icfp2018@www.tilab.tuwien.ac.at:html/.

curl -L \
  --data-urlencode action=submit \
  --data-urlencode privateID="$ID" \
  --data-urlencode submissionURL="$URL/$FILE.zip" \
  --data-urlencode submissionSHA="$SHA" \
  https://script.google.com/macros/s/AKfycbzQ7Etsj7NXCN5thGthCvApancl5vni5SFsb1UoKgZQwTzXlrH7/exec
