# Generate document size using pkgdown

library(pkgdown)
unlink("docs", recursive = T)
build_site()
