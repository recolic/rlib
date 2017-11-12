
def:
	echo "make install/uninstall."
	exit 1

install:
	[ ! -d /usr/include/rlib ] || rm -rf /usr/include/rlib
	cp -r . /usr/include/rlib
	rm -rf /usr/include/rlib/test /usr/include/rlib/.git

uninstall:
	rm -rf /usr/include/rlib
