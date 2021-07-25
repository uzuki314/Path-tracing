#ifndef __object_ray_H__
#define __object_ray_H__

#include "avx2_vector.h"

#define R$(r) (Ray[]){r}

typedef struct ray {
    Vec o, d;
}Ray;

/**
 * initial a Ray using two Vec as param
 * 
 * @param _o Ray start point
 * @param _d directoin vector
 * @return a temporary Ray with Vec 'o' as start point and 'd' as direction vector .
 */
static inline Ray iniRay(Vec *_o, Vec *_d){
    return(Ray){ .o = *_o,
                 .d = *_d };
}
#endif /* __object_ray_H__ */