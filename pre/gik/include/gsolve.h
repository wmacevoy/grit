#pragma once

#include <string>
#include <ostream>
#include "mat.h"

void gsolve(std::ostream &out,
	    const std::string &name, const Vec &equations, 
            const Vec &unknowns, const Vec &parameters);
