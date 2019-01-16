# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <wuh20@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#
# This script simply calls all the test in the current folder

files <- list.files(path = '.', pattern = "test_(C|R).*\\.R", full.names = T)

for (file in files) {
  cat(paste("Executing", file, "\n"))
  source(file, echo = F)
}

cat("You have survived all the tests!\n")
