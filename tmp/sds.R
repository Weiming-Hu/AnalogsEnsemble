# https://www.johndcook.com/blog/standard_deviation/

vec.ori <- sample(100, 90)
vec.add <- sample(100, 1)

# Correct answer
var(c(vec.ori, vec.add))

# Running mean answer
# 
# Sk = Sk-1 + (xk – Mk-1)*(xk – Mk).
# 

m.k.pre <- mean(vec.ori)
m.k.cur <- m.k.pre + (vec.add - m.k.pre) / (length(vec.ori) + 1)
(var(vec.ori) * (length(vec.ori) - 1) + (vec.add - m.k.pre) * (vec.add - m.k.cur)) / (length(vec.ori))


runningSds <- function(vec) {
  m.pre <- vec[1]
  m.cur <- 0
  k <- 1
  
  for (i in 2:length(vec)) {
    k <- k + 1
    m.cur <- m.pre + (vec[i] - m.pre) / k
  }
  
}


mean
var.circular

vec.ori <- c(807,249,73,658,930,272,544,878,923,709,440,165,492,42,987,503,327,729,840,612)
vec.add <- 0
s <- mean(sin(vec.ori * pi / 180))
c <- mean(cos(vec.ori * pi / 180))
e <- sqrt(1 - (s^2 + c^2))
asin(e) * (1 + (2/sqrt(3) - 1)* e^3) * 180 / pi

# Correct answer
s <- mean(sin(c(vec.ori, vec.add) * pi / 180))
c <- mean(cos(c(vec.ori, vec.add) * pi / 180))
e <- sqrt(1 - (s^2 + c^2))
asin(e) * (1 + (2/sqrt(3) - 1)* e^3) * 180 / pi

s <- mean(sin(vec.ori * pi / 180))
c <- mean(cos(vec.ori * pi / 180))
m.k.pre <- atan2(c, s)

s <- mean(sin(c(vec.ori, vec.add) * pi / 180))
c <- mean(cos(c(vec.ori, vec.add) * pi / 180))
atan2(c, s)

func <- function(x) {atan2(mean(cos(x * pi / 180)), mean(sin(x * pi / 180)))}
m.k.pre + (func(vec.add) - m.k.pre) / (length(vec.ori) + 1)

