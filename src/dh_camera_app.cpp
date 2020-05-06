#include "dhnetsdk.h"
#include "dh_camera.h"
#include <stdio.h>



int main(int argc, char *argv[]){
    ros::init(argc,argv,"dh_camera_app");
    dh_camera::DhCamera camera;
    printf("start run camera!\n");
    camera.run();
    printf("finish run camera!\n");
    return 0;
}