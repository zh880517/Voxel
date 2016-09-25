#pragma once
#include <iostream>


#ifndef LOG_ERROR
#define LOG_ERROR(msg) std::cout <<__FUNCTION__ <<  "Error: " << msg << std::endl;
#endif