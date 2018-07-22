#!/bin/bash

SEDLIST=(/usr/local/bin/gsed /bin/sed /usr/bin/sed)

for SED in "${SEDLIST[@]}"
do
    if [ -x "$SED" ]
    then
	"$SED" "$@"
	exit $?
    fi
done

echo "$0: FATAL ERROR: sed not found" >&2
exit 1
