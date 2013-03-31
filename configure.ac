AC_INIT([CamelRace], [0.1], [javier.angulo1@gmail.com])

AC_PREFIX_DEFAULT([/usr/local])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CXX

LT_INIT

PKG_CHECK_MODULES(OGF, [OGF])

AC_OUTPUT([
	Makefile
	code/Makefile
])
