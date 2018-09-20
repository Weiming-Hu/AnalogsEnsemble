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
