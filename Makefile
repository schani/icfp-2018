check:
	shellcheck *.sh

clean:
	rm -f *~
	make -C computing clean
