AUTOMAKE_OPTIONS=foreign
DISTCLEANFILES = Makefile.in

CFLAGS="-g"
CFLAGS+=$(shell pkg-config --cflags gtk+-2.0)
CFLAGS+=$(shell pkg-config --cflags libglade-2.0)

LDFLAGS+=$(shell pkg-config --libs gtk+-2.0)
LDFLAGS+=$(shell pkg-config --libs libglade-2.0)
LDFLAGS+=$(shell pkg-config --libs  gmodule-export-2.0)

bin_PROGRAMS = ui_glade.h jnSSH2-sm

jnSSH2_sm_SOURCES = main.c

ui_glade.h: ui.glade
	$(top_srcdir)/f2h/f2h $^ $@

