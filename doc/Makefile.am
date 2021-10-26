## Process this file with automake to produce Makefile.in
EXTRA_DIST=faq.html PROTOCOL HISTORY fsp-faq.sgml fsp-faq.html

noinst_DATA = fsp-faq.html

SUFFIXES = .html .sgml
.sgml.html:
	sgmlfmt -d docbook -f html $<
#	jade -t sgml -d /usr/local/share/sgml/docbook/dsssl/modular/html/docbook.dsl  $<
