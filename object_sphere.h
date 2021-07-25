#ifndef __object_sphere_H__
#define __object_sphere_H__

#include "object_proto.h"
#include "object_ray.h"
typedef struct sphere Sphere;
struct sphere { 
    extends_proto(Sphere);       //extends object prototype    
    double rad;                         //radius 
};

/**
 * Determine whether the ray and the sphere intersect
 * Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
 *
 * @param s the sphere to calculate whether it intersects the vector "r"
 * @param r the ray to calculate whether it intersects the sphere "s"
 * @return the distance ,0 if nohit
 */
double s_intersect(Sphere *s,Ray *r);

Sphere iniSphere(double rad_, Vec *p_, Vec *e_, Vec *c_, Refl_t refl_);

#endif /* __object_sphere_H__ */