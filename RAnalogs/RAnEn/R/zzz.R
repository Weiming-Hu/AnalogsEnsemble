.onLoad <- function(lib, pkg) {
  version <- read.dcf(file.path(lib, pkg, "DESCRIPTION"), "Version")

if (interactive()) {
    message <- paste(
"-------------------------------------------
-------------------------------------------
      ____   ___            ______     
     / __ \\ /   |   ____   / ____/____ 
    / /_/ // /| |  / __ \\ / __/  / __ \\
   / _, _// ___ | / / / // /___ / / / /
  /_/ |_|/_/  |_|/_/ /_//_____//_/ /_/ 
                                       
------------------------------------------- GEOlab @ Penn State
------------------------------------------- RAnEn Version", version)
    } else {
        message <- paste("Package 'RAnEn' version", version)
    }
    if (!check_OpenMP()) {
        message <- paste(message, "\n*** Multi-thread not supported ***",
                         sep = '')
    }

    packageStartupMessage(message)
    invisible()
}

