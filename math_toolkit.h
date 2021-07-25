#ifndef __math_toolkit_H__
#define __math_toolkit_H__

#include <math.h>
#include "objects_scene.h"

static inline double clamp(double x) { 
    return x<0 ? 0 : x>1 ? 1 : x;
} 

static inline int toInt(double x) { //gamma 
    return (int)(pow(clamp(x),1/2.2)*255+.5);
}


bool intersect(Ray *r, double *t, int *id){ 
    double d, inf = *t = 1e20; 
    for(int i=spheres_num;i--;) if( (d= spheres[i].intersect(&spheres[i],r)) && (d<*t) ){*t=d;*id=i;} 
    return *t<inf; 
} 

/**
 * The ray tracing algorithm 
 *
 * L0 = Le0 + f0*(L1)
 *    = Le0 + f0*(Le1 + f1*L2)
 *    = Le0 + f0*(Le1 + f1*(Le2 + f2*(L3))
 *    = Le0 + f0*(Le1 + f1*(Le2 + f2*(Le3 + f3*(L4)))
 *    = ...
 *    = Le0 + f0*Le1 + f0*f1*Le2 + f0*f1*f2*Le3 + f0*f1*f2*f3*Le4 + ...
 *
 * So:
 *  F = 1
 *  while (1){
 *     L += F*Lei
 *     F *= fi
 *  }
 * 
 * @param r_ 
 * @param depth_ :depth; A counter that count number of current reflection
 * @param Xi 
 * @return Vec 
 */
Vec radiance(Ray *r_, int depth_, unsigned short *Xi){
    double t;                               // distance to intersection
    int id=0;                               // id of intersected object
    Ray r=*r_;
    int depth=depth_;
    
    Vec cl = nVec(0,0,0);   // accumulated color
    Vec cf = nVec(1,1,1);   // accumulated reflectance
    while (1){
        if (!intersect(&r, &t, &id)) return cl; // if miss, return black
        
        Sphere *obj = &spheres[id];        // the hit object
        Vec x = v_fmadd(&r.d,V$(uVec(t)),&r.o), 
            n = v_norm(V$((v_sub(&x,&obj->p)))), 
            nl = v_dot(&n,&r.d)<0?n:v_times(&n,-1), f = obj->c;
        double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
        cl = v_fmadd(&cf,&obj->e,&cl);
        if (++depth>5) {if (erand48(Xi)<p) f=v_times(&f,(1/p)); else return cl;} //R.R.   
        cf = v_mul(&cf,&f);
        if (obj->refl == DIFF){                  // Ideal DIFFUSE reflection
            double r1=2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2);
            Vec w=nl, u=v_norm(V$(v_cross(V$(fabs(w.x)>.1?nVec(0,1,0):nVec(1,0,0)),&w))), v=v_cross(&w,&u);
            Vec d = v_norm(V$(v_fmadd(&w, V$(uVec(sqrt(1-r2))), V$(v_fmadd(&u, V$(uVec(cos(r1)*r2s)), V$(v_times(&v,sin(r1)*r2s)))))));
            //return obj.e + f.mult(radiance(Ray(x,d),depth,Xi));
            r = iniRay(&x,&d);
            continue;
        } else if (obj->refl == SPEC){           // Ideal SPECULAR reflection
            //return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi));
            r = iniRay(&x, V$(v_sub(&r.d, V$(v_times(&n,2*v_dot(&n,&r.d))))) );
            continue;
        }
        Ray reflRay = iniRay(&x, V$(v_sub(&r.d, V$(v_times(&n,2*v_dot(&n,&r.d)))))) ;     // Ideal dielectric REFRACTION
        bool into = v_dot(&n,&nl)>0;                // Ray from outside going in?
        double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=v_dot(&r.d,&nl), cos2t;
        if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0){    // Total internal reflection
            //return obj.e + f.mult(radiance(reflRay,depth,Xi));
            r = reflRay;
            continue;
        }
        Vec tdir = v_norm(V$(v_sub(V$(v_times(&r.d,nnt)), V$(v_times(&n,((into?1:-1)*(ddn*nnt+sqrt(cos2t))))))));
        double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:v_dot(&tdir,&n));
        double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
        // return obj.e + f.mult(erand48(Xi)<P ?
        //                       radiance(reflRay,    depth,Xi)*RP:
        //                       radiance(Ray(x,tdir),depth,Xi)*TP);
        if (erand48(Xi)<P){
            cf = v_times(&cf,RP);
            r = reflRay;
        } else {
            cf = v_times(&cf,TP);
            r = iniRay(&x,&tdir);
        }
        continue;
    }
}

#endif /* __object_ray_H__ */
