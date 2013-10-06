#pragma once
// fit quadratic/linear so that the curve
//
// q0(t) = c0[0]+c0[1]*(t-ts[1])+c0[2]*(t-ts[1])^2/2 
//
// fits
//
// q0(ts[0])=p[0], q0(ts[1])=p[1], q0(ts[2])=p[2]
//
// Unless the c0[2] would be large compared to cutoff*c[1], so that
// only a linear fit is made:
//
void fit(double ts[3],float p[3],float c0[3], float c1[3], float cutoff);
