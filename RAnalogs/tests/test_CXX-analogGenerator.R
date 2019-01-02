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

# This script is used to test C++ executable Analog Generator.

old.wd <- getwd()
setwd('../../apps/app_analogGenerator/example/')

scripts <- c(
  './compare_methods.sh',
  './compare_sds_results.sh'
)

ret <- system(scripts)
  
setwd(old.wd)

if (ret == 0) {
  print("You survived the tests for analogGenerator!")
} else {
  stop("Error: Something is wrong for analogGenerator tests.")
}
