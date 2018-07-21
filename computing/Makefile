check:
	shellcheck *.sh

demo:
	./compute.sh run demo ./add.sh demo/120args result

key-add:
	echo THIS WILL ADD ALL HOST KEYS TO YOUR ~/.ssh/known_hosts FILE
	echo press ENTER to continue, ctl-c to abort
	read
	./compute.sh scan-keys >> ~/.ssh/known_hosts 2>/dev/null

clean:
	rm -rf *~ build result

.PHONY: clean demo key-add
