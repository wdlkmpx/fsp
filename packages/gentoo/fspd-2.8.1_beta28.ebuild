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
		prefix=${D}
		without-clients=yes
		mandir=${D}/usr/share/man
		docdir=${D}/usr/share/doc/${PF}
		sysconfdir=/etc
	)
}

src_compile() {
	escons "${MYSCONS[@]}"
}

src_install() {
	escons "${MYSCONS[@]}" install
	rm -f ${D}/bin/fspscan ${D}/usr/share/man/man1/fspscan*
	mkdir -p ${D}/etc
	mv ${D}/share/examples/fsp/fspd.conf ${D}/etc
	rm -rf ${D}/share
}
