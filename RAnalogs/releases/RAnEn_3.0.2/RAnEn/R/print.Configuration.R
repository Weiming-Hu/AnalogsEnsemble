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
#
print.Configuration <- function(x) {
	
	if (class(x) != 'Configuration') stop('Not a Configuration object.')
	
	valid <- T
	for (i in 1:length(x)) if (is.null(x[[i]])) valid <- F
	
	cat('Class: Configuration list\n')
	cat('Mode: ', x$mode, '\n')
	cat('Variables: ')
	cat(paste(attr(x, 'names'), collapse = ', '))
	cat('\n')
	
	if (!valid) {
		cat('* Not ready to be used. NULL exists in variables. *\nNull variables: ')
		null.variables <- c()
		for (i in 1:length(x)) if (is.null(x[[i]])) null.variables <- c(null.variables, names(x)[i])
		cat(paste(null.variables, collapse = ', '))
	}
}