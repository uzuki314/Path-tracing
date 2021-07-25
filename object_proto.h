#ifndef __object_proto_H__
#define __object_proto_H__

#include "avx2_vector.h"
//                     漫射   鏡面  玻璃
typedef enum _Refl_t { DIFF, SPEC, REFR } Refl_t;  // material types, used in radiance()
/**
 * @param p object position
 * @param e object emission
 * @param c object color
 * @param refl object reflection type (DIFFuse, SPECular, REFRactive) 
 * @param intersect use function pointer mimic Encapsulation
 */
#define extends_proto(name)\
    struct{\
        Vec p, e, c;\
        Refl_t refl; };\
    double (*intersect)(name *, Ray *);

#endif /* __object_proto_H__ */