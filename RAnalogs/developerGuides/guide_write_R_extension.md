# Guide for Writing An R Extension Note

### When function signatures have been changed
We need to change the interfaces between R and C++ using
`Rcpp::compileAttributes()` function.

```
library(Rcpp)
Rcpp::compileAttributes(pkgdir = "[library folder]")
```


### When Roxygen comments have been changed
We need to regenerate our documents, and possibly the NAMESPACE file.

Before generating any documents, make sure you have already complete
the `RAnEn` package folder. Be sure to run `cmake` and `make` process
in advance.

Set the working directory to the R library root folder. And then
regenerate the documents using `devtools::document()` function.
This will generate both the MAN documents and the NAMESPACE file.

### All Together

```
# set working directory at 'RAnalogs/'
Rcpp::compileAttributes(pkgdir = "RAnEn")
setwd('RAnEn/')
devtools::document()
```

If you see errors like the following:

```
 Error in .Call("_RAnEn_checkOpenMP", PACKAGE = "RAnEn") : 
  "_RAnEn_checkOpenMP" not available for .Call() for package "RAnEn" 
```

This is usually caused by the missing entries in `NAMESPACE` file.

Try to call `load_all()` within the `RAnEn` folder and rerun `document()`.
