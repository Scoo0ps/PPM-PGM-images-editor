#include <stdio.h>
#include "pixels.h"


int arrange_pixel_value(int pixel_value){
    if (pixel_value < 0){
        return 0;
    }
    else if (pixel_value > 255){
        return 255;
    }
    else{
        return pixel_value;
    }
}


