# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University

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
                                       
------------------------------------------- 
-------------------------------------------
RAnEn Version", version, "
Geoinformatics and Earth Observation Laboratory
The Pennsylvania State University")
	} else {
		message <- paste("Package 'RAnEn' version", version)
	}
	
	message <- paste0(message, "\nPlease consider citing RAnEn: citation('RAnEn')")
	
	if (!checkOpenMP()) {
		message <- paste(message, "\n*** Multi-thread not supported ***", sep = '')
	}
	
	packageStartupMessage(message)
	invisible()
}

