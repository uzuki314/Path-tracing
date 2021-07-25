/**
 * This program get the conception from
 *      -> smallpt, a Path Tracer by Kevin Beason, 2008 
 *      -> https://github.com/embedded2016/raytracing
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

int main(int argc, char *argv[]){ 
    iniscene();//原始像素 w=1024, h=768
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