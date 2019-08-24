# fspd Gentoo ebuild
# Created by 2019 Radim Kolar
# Public domain

EAPI=7

PYTHON_COMPAT=( python2_7 python3_6 )
inherit python-any-r1 multilib scons-utils toolchain-funcs

DESCRIPTION="FSP server"
HOMEPAGE="http://fsp.sourceforge.net/"
SRC_URI="mirror://sourceforge/fsp/fsp/${PV/_beta/b}/fsp-${PV/_beta/b}.tar.bz2"
S=${WORKDIR}/fsp-${PV/_beta/b}

LICENSE="MIT"
SLOT="0"
KEYWORDS="~amd64"

BDEPEND="sys-devel/flex"

src_configure() {
	MYSCONS=(
		CC="$(tc-getCC)"
		prefix=${D}/usr
		without-clients=yes
		without-fspscan=yes
		mandir=${D}/usr/share/man
		docdir=${D}/usr/share/doc/${PF}
		examplesdir=${D}/usr/share/examples/fspd
		sysconfdir=/etc
	)
}

src_compile() {
	escons "${MYSCONS[@]}"
}

src_install() {
	escons "${MYSCONS[@]}" install
	mkdir -p ${D}/etc
	mv ${D}/usr/share/examples/fspd/fspd.conf ${D}/etc
	rm -rf ${D}/usr/share
}
