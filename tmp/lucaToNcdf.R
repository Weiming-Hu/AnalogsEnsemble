library(RAnEn)

load('~/geolab_storage_V3/data/Analogs/LucaWindData.Rdata')

obs.wspd <- obs.wspd[,1:8,]
obs.wspd <- matrix(obs.wspd, nrow=nrow(obs.wspd))
dim(obs.wspd) <- c(1, dim(obs.wspd))
obs.times <- seq(0, by=3, length=dim(obs.wspd)[3])

station = 211
obs.wspd = obs.wspd[,station,,drop=F]

observations <- list(Data = obs.wspd, ParameterNames = "wspd",
                     Xs = mod.slong[station,1], Ys = mod.slat[station,1], Times = obs.times)
writeNetCDF("Observations", observations, "~/Desktop/observations_211.nc",
            global.attrs = list(unit = "GEOlab", website = "geolab.psu.edu/"))

data <- array(NA, dim=c(4, dim(mod.PRES)))
data[1,,,] <- mod.PRES
data[2,,,] <- mod.TMP
data[3,,,] <- mod.wspd
data[4,,,] <- mod.wdir

data <- aperm(data, c(1,2,4,3))
data.times <-seq(0,by=24,length=dim(data)[3])
data.flt  <- seq(0,by=3, length=dim(data)[4])

data=data[,station,,,drop=F]

forecasts <- list(
  Data = data, ParameterNames =c("PRES","TMP", "wspd", "wdir"),
  ParameterCirculars = "wdir",
  Xs = mod.slong[station,1], Ys = mod.slat[station,1], Times = data.times, 
  FLTs = data.flt)

writeNetCDF("Forecasts", forecasts, "~/Desktop/forecasts_211.nc",
            global.attrs = list(unit = "GEOlab", website = "geolab.psu.edu/"))

