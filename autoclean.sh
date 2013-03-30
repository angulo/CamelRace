if test -f Makefile; then
	make distclean
fi

rm -rf \
	autom4te.cache \
	config.guess \
	config.sub \
	configure \
	depcomp \
	install-sh \
	missing \
	Makefile.in \
	aclocal.m4 \
	config.h.in \
	ltmain.sh \
	src/Makefile.in
