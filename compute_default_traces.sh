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
    BINS=("src/generate_default_trace" "Bertl/ascii2nbt")
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
	binfile=${bin##*/}
	bindir=${bin%/*}
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

mkdir -p result
./generate_default_trace input-models/\$1 | ./ascii2nbt > result/\${1#.mdl}.nbt
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
