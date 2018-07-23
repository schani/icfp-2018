check:
	shellcheck *.sh

clean:
	rm -f *~
	for dir in computing src Bertl; do make -C $$dir clean; done
