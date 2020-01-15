weiming <- read.table('~/Desktop/debug_few.txt', sep = ',', header = F)
guido <- read.table('~/tmp/CAnalogs/CAnalogs/debug_log_few.txt', sep = ',', header = F)


data = matrix(nrow=nrow(guido), ncol = 16)
counter  = 1

for ( parameter in min(guido[,2]):max(guido[,2])) {
  for ( train in min(guido[,4]):max(guido[,4])) {
    for ( test in min(guido[,6]):max(guido[,6])) {
      for ( flt in min(guido[,8]):max(guido[,8])) {

        valid.g <- which( guido[,2]==parameter & guido[,4] == train & guido[,6] == test & guido[,8] == flt)
        #print(guido[valid.g,])
        
        valid.w <-which( weiming[,2]==parameter & weiming[,4] == train & weiming[,6] == test & weiming[,8] == flt)
        
        data[counter,1:4]  <- c(parameter,train,test,flt)
        data[counter,c(5,7,9,11,13,15)] <- as.numeric(as.vector(guido[valid.g,c(10,12,14,16,18,20)]))
        data[counter,c(6,8,10,12,14,16)] <- as.numeric(as.vector(weiming[valid.w,c(10,12,14,16,18,20)]))

        
        counter = counter+1
      }
    }
  }
}

i = which(data[,15] != data[,16])


range(abs(data[i,15] - data[i,16]))

data[i, ]
