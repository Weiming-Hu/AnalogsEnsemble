# # https://www.johndcook.com/blog/standard_deviation/
# 
# 
# 
# # My answer
# sdAccum(val = vec.add, M = mean(vec.ori), sd = sd(vec.ori),
#         n = length(vec.ori), circular = F)$sd
# 
# # Correct answer
# sd(c(vec.ori, vec.add))
# 
# # My circular answer
# sdAccum(val = vec.add, M = mean(vec.ori * pi / 180), sd = sd(vec.ori * pi / 180),
#         n = length(vec.ori), circular = T)
# meanCircular(c(vec.ori, vec.add))
# 
# # Correct circular answer
# sdsCircular(c(vec.ori, vec.add))
# 
# sd(vec.ori * pi / 180) * 180 / pi
# sdsCircular(vec.ori)
# 
# 
# m.k.pre <- mean(vec.ori)
# m.k.cur <- m.k.pre + (vec.add - m.k.pre) / (length(vec.ori) + 1)
# (var(vec.ori) * (length(vec.ori) - 1) + (vec.add - m.k.pre) * (vec.add - m.k.cur)) / (length(vec.ori))
# 
# 
# runningSds <- function(vec) {
#   m.pre <- vec[1]
#   m.cur <- 0
#   k <- 1
#   
#   for (i in 2:length(vec)) {
#     k <- k + 1
#     m.cur <- m.pre + (vec[i] - m.pre) / k
#   }
#   
# }
# 
# 
# mean
# var.circular
# 
# 
# 
# # Correct answer
# s <- mean(sin(c(vec.ori, vec.add) * pi / 180))
# c <- mean(cos(c(vec.ori, vec.add) * pi / 180))
# e <- sqrt(1 - (s^2 + c^2))
# asin(e) * (1 + (2/sqrt(3) - 1)* e^3) * 180 / pi
# 
# s <- mean(sin(vec.ori * pi / 180))
# c <- mean(cos(vec.ori * pi / 180))
# m.k.pre <- atan2(c, s)
# 
# s <- mean(sin(c(vec.ori, vec.add) * pi / 180))
# c <- mean(cos(c(vec.ori, vec.add) * pi / 180))
# atan2(c, s)
# 
# 
# m.k.pre + (func(vec.add) - m.k.pre) / (length(vec.ori) + 1)

# sdAccumLinear <- function(val, M, sd, n, circular) {
#   if (circular) {
#     s <- mean(sin(c(M * pi / 180, val * pi / 180)))
#     c <- mean(cos(c(M * pi / 180, val * pi / 180)))
#     M.new <- atan2(c, s) * 180/ pi
#     return(list(M = M.new))
#   } else {
#     M.new <- M + (val - M) / (n + 1)
#     S.new <- sd^2 * (n-1) + (val - M) * (val - M.new)
#     return(list(M = M.new, sd = sqrt(S.new/(n)))) 
#   }
# }
# meanCircular <- function (deg) {
# atan2(sum(sin(deg * pi / 180)), sum(cos(deg * pi / 180))) * 180 / pi
# }
# 
# sdAccumLinear <- function (val, M, sd, n) {
#   M.new <- M + (val - M) / (n + 1)
#   S.new <- sd^2 * (n-1) + (val - M) * (val - M.new)
#   return(list(M = M.new, sd = sqrt(S.new/(n)))) 
# }
# 
# meanAccumCircular <- function (deg, M, n) {
#   
#   rad <- deg * pi / 180
#   M.rad <- M * pi / 180
#   
#   M.sin <- sin(M.rad)
#   M.cos <- cos(M.rad)
#   
#   rad.sin <- sin(rad)
#   rad.cos <- cos(rad)
#   
#   M.sin.new <- M.sin + (rad.sin - M.sin) / (n + 1)
#   M.cos.new <- M.cos + (rad.cos - M.cos) / (n + 1)
#   
#   atan2(M.sin.new, M.cos.new) * 180 / pi
# }

sdsCircular <- function (vec) {
  s <- mean(sin(vec * pi / 180))
  c <- mean(cos(vec * pi / 180))
  e <- sqrt(1 - (s^2 + c^2))
  asin(e) * (1 + (2/sqrt(3) - 1)* e^3) * 180 / pi
}

# Tests
vec.ori <- c(383,886,777,915,793,335,386,492,649,421,362,27,690,59,763,926,540,426,172,736)
vec.add <- 0

sdsCircular(vec.ori)
sdsCircular(c(vec.ori, vec.add))

sdsAccumCircular <- function (add, s, c, n) {
  s.add <- sin(add * pi / 180)
  c.add <- cos(add * pi / 180)
  
  s <- s + (s.add - s) / (n+1)
  c <- c + (c.add - c) / (n+1)
  
  e <- sqrt(1 - (s^2 + c^2))
  asin(e) * (1 + (2/sqrt(3) - 1)* e^3) * 180 / pi
}

sdsAccumCircular(add = vec.add, s = mean(sin(vec.ori * pi / 180)), c = mean(cos(vec.ori * pi / 180)), n = length(vec.ori))

# cat("My linear answer:", sdAccumLinear(val = vec.add, M = mean(vec.ori), sd = sd(vec.ori), n = length(vec.ori))$sd,
#     "\nCorrect linear answer:", sd(c(vec.ori, vec.add)))
# 
# sdsCircular(vec.ori)
# sdsCircular(c(vec.ori, vec.add))
# 
# meanCircular(c(vec.ori, vec.add))
# sdAccumCircular(deg = vec.add, M = meanCircular(vec.ori), sd = sdsCircular(vec.ori), n = length(vec.ori))
