#pragma once

#include "Safety.h"

SafetySP CreateSafetyClient(const std::string &me,const std::string &server, int rate);
