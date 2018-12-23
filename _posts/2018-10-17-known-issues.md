---
layout: post
title: Known Issues
tags:
  - issue
---

#### `NetBeans` does not recognize the executables after building the C++ programs.

With Linux, by default, the GCC compiler will compile executable shared libraries that act like executables to a user, but `NetBeans` will not recognize this type. To resolve this issue with `NetBeans`,  add `-no-pie` to the `CMAKE_CXX_FLAGS` or/and `CMAKE_C_FLAGS` depending on which compiler is used.

Some references include [Q&A 1](https://www.linuxquestions.org/questions/arch-29/netbeans-will-not-recognize-build-as-executable-4175620457/); [Q&A 2](https://askubuntu.com/questions/911538/disable-pie-and-pic-defaults-in-gcc-on-ubuntu-17-04); [GCC Options for linking](http://gcc.gnu.org/onlinedocs/gcc/Link-Options.html).

#### `RAnEn` is built with GNU compiler and CMake process. But it shows up not supporting `OpenMP`.

If you have run the `roxygen` command before the make process, the library will not be supporting `OpenMP`. Make sure that before your CMake's runtime, you have a `clean` directory under `RAnalogs/RAnEn/src`. An easy way to do this is to run `make clean-roxygen`, and repeat the build and make process.

#### Linking errors when building the C++ library during the second time.

If you have successfully built the C++ library before and when you modifed any files or did a `git pull`, you found the `make` process failed. This can be caused by using the old dependencies previously built and stored in the `dependency/` directory. This typically happens when you've also changed the compiler. An easy workaround for this is to remove all the build files, include the directories `dependency\`, `output\`, and `build\`, and start from scratch with a clean directory.

#### How do I make sure whether `OpenMP` is supported.

If you are using the `RAnEn` library, you can check the automatic message printed on loading the library. If it does not say "OpenMP is not supported", then it is supported.

If you are using the C++ programs, you need to first build tests by specifying `-DCMAKE_BUILD_TESTS=ON` to `cmake`. Then after the program compiles, there is a test at `AnalogsEnsemble/output/test/runAnEn`. If you see `OpenMP is supported` and the number of threads in the standard output message after running the test executable, `OpenMP` is correctly configured. You can also try to modify the system environment variable `OMP_NUM_THREADS` and run the test executable again, see how the system environment variable affects the number of threads being created.

#### Compilation failure on Windows

Please see the [article](https://weiming-hu.github.io/AnalogsEnsemble/2018/10/10/installation-on-windows.html).

#### Dynamic linking error on MacOS.

Sometimes when I run the test for `AnEnIO` and I will see the following error.

```
testAnEnIO::testReadObservationFile
dyld: lazy symbol binding failed: Symbol not found: __ZN6netCDF6NcFileC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEENS0_8FileModeE
  Referenced from: /Users/wuh20/github/AnalogsEnsemble/output/lib/libAnEnIO.dylib
  Expected in: /usr/local/lib/libnetcdf-cxx4.1.dylib

dyld: Symbol not found: __ZN6netCDF6NcFileC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEENS0_8FileModeE
  Referenced from: /Users/wuh20/github/AnalogsEnsemble/output/lib/libAnEnIO.dylib
  Expected in: /usr/local/lib/libnetcdf-cxx4.1.dylib

Abort trap: 6
```

I can normally get away from this by specifying `-DCMAKE_MACOSX_RPATH=ON` for `cmake` and then recompile everything.

