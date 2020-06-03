SUMMARY = "Tools for UInput"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# Use the latest version at 26 Oct, 2013
#SRCREV = "19ff218870311206c8fa47a3fcbe174d72373c8c"
SRC_URI = "git://github.com/harryausten/ydotool"

SRCREV = "${AUTOREV}"
PV = "1.0+git${SRCPV}"

S = "${WORKDIR}/git"

EXTRA_OEMAKE = "'CC=${CC}' 'RANLIB=${RANLIB}' 'AR=${AR}' 'CFLAGS=${CFLAGS} -I${S}/include -DWITHOUT_XATTR' 'BUILDDIR=${S}'"

# this command will be run to compile your source code. it assumes you are in the
# directory indicated by S. i'm just going to use make and rely on my Makefile

INSANE_SKIP_${PN} = "ldflags"

do_compile () {
	make clean
	make
}

# this will copy the compiled file and place it in ${bindir}, which is /usr/bin
do_install () {
	install -d ${D}${bindir}
	install -m 0755 ydotool ${D}${bindir}
	install -m 0755 ydotoold ${D}${bindir}
	install -m 0755 input ${D}${bindir}
	install -m 0755 screencap ${D}${bindir}
	install -m 0755 adb_init ${D}${bindir}
	install -m 0755 uinput_init ${D}${bindir}
}
