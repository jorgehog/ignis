#pragma once

#include <limits>
#include <sys/types.h>

#ifndef IGNIS_DIM
#define IGNIS_DIM 3
#endif

const uint IGNIS_X = 0;
const uint IGNIS_Y = 1;
const uint IGNIS_Z = 2;

const uint IGNIS_UNSET_UINT = std::numeric_limits<uint>::max();
