AUTOMAKE_OPTIONS=foreign
DISTCLEANFILES = Makefile.in

desktopdir=/usr/share/applications/
desktop_PROGRAMS=jnSSH2-launcher.desktop

bin_PROGRAMS = jnSSH2-launcher

jnSSH2-launcher: jnSSH2-launcher.py
	cp $^ $@
	chmod 755 $@

jnSSH2-launcher.desktop: jnSSH2-launcher.desktop.in
	cp $^ $@
