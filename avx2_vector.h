#ifndef __avx2_vector_H__
#define __avx2_vector_H__

#include <math.h>
#include <x86intrin.h>

/**
 * Let the temporarily returned Vec be available "address of" which can then be passed into the function as a pointer parameter.
 * 
 */
#define V$(v) (Vec[]){v}  

/**
 * combine three doubles and __m256d in Vec union.
 * __m256d's first three mem will aligning x, y, z individually.
 */
typedef union vec{
    struct { double x, y, z; };
    __m256d vec;
}Vec;

/**
 * initial a Vec using three double as param
 * 
 * @param _x x direction vector
 * @param _y y direction vector
 * @param _z z direction vector
 * @return a temporary Vec which uses three doubles as three-dimensional vector.
 */
static inline Vec nVec(double _x, double _y, double _z) { 
    return (Vec){ .x = _x,
                  .y = _y, 
                  .z = _z }; 
};

/**
 * initiall a Vec using one double, it will do a scalar expansion like (a)->(a,a,a)
 * 
 * @param _n scalar
 * @return a temporary Vec which three directions have the same value.
 */
static inline Vec uVec(double _n) { 
    return (Vec){ .vec = _mm256_set_pd(_n,_n,_n,_n) };
};

/**
 * initial a Vec by __m256d
 * 
 * @param vec __m256d, which include 4 doubles, only the first three are used in this implement.
 * @return a temporary Vec. 
 */
static inline Vec mVec(__m256d vec) { 
    return (Vec){ .vec = vec };
};

/**
 * add Vec 'a' and 'b' element-by-element 
 * 
 * @param a 
 * @param b 
 * @return a temporary Vec which from a+b. 
 */
static inline Vec v_add(Vec *a,  Vec *b){
    return mVec(_mm256_add_pd(a->vec, b->vec));
}

/**
 * subtract Vec 'a' and 'b' element-by-element 
 * 
 * @param a 
 * @param b 
 * @return a temporary Vec which from a-b. 
 */
static inline Vec v_sub(Vec *a,  Vec *b){
    return mVec(_mm256_sub_pd(a->vec, b->vec)); 
}

/**
 * multiply Vec 'a' and 'b' element-by-element 
 * 
 * @param a 
 * @param b 
 * @return a temporary Vec which from a.*b. 
 */
static inline Vec v_mul(Vec *a,  Vec *b){
    return mVec(_mm256_mul_pd(a->vec, b->vec));
}

/**
 * multiply Vec 'a' and 'b' element-by-element, and then add 'c' element-by-element as the same.
 * different with v_add(V$(v_mul(a, b)), c),it use FMA instruction
 * 
 * @param a 
 * @param b 
 * @param c
 * @return a temporary Vec which from a.*b+c. (Fused-Multiply-Add)
 */
static inline Vec v_fmadd(Vec *a,  Vec *b, Vec *c){
    return mVec(_mm256_fmadd_pd(a->vec, b->vec, c->vec));
} 

/**
 * multiply Vec 'a' and uVec(t) element-by-element 
 * 
 * @param a 
 * @param t 
 * @return a temporary Vec which from t*a. 
 */
static inline Vec v_times(Vec *a,double t){
    return mVec(_mm256_mul_pd(a->vec, _mm256_set_pd(t,t,t,t))); 
}

/**
 * do a cross product for Vec 'a' and 'b' 
 * 
 * @param a 
 * @param b 
 * @return a temporary Vec which from a cross b. 
 */
static inline Vec v_cross(Vec *a,  Vec *b){
    return (Vec){ .vec =_mm256_permute4x64_pd(
    _mm256_sub_pd(
    _mm256_mul_pd(a->vec, _mm256_permute4x64_pd(b->vec, _MM_SHUFFLE(3, 0, 2, 1))),
    _mm256_mul_pd(b->vec, _mm256_permute4x64_pd(a->vec, _MM_SHUFFLE(3, 0, 2, 1)))
    ), _MM_SHUFFLE(3, 0, 2, 1))};
}

/**
 * do a dot product for Vec 'a' and 'b' 
 * 
 * @param a 
 * @param b 
 * @return a double which from a dot b. 
 */
static inline double v_dot(Vec *a,  Vec *b){
    Vec result = v_mul(a,b); return result.x+result.y+result.z;
}

/**
 * Normalize the a Vec
 * 
 * @param a 
 * @return 'a' vec which has been normalized.
 */
static inline Vec   v_norm(Vec *a){                     // a/|a|  normalization
    return *a = (v_times(a,1/sqrt(v_dot(a,a))));
}

#endif /* __avx2_vector_H__ */