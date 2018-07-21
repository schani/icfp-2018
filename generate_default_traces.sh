#!/bin/bash

MODELS=`cd lightning_problems; ls *mdl`

make -C src generate_default_trace
make -C Bertl ascii2nbt
for file in $MODELS; do
	echo Converting $file to submission/Traces/${file/_tgt.mdl/.nbt}
	src/generate_default_trace lightning_problems/$file | Bertl/ascii2nbt > submission/Traces/${file/_tgt.mdl/.nbt}
done
