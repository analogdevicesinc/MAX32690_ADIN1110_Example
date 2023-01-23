#ifndef __ERRNO_H__
#define __ERRNO_H__

// IAR and lwIP have conflicting errno.h files.  Make sure only the IAR version is included.
#include <c/errno.h>

#endif // __ERRNO_H__
