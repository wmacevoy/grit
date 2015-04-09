all : tmps libs progs

tmps : tmp/linear_solve.hpp tmp/solve_damped_newton.hpp tmp/solve_empty.hpp

DEPS=utilities prepreprocess symbolic

tmp/linearalgebra.cppp : tmp/linear_solve.hppp

include ../rules.mk
