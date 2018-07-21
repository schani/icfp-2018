#!/bin/bash

source /usr/libexec/bash-base.bash || {
   echo "$0: fatal error:" \
      "failed to source /usr/libexec/bash-base/base.bash" >&2
   exit 1
}

function do_ssh() {
    (( $# == 3 )) || fatal "do_ssh called with $# args instead of 2"

    local index_nr="$1" h="$2" cmd="$3" prefix
    local index
    printf -v "index" "%06i" "${index_nr}"

    # shellcheck disable=SC2029
    if ssh -oConnectTimeout=5 -oBatchMode=yes -E "$TMPDIR/$index.$h.sshlog" \
	"$SSH_USER@$h" "$cmd" \
	2> "$TMPDIR/$index.$h.stderr" > "$TMPDIR/$index.$h.stdout"
    then
	prefix="+   "
    else
	local retval="$?"

	if (( retval == 255 ))
	then
	    if [ -s "$TMPDIR/$index.$h.sshlog" ]
	    then
		# probably a failure of ssh not on remote side
		prefix="!ssh"
	    else
		prefix="e255"
		touch "$TMPDIR/ERROR"
	    fi
	else
	    printf -v prefix "e%3i" "$retval"
	    touch "$TMPDIR/ERROR"
	fi
    fi
	
    echo -n "${prefix}" > "$TMPDIR/$index.$h"
    if [ -s "$TMPDIR/$index.$h.stdout" ]
    then
	echo -n " [STDOUT] " >> "$TMPDIR/$index.$h"
	tr '\n' ' ' < "$TMPDIR/$index.$h.stdout" | tr -d '\r' \
	    >> "$TMPDIR/$index.$h"
    fi
    if [ -s "$TMPDIR/$index.$h.stderr" ]
    then
	echo -n " [STDERR] " >> "$TMPDIR/$index.$h"
	tr '\n' ' ' < "$TMPDIR/$index.$h.stderr" | tr -d '\r' \
	    >> "$TMPDIR/$index.$h"
	touch "$TMPDIR/ERROR"
    fi
    if [ -s "$TMPDIR/$h.sshlog" ]
    then
	echo -n " [SSH] " >> "$TMPDIR/$index.$h"
        tr '\n' ' ' < "$TMPDIR/$index.$h.sshlog" | tr -d '\r' >> \
	    "$TMPDIR/$index.$h"
	touch "$TMPDIR/ERROR"
    fi
    echo "" >> "$TMPDIR/$index.$h"
    [ -a "$TMPDIR/ERROR" ] || {
	if [ -s "$TMPDIR/$index.$h.stdout" ]
	then
	    cat "$TMPDIR/$index.$h.stdout"
	fi
    }
    rm -f "$TMPDIR/$index.$h."{stdout,stderr,sshlog}
}

function ssh_show_results() {
    local f
    for f in "$TMPDIR"/*.*
    do
	cat "$f"
    done
}

function ssh_check_for_errors() {
    [ -f "$TMPDIR/ERROR" ] || return 0

    ssh_show_results
}

# currently if run runs out of nodes it will start over again
function run() {
    (( $# == 4 )) || fatal "run: got $# args instead of 4"

    local dir="$1" bin="$2" arglistfile="$3" resultdir="$4"
    local RUNDIR="$TMPDIR/RUN" RESULTDIR="$TMPDIR/RESULT"

    [ -a "$resultdir" ] && {
	log "run: aborting cause resultdir already exists: $resultdir"
	bb_quit 1
    }

    local runid=$(basename "$TMPDIR")

    mkdir "$RUNDIR" || fatal "run: failed to mkdir '$RUNDIR'"
    mkdir "$RESULTDIR" || fatal "run: failed to mkdir '$RESULTDIR'"

    [ -f "$arglistfile" ] || fatal "run: no such arglist file: $arglistfile"
    cp "$arglistfile" "$RUNDIR/arglistfile"

    if [ -d "$dir" ]
    then
	cp -a "$dir" "$RUNDIR/wd" || fatal "run: failed to cp -a"
	tar -C "$RUNDIR/wd" -czf "$RUNDIR/wd.tar.gz" "." || \
	    fatal "tar: failed to tar -czf"
    else
	fatal "run: no such dir found: $dir"
    fi

    local line index=0 nodenr=0 node
    while read -r line || [[ -n "$line" ]]
    do
	node="${nodes[$nodenr]}"
	#echo "DEBUG: index=$index nodenr=$nodenr lastnode=$node of ${#nodes[@]}"
	nodenr=$((nodenr + 1))
	(( nodenr < ${#nodes[@]} )) || nodenr=0

	local remotewd="$runid.$index"

	do_ssh "$index" "$node" "mkdir $remotewd && tar -C $remotewd -xzf - && cd $remotewd && \"$bin\" $line > STDOUT && cd ~ && tar -C $remotewd -czf - . && rm -rf $remotewd" < "$RUNDIR/wd.tar.gz" > "$RESULTDIR/$index-result.tar.gz" &
	index=$((index + 1))
    done < "$RUNDIR/arglistfile"
    wait

    mkdir "$resultdir"
    mv "$RESULTDIR/"* "$resultdir/."

    ssh_check_for_errors
}

function renew_keys() {
    (( $# == 0 )) || fatal "renew_keys: got args not takes none"

    PUBKEY_FILE="ssh_keys.pub"

    [ -f "$PUBKEY_FILE" ] || fatal "renew_keys did not find file: $PUBKEY_FILE"

    local node index=0
    for node in "${nodes[@]}"
    do
	do_ssh "$index" "$node" "cat > .ssh/authorized_keys" < "$PUBKEY_FILE" &
	index=$((index + 1))
    done
    wait

    ssh_check_for_errors
}

function scan_keys() {
    (( $# == 0 )) || fatal "scan_keys: got args not takes none"

    local node index=0
    for node in "${nodes[@]}"
    do
	ssh-keyscan "$node"
    done
}

function check_nodes() {
    (( $# == 0 )) || fatal "check_nodes: got args not takes none"

    local node index=0
    for node in "${nodes[@]}"
    do
	do_ssh "$index" "$node" "uname -a" >/dev/null &
	index=$((index + 1))
    done
    wait
 
    ssh_show_results
}

function usage() {
cat >&2 <<EOF
${0##*/}: usage: ${0##*/} command
  where command can be:
    run <dir> <binary> <arg-list-file> <result-dir> .. run binary in dir 
          with args found in arg-list-file, nodes will be used in paralell
    scan-keys .. scans for ssh host keys and displays them to stdout
    renew-keys .. will renew all ssh keys from ssh_keys.pub
    check-nodes .. check if nodes are up (output of uname -a is shown)
EOF
    bb_traps_disable
    exit 1
}

function main() {
    (( $# > 0 )) || usage

    SSH_USER=compute

    local cmd="$1"
    shift

    [ -f NODES ] || fatal "no such file: NODES"
    declare -a nodes
    readarray -t nodes < "NODES"

    TMPDIR=$(mktemp -d /tmp/compute.XXXXXXXXXX) || fatal "mktemp failed"

    case "$cmd" in
	run) run "$@";;
	scan-keys) scan_keys "$@";;
	renew-keys) renew_keys "$@";;
	check-nodes) check_nodes "$@";;
	*) usage
    esac

    rm -rf "$TMPDIR" || fatal "failed to remove TMPDIR: $TMPDIR"
}

main "$@"

bb_quit
