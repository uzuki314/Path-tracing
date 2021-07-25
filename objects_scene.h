#ifndef __objects_scene_H__
#define __objects_Scene_H__

#include "object_sphere.h"

#define spheres_num  10
/*  
        y|
         |____x
        /
       /z
*/
Sphere spheres[spheres_num];
//Scene:                 radius,            position,               emission,             color,          material 
static inline void inispheres(){//spheres initialization 
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

static inline void iniscene(){
    inispheres();
}
#endif /*__objects_Scene_H__*/