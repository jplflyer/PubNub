# Introduction
This repository is Joe's attempt to get a single library to compile against. Most of the work will be in a Makefile in the cpp directory.

I have:

  - Moved this directory's README.md to README.orig.md.
  - Created cpp/Makefile.
  - Created samples and copied some files from the cpp directory.

See samples/Makefile to see how to get a successful build of your own programs.

# Directions To Build The Library
To produce /usr/local/lib/libpubnub.a and /usr/local/include/pubnub:

    cd cpp
    make
    make install  # May need to sudo

All source is compiled with g++, which means the library is NOT compatible with linking against programs compiled with gcc. You could change the gcc/Makefile to compile .c with gcc, and then add -D PUBNUB_USE_EXTERN_C when compiling with g++. If you do that, then you also need to use that flag in your own Makefiles. As I am not going to do any of this from C and only from C++, I didn't find that useful.
