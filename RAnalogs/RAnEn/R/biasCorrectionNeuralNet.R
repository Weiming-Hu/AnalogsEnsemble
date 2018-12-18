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

#' RAnEn::biasCorrectionNeuralNet
#' 
#' Use a neural network to correct AnEn bias.
#' 
#' First, a group of search day samples are selected, either systematically or
#' randomly.
#' 
#' Then, analogs are generated for these sample days at all available locations
#' and FLTs using leave-one-out method. Leave-one-out method remoev only the sample
#' day from the search data set and use the rest of the search data as search.
#' 
#' After analogs are generated for samples, analog means are calculated and their
#' individual bias can be calculated as well.
#' 
#' Then, a neural network is trained potentially using the following variables:
#' - all variables from the model;
#' - locations if provided or loaction indices
#' - FLT indices
#' The output of the neural network will be bias. Which variables to use depends on
#' the correlation between variables and bias. the threshold can be set using
#' cor.threshold.
#' 
#' Finally, the bias for tests are calculated using the neural network and AnEn 
#' results are corrected.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param AnEn An AnEn object.
#' @param config The configuration object used to generate the AnEn results.
#' @param sample.size The number of samples to take from search days.
#' @param sample.method The sample method. It can be one of 'systematic' and 'random'.
#' @param cor.threshold The correlation threshold to determine whether a variable will
#' be used.
#' @param maxit The maximum number of iteration when training the neural network.
#' @param nnet.size The number of nodes in the hidden layer of the neural network.
#' @param group.func A function that summarizes all the member values
#' into one value. For example, it can be 'mean', 'median', 'max', or min'.
#' @param ... Extra parameters to function group.func.
#' @param show.progress A logical for whether to show the progress bar
#' @param keep.bias A logical for whether to keep the bias array
#' @param keep.model A logical for whether to keep the neural network and data
#' @param overwrite A logical for whether to overwrite correction results.
#' @param parallel A logical for whether to use parallelization.
#' @param num.cores The number of cores to use for parallelization.
#' @return An AnEn object.
#' 
#' @importFrom pbapply pbsapply
#' @importFrom parallel detectCores
#' @importFrom nnet nnet
#' @useDynLib RAnEn
#' 
#' @export
biasCorrectionNeuralNet <- function(
  AnEn, config, sample.size = 20, sample.method = 'systematic', cor.threshold = 0.1,
  maxit = 2000, nnet.size = NA, group.func, ..., show.progress = T, keep.bias = F,
  keep.model = F, overwrite = F, parallel = F, num.cores = stop("Please specify num.cores!")) {
  
  require(pbapply)
  if (!show.progress) pbo <- pboptions(type = "none")
  
  # Check for overwriting
  if ('analogs.cor' %in% names(AnEn) ||
      'bias' %in% names(AnEn) ||
      'bias.model' %in% names(AnEn)) {
    if (!overwrite) {
      stop('Corrected AnEn already exists. Use overwrite = T to orverwrite them.')
    }
  }
  
  # Disable all the meta data for faster process
  config$preserve_mapping <- F
  config$preserve_similarity <- F
  config$preserve_search_stations <- F
  config$verbose <- 1
  
  # Preserve real time because this makes the query for observation easier
  config$preserve_real_time <- T
  
  if (dim(AnEn$analogs)[1] == 1) {
    stop("The current implementation does not work for 1 grid point.")
  }
  
  # Sample from the search forecasts
  if (show.progress) cat("Sample days from search forecasts ...\n")
  
  if (sample.method == 'systematic') {
    test.days <- unique(floor(seq(1, dim(config$search_forecasts)[3], length.out = sample.size)))
  } else if (sample.method == 'random') {
    test.days <- sample.int(dim(config$search_forecasts)[3], n = sample.size)
  } else {
    stop(paste("Unknown sample method:", sample.method))
  }
  
  if (show.progress) cat("Sampled day(s): ", test.days, '\n')
  
  # Run some checks
  if (length(config$search_times) <= 1) {
    stop("Too few search times!")
  }
  if (dim(config$search_forecasts)[3] <= 1) {
    stop("Too few search times!")
  }
  
  # Generate AnEn for sampled days and compute bias for each AnEn
  if (show.progress) cat("Compute Bias for sampled days using the left-one-out method ...\n")
  
  if (parallel) {
    require(parallel)
    cl <- makeCluster(num.cores)
  } else {
    cl <- NULL
  }
  
  bias <- pbsapply(simplify = "array", cl = cl, X = test.days,
                   FUN = function(test.day, config, group.func, ...) {
    require(RAnEn)
    
    # Only include the one day in test forecasts
    config$test_forecasts <- config$search_forecasts[, , test.day, , drop = F]
    
    # Compute how many days are included in FLTs
    covered.days <- ceiling(max(config$search_flts) / 3600 / 24)
    
    # Remove the one day from the search forecasts and following several days 
    # if FLTs cover multiple days
    #
    test.time <- config$search_times[test.day]
    config$search_times <- config$search_times[-(test.day:(test.day + covered.days - 1))]
    config$search_forecasts <- config$search_forecasts[
      , , -(test.day:(test.day + covered.days - 1)), , drop = F]
    
    # Generate AnEn
    AnEn <- RAnEn::generateAnalogs(config)
    
    # Compute mean across ensemble values
    analogs.mean <- apply(AnEn$analogs[, 1, , , 1, drop = F], c(1, 2, 3, 5), group.func, ...)
    dim(analogs.mean) <- dim(analogs.mean)[c(1, 3)]
    
    # Get the indices for test times and flts in observation times
    indices <- lapply(test.time + config$search_flts, function(t, times) {
      return(which(times == t))
    }, times = config$observation_times)
    indices <- as.numeric(indices)
    
    if (all(is.na(indices))) {
      stop("No combinations of Time and FLT are found in observations.")
    }
    
    # Query observations
    observations <- config$search_observations[config$observation_id, , indices, drop = F]
    dim(observations) <- dim(observations)[-1]
    
    # Compute the bias (observation - mean of AnEn) between the AnEn and the observations
    # for each test location and each FLT
    #
    bias <- observations - analogs.mean
    
    return(bias)
    
  }, config = config, group.func = group.func)
  
  if (parallel) {
    stopCluster(cl)
  }
  
  # Make sure bias is of class array
  stopifnot(class(bias) == 'array')
  
  # Prepare training data for ANN
  if (show.progress) cat("Prepare training data for building the neural network ...\n")
  data.nn.train <- data.frame(
    bias = as.vector(aperm(bias, perm = c(1, 3, 2))),
    FLT = rep(1:dim(config$search_forecasts)[4],
              each = length(test.days) * dim(config$search_forecasts)[2])
  )
  
  for (i in 1:dim(config$search_forecasts)[1]) {
    rand.var <- as.vector(config$search_forecasts[i, , test.days, ])
    if (length(rand.var) != nrow(data.nn.train)) {
      stop("Different numbers of weather variable values found.")
    }
    data.nn.train <- cbind(data.nn.train, rand.var)
  }
  colnames(data.nn.train)[3:ncol(data.nn.train)] <- 
    paste('parameter', 1:dim(config$search_forecasts)[1], sep = '')
  
  # Add location variable by using the real xs/ys if they exist or by using 
  # the indices of locations
  #
  if ('search_stations_x' %in% names(config) && 'search_stations_y' %in% names(config)) {
    x <- rep(config$search_stations_x,
             times = dim(config$search_forecasts)[4] * length(test.days))
    y <- rep(config$search_stations_y,
             times = dim(config$search_forecasts)[4] * length(test.days))
    data.nn.train <- cbind(data.nn.train, x, y)
  } else {
    location <- rep(1:dim(config$search_forecasts)[2],
                    times = dim(config$search_forecasts)[4] * length(test.days))
    data.nn.train <- cbind(data.nn.train, location)
  }
  
  # Normalize variables using column maximum values
  col.max <- apply(data.nn.train, 2, max, na.rm = T)
  data.nn.train.norm <- sweep(data.nn.train, 2, col.max, `/`)

  # Remove rows with NA bias
  na.indices <- which(is.na(data.nn.train.norm$bias))
  if (length(na.indices) != 0) data.nn.train.norm <- data.nn.train.norm[-na.indices, ]
  
  # Define model formula based on correlation
  cor.bias <- cor(data.nn.train.norm)[1, ]
  selected <- which(cor.bias < 1 & cor.bias > cor.threshold)
  if (length(selected) == 0) stop("No parameters are correlated enough with bias.")
  formula <- as.formula(paste("bias ~", paste(colnames(data.nn.train)[selected], collapse = "+")))
  
  # Train an ANN for bias correction
  require(nnet)
  if (show.progress) cat("Train the neural network ...\n")
  if (is.na(nnet.size)) nnet.size <- ncol(data.nn.train) - 1
  model.nn <- nnet(formula = formula, data = data.nn.train.norm,
                   size = nnet.size, decay = 5e-4, maxit = maxit, linout = T)
  
  # Generate the bias for test forecasts
  if (show.progress) cat("Prepare test data for running the neural network ...\n")
  data.nn.test <- data.frame(
    FLT = rep(1:dim(config$test_forecasts)[4],
              each = prod(dim(config$test_forecasts)[2:3]))
  )
  
  for (i in 1:dim(config$test_forecasts)[1]) {
    rand.var <- as.vector(config$test_forecasts[i, , , ])
    if (length(rand.var) != nrow(data.nn.test)) {
      stop("Different numbers of weather variable values found.")
    }
    data.nn.test <- cbind(data.nn.test, rand.var)
  }
  colnames(data.nn.test)[2:ncol(data.nn.test)] <- 
    paste('parameter', 1:dim(config$test_forecasts)[1], sep = '')
  
  # Add location variable by using the real xs/ys if they exist or by using 
  # the indices of locations
  #
  if ('location' %in% colnames(data.nn.train)) {
    location <- rep(1:dim(config$test_forecasts)[2],
                    times = prod(dim(config$test_forecasts)[3:4]))
    data.nn.test <- cbind(data.nn.test, location)
  } else {
    x <- rep(config$test_stations_x, times = prod(dim(config$test_forecasts)[3:4]))
    y <- rep(config$search_stations_y, times = prod(dim(config$search_forecasts)[3:4]))
    data.nn.test <- cbind(data.nn.test, x, y)
  }
  
  if (show.progress) cat("Estimate bias ...\n")
  data.nn.test.norm <- sweep(data.nn.test, 2, col.max[-which(names(col.max) == 'bias')], `/`)
  bias.pred <- predict(model.nn, newdata = data.nn.test.norm) * col.max['bias']
  
  # A little trick to generate a duplicated bias arary that has the same form as
  # analogs so that it is easier to sum them up, instead of using a slower for-loop.
  # This trick requires more memory though.
  #
  bias.pred <- aperm(array(rep(bias.pred, each = dim(AnEn$analogs)[4]),
                           dim = dim(AnEn$analogs)[4:1]), 4:1)
  dim(bias.pred) <- c(dim(bias.pred), 1)
  
  if (show.progress) cat("Apply bias to analogs ...\n")
  AnEn$analogs.cor <- AnEn$analogs
  AnEn$analogs.cor[, , , , 1] <- AnEn$analogs.cor[, , , , 1, drop = F] + bias.pred
  
  if (keep.bias) {
    # Return the single bias for each station, each time, and each FLT
    AnEn$bias <- bias.pred[, , , 1, 1, drop = F]
    dim(AnEn$bias) <- dim(AnEn$bias)[1:3]
  }
  
  if (keep.model) {
    AnEn$bias.model <- list(model = model.nn,
                            train = data.nn.train.norm,
                            test = data.nn.test.norm,
                            normalize.factor = col.max)
  }
  
  if (!show.progress) on.exit(pboptions(pbo))  
  return(AnEn)
}
