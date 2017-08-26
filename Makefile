
build:
	# build test file here.

install:
	[ ! -d /usr/include/rlib ] || rm -rf /usr/include/rlib
	cp -r . /usr/include/rlib
	rm -rf /usr/include/rlib/test

