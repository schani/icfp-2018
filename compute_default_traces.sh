#!/bin/bash

source /usr/libexec/bash-base.bash || {
   echo "$0: fatal error:" \
      "failed to source /usr/libexec/bash-base/base.bash" >&2
   exit 1
}

function clean_builddir() {
    rm -rf "$BUILDDIR"
}

function main() {
    COMPUTEDIR="${PWD}/computing"
    BUILDSUBDIR="build"
    BUILDDIR="$COMPUTEDIR/$BUILDSUBDIR"
    BINS=("src/generate_default_trace" "src/build" "src/run_trace" "Bertl/ascii2nbt")
    MODELS=(lightning_problems/*.mdl)

    [ -d "computing" ] || fatal "must be called outside of computing dir"

    if [ -d "$BUILDDIR" ]
    then
	log "builddir '$BUILDDIR' already exists, giving up"
	bb_quit 1
    fi

    mkdir "$BUILDDIR" || fatal "failed to: mkdir '$BUILDDIR'"
    bb_register_exit_hook clean-builddir clean_builddir

    # compile and put binaries into builddir
    local binfile bindir
    for bin in "${BINS[@]}"
    do
	binfile="${bin##*/}"
	bindir="${bin%/*}"
	make -C "$bindir" "$binfile" || \
	    fatal "failed to: make -C '$bindir' '$binfile'"
	cp "$bin" "$BUILDDIR/." || fatal "failed to: '$bin' '$BUILDDIR/.'"
    done

    # move input data into builddir/input-models
    mkdir "$BUILDDIR/input-models" || \
	fatal "failed to: mkdir '$BUILDDIR/input-models'"
    cp "${MODELS[@]}" "$BUILDDIR/input-models/." || \
	fatal "failed to: cp '${MODELS[*]}' '$BUILDDIR/input-models/.'"

    # create run.sh
    cat > "$BUILDDIR/run.sh" << EOF
#!/bin/bash

set -o pipefail

mkdir -p result

mkdir -p tmp
SCORE=999999999999999999
BEST=
for bin in generate_default_trace build
do
  if ./\$bin input-models/\$1 | ./ascii2nbt > tmp/\$bin.nbt
  then
    if MAYBE=\$(./run_trace input-models/\$1 tmp/\$bin.nbt)
    then
      # looks valid
      echo \$MAYBE > tmp/\$bin.energy
      if (( MAYBE < SCORE ))
      then
        cp tmp/\$bin.nbt result/\${1%_tgt.mdl}.nbt
        SCORE=\$MAYBE
        BEST=\$bin
      fi
    fi
  else
    echo "\$bin failed" > tmp/\$bin.error
  fi
done
echo "winner was \$BEST with energy \$SCORE"
EOF
    chmod +x "$BUILDDIR/run.sh"

    # create arglist
    # run.sh will be called with each line of arglist
    for model in "${MODELS[@]}"
    do
	echo "${model##*/}" >> "$BUILDDIR/arglist"
    done

    # compute it
    cd "$COMPUTEDIR" || fatal "failed to: cd '$COMPUTEDIR'"
    ./compute.sh run "$BUILDSUBDIR" ./run.sh "$BUILDSUBDIR/arglist" result

    bb_unregister_exit_hook clean-builddir
}

main "$@"

bb_quit
