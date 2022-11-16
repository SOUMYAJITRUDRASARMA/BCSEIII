#ifndef REGION_CODES_H
#define REGION_CODES_H

#include <QtCore>

// REGION CODES FOR COHEN SUTHERLAND ALGORITHM ...
typedef char Region;  // UDRL

// Constants  for Cohen Sutherland ALgorithm ...
const
Region UP    = 1 << 3,
       DOWN  = 1 << 2,
       LEFT  = 1 << 1,
       RIGHT = 1 << 0,
       INSIDE = 0;


#endif // REGION_CODES_H
