bin_PROGRAMS = lightjbl

lib_LTLIBRARIES = liblightjbl.la

AM_CFLAGS = -I$(top_srcdir)/libev

liblightjbl_la_SOURCES = \
	log.h \
	bignum.c \
	bignum.h \
	bn_mul.h \
	md5.c \
	md5.h \
	request.c \
	request.h \
	rsa.c \
	rsa.h \
	sds.c \
	sds.h \
	sdsalloc.h \
	signer.c \
	signer.h \
	socket.c \
	socket.h

if WIN32
liblightjbl_la_SOURCES += win32.c win32.h
endif

liblightjbl_la_LIBADD = $(top_builddir)/libev/libev.la

lightjbl_SOURCES = main.c

lightjbl_LDADD = liblightjbl.la
