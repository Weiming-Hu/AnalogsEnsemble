# Generate document using pkgdown

library(pkgdown)

unlink("docs", recursive = T)

# Sometimes the internet connection can cause problems.
# So I disabled it.
#
options(pkgdown.internet = FALSE)

build_site()
