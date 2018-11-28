# Inspect Shared Libraries

### MacOS

```
otool -L <file>
```

```
otool -l <file>
```

```
otool -D <file>
```

### Linux

Display the contents of the ELF file's dynamic section. This command is used to print out the dependent library runpath. In stead of printing everything, we direct the output and only take the first several lines which contains the library runpath.

```
readelf -d <file> | head -10
```

### No rule to make for a dynamic library

*OS: `Ubuntu 18.04.1 LTS`*


If the following error message occurs during the build time for building `gribConverter`:

```
make[2]: *** No rule to make target '/usr/lib/x86_64-linux-gnu/libopenjp2.so', needed by '../output/bin/gribConverter'.  Stop.
CMakeFiles/Makefile2:673: recipe for target 'apps/app_gribConverter/CMakeFiles/gribConverter.dir/all' failed
make[1]: *** [apps/app_gribConverter/CMakeFiles/gribConverter.dir/all] Error 2
Makefile:140: recipe for target 'all' failed
make: *** [all] Error 2
```

This is probably because you don't have the dynamic library at the expected location. This can happen when you removed libraries that you should not.

First, we need to make sure the file is missing at the expected location.

```
$ stat /usr/lib/x86_64-linux-gnu/libopenjp2.so
stat: cannot stat '/usr/lib/x86_64-linux-gnu/libopenjp2.so': No such file or directory
```

The file does not exist. So we need to find out which package to install in order to have this library. We do this with the help of `apt-file`.

```
$ sudo apt-get install apt-file
$ apt-file update
$ sudo apt-file search /usr/lib/x86_64-linux-gnu/libopenjp2.so
libopenjp2-7: /usr/lib/x86_64-linux-gnu/libopenjp2.so.2.3.0
libopenjp2-7: /usr/lib/x86_64-linux-gnu/libopenjp2.so.7
libopenjp2-7-dev: /usr/lib/x86_64-linux-gnu/libopenjp2.so
```

Finally, install one of the libraries. I chose the develop version.

```
$ sudo apt-get install libopenjp2-7-dev
```

The error should be fixed.
