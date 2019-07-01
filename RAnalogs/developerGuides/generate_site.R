# Generate document using pkgdown

library(pkgdown)
library(devtools)

# unlink("docs", recursive = T)

# Sometimes the internet connection can cause problems.
# So I disabled it.
#
# options(pkgdown.internet = FALSE)
# options(repos='http://cran.rstudio.com/')

load_all()
build_site()
