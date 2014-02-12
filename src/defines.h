#ifndef IGNIS_DEFINES_H
#define IGNIS_DEFINES_H

#include "../ignisDefines.h"

#ifndef IGNIS_DIM
#define IGNIS_DIM 2
#endif

#ifndef IGNIS_NX
#define IGNIS_NX 10
#endif

#ifndef IGNIS_NY
#define IGNIS_NY 10
#endif

#ifndef IGNIS_NZ
#define IGNIS_NZ 10
#endif


#if IGNIS_DIM == 3

#define IGNIS_NZ 100
#define IGNIS_N IGNIS_NX*IGNIS_NY*IGNIS_NZ

#else

#define IGNIS_N IGNIS_NX*IGNIS_NY

#endif


/*
 *
 */

#if IGNIS_DIM == 2 && defined (IGNIS_PERIODIC_Z)
#undef IGNIS_PERIODIC_Z
#endif

#define quickPrint(expr) \
    std::cout << expr << std::endl

#define IGNIS_X 0
#define IGNIS_Y 1
#define IGNIS_Z 2

#define IGNIS_UNSET_UINT 913371337

#define toStr boost::lexical_cast<std::string>


#endif // DEFINES_H
