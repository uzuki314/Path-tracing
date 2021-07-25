/**
 * Orignal program -> smallpt, a Path Tracer by Kevin Beason, 2008 
 * This program is rewrite from smallpt, which use c++.
 * I try to use the concept of Object-oriented Programming.
 */
#include <stdio.h>      
#include <stdlib.h>     // Usage: time ./raytracing 5000 && convert image.ppm result.png
#include <stdbool.h>    // 3900x wsl : real    16m29.468s
#include <math.h>       // Usage: time ./raytracing 1000 && convert image.ppm result.png
#include <string.h>     // 3900x wsl : real    3m17.405s

#include "avx2_vector.h"
#include "object_ray.h"
#include "object_sphere.h"
#include "math_toolkit.h"
#define spheres_num  10
/*  
        y|
         |____x
        /
       /z
*/
Sphere spheres[spheres_num];//Scene:radius, position,                 emission,                color,                      material 
void inispheres(){//spheres initialization 
    spheres[0] = iniSphere(1e5,      V$(nVec( 1e5+1,40.8,81.6)), V$(nVec(0,0,0)),   V$(nVec(.75,.25,.25)),    DIFF),//Left 
    spheres[1] = iniSphere(1e5,      V$(nVec(-1e5+99,40.8,81.6)),V$(nVec(0,0,0)),   V$(nVec(.25,.25,.75)),    DIFF),//Rght 
    spheres[2] = iniSphere(1e5,      V$(nVec(50,40.8, 1e5)),     V$(nVec(0,0,0)),   V$(nVec(.75,.75,.75)),    SPEC),//Back 
    spheres[3] = iniSphere(1e5,      V$(nVec(50,40.8,-1e5+300)), V$(nVec(0,0,0)),   V$(nVec(0,0,0)),          DIFF),//Frnt 
    spheres[4] = iniSphere(1e5,      V$(nVec(50, 1e5, 81.6)),    V$(nVec(0,0,0)),   V$(nVec(.75,.75,.75)),    DIFF),//Botm 
    spheres[5] = iniSphere(1e5,      V$(nVec(50,-1e5+81.6,81.6)),V$(nVec(0,0,0)),   V$(nVec(.75,.75,.75)),    DIFF),//Top 
    spheres[6] = iniSphere(16.5*.618,V$(nVec(65,32,78)),         V$(nVec(0,0,0)),   V$(nVec(.999,.999,.999)), REFR),//r_ear
    spheres[7] = iniSphere(16.5*.618,V$(nVec(35,32,78)),         V$(nVec(0,0,0)),   V$(nVec(.999,.999,.999)), REFR),//l_ear 
    spheres[8] = iniSphere(16.5,     V$(nVec(50,16.5,78)),       V$(nVec(0,0,0)),   V$(nVec(.999,.999,.999)), REFR),//face 
    spheres[9] = iniSphere(600,      V$(nVec(50,681.6-.27,81.6)),V$(nVec(12,12,12)),V$(nVec(0,0,0)),          DIFF);//Lite 
}

bool intersect(Ray *r, double *t, int *id){  //need to call by address 
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
int main(int argc, char *argv[]){ 
    inispheres();//原始像素 w=1024, h=768
    int w=1920, h=1440, samps = argc==2 ? atoi(argv[1])/4 : 1; // # samples 
    Ray cam = iniRay(V$(nVec(50,52,300)), V$(v_norm(V$(nVec(0,-0.042612,-1))))); // cam pos, dir 
    Vec cx=nVec(w*.5135/h,0,0), cy=v_times(V$(v_norm(V$(v_cross(&cx,&cam.d)))),.5135),r; 
    double **c = malloc(sizeof(double*)*(w*h));
    for (int i = 0; i < (w*h); i++){ c[i] = malloc(sizeof(double)*3); }
#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP 
    for (int y=0; y<h; y++){                       // Loop over image rows 
        fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*y/(h-1)); 
        for (unsigned short x=0, Xi[3]={0,0,y*y*y}; x<w; x++)   // Loop cols 
            for (int sy=0, i=(h-y-1)*w+x; sy<2; sy++)     // 2x2 subpixel rows 
                for (int sx=0; sx<2; sx++, r=nVec(0,0,0)){        // 2x2 subpixel cols 
                    //the color of the pixel is obtained by taking samps, averaging, and then averaging again based on 4 sub-pixels.
                    for (int s=0; s<samps; s++){ 
                        double r1=2*erand48(Xi), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1); 
                        double r2=2*erand48(Xi), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2); 
                        Vec d = v_fmadd(&cy, V$(uVec( ( (sy+.5 + dy)/2 + y)/h - .5)), V$(v_fmadd( &cx, V$(uVec(( (sx+.5 + dx)/2 + x)/w - .5)), &cam.d) ) ); 
                        r = v_fmadd(V$(radiance(R$(iniRay(V$( v_fmadd(&d, V$(uVec(140)), &cam.o)), V$(v_norm(&d))) ) ,0,Xi)) ,V$(uVec(1./samps)), &r);
                    } // Camera rays are pushed ^^^^^ forward to start in interior 
                    Vec c_i =  v_fmadd(V$(nVec(clamp(r.x),clamp(r.y),clamp(r.z))), V$(uVec(.25)), V$(nVec(c[i][0], c[i][1], c[i][2])) );
                    c[i][0] = c_i.x; c[i][1] = c_i.y; c[i][2] = c_i.z;
                } 
    } 
    FILE *f = fopen("image.ppm", "w");         // Write image to PPM file. 
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255); 
    for (int i=0; i<w*h; i++) fprintf(f,"%d %d %d ", toInt(c[i][0]), toInt(c[i][1]), toInt(c[i][2])); 
    return 0;
}