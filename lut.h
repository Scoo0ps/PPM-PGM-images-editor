#include <stdio.h>
#include "pictures.h"

#ifndef LUT_H
#define LUT_H

typedef struct {
    unsigned int n;
    byte* tab;
} Lut;


Lut create_lut(unsigned int);

void clean_lut(Lut);

int is_empty_lut(Lut);

Picture apply_lut(Lut, Picture);

#endif
