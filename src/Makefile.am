AM_CFLAGS = \
    $(VARNISHAPI_CFLAGS) \
    $(ZSTD_CFLAGS) \
    -Wall \
    -Werror

AM_LDFLAGS = \
    $(VMOD_LDFLAGS) \
    $(VARNISHAPI_LIBS) \
    $(ZSTD_LIBS) \
    -ldl \
    -lz

vmod_LTLIBRARIES = libvmod_zstd.la

libvmod_zstd_la_SOURCES = \
	vmod_zstd.c

nodist_libvmod_zstd_la_SOURCES = \
	vcc_zstd_if.c \
	vcc_zstd_if.h

vmod_vcs_version.txt: $(nodist_libvmod_zstd_la_SOURCES)

DISTCLEANFILES = vmod_vcs_version.txt

@BUILD_VMOD_ZSTD@

# Test suite

AM_TESTS_ENVIRONMENT = \
	PATH="$(abs_builddir):$(VARNISH_TEST_PATH):$(PATH)" \
	LD_LIBRARY_PATH="$(VARNISH_LIBRARY_PATH):$(MODSECURITY_LIBRARY_PATH)"
TEST_EXTENSIONS = .vtc
VTC_LOG_COMPILER = varnishtest -lv
AM_VTC_LOG_FLAGS = \
	-p vcl_path="$(abs_top_srcdir)/vcl:$(VARNISHAPI_VCLDIR)" \
	-p vmod_path="$(abs_builddir)/.libs:$(vmoddir):$(VARNISHAPI_VMODDIR)"

TESTS = \
    vtc/test_version.vtc

# Documentation

dist_doc_DATA = \
	vmod_zstd.vcc \
	vmod_vcs_version.txt \
	$(TESTS)

dist_man_MANS = \
	vmod_zstd.3

.rst.1:
	$(AM_V_GEN) $(RST2MAN) $< $@
