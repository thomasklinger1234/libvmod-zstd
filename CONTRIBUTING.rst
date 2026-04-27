CONTRIBUTING
============


For developers
--------------

The VMOD source code is written in C, and compilation has been tested with gcc
and clang. The code MUST always compile successfully with both of
them.

The build specifies C99 conformance for C sources (``-std=c99``). All
compiler warnings are turned on, and all warnings are considered
errors (``-Werror -Wall -Wextra``).  The code MUST always build
without warnings or errors under these constraints.

By default, ``CFLAGS`` is set to ``-g -O2``, so that symbols are
included in the shared library, and optimization is at level
``O2``. To change or disable these options, set ``CFLAGS`` explicitly
before calling ``bootstrap`` (it may be set to the empty string).

For development/debugging cycles, the ``bootstrap`` option
``--enable-debugging`` is recommended (off by default). This will turn
off optimizations and function inlining, so that a debugger will step
through the code as expected.

By default, the VMOD is built with the stack protector enabled
(compile option ``-fstack-protector``), but it can be disabled with
the ``bootstrap`` option ``--disable-stack-protector``.
