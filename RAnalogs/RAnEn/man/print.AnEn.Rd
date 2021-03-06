% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/print.AnEn.R
\name{print.AnEn}
\alias{print.AnEn}
\title{RAnEn::print.AnEn}
\usage{
\method{print}{AnEn}(x, recursive = F)
}
\arguments{
\item{x}{an AnEn object}

\item{recursive}{Whether to print all variables from nest-groups}
}
\description{
Overload print function for class AnEn
}
\examples{
AnEn <- list()
class(AnEn) <- c('AnEn', class(AnEn))
print(AnEn)

}
\author{
Weiming Hu \email{weiming@psu.edu}
}
