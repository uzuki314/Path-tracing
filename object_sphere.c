#include "object_sphere.h"

double s_intersect(Sphere *s,Ray *r) {
    Vec op = v_sub(&(s->p), &(r->o)); 
    double t, eps=1e-4, b=v_dot(&op,&(r->d)), det = b*b - v_dot(&op,&op) + s->rad * s->rad; 
    if (det<0) return 0; else det = sqrt(det); 
    return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0); 
}

Sphere iniSphere(double rad_, Vec *p_, Vec *e_, Vec *c_, Refl_t refl_) {
    return (Sphere){ .rad  = rad_,
                     .p    = *p_,
                     .e    = *e_,
                     .c    = *c_,
                     .refl = refl_,
                     .intersect = s_intersect};
}