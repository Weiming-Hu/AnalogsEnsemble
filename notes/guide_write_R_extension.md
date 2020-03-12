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
# set working directory at 'RAnEn/'
Rcpp::compileAttributes()
devtools::load_all()
devtools::document()
```
