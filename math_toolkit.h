#ifndef __math_toolkit_H__
#define __math_toolkit_H__

#include <math.h>

static inline double clamp(double x) { 
    return x<0 ? 0 : x>1 ? 1 : x;
} 
static inline int toInt(double x) { //gamma 
    return (int)(pow(clamp(x),1/2.2)*255+.5);
}

#endif /* __object_ray_H__ */
