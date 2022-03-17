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
build_site(overrider = list(repo = list(url = list(
  home = 'https://weiming-hu.github.io/AnalogsEnsemble/',
  source = 'https://github.com/Weiming-Hu/AnalogsEnsemble/tree/master/RAnalogs/RAnEn/',
  issue = 'https://github.com/Weiming-Hu/AnalogsEnsemble/issues/',
  user = 'https://github.com/Weiming-Hu/'))))
