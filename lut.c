#include <stdio.h>
#include <stdlib.h>
#include "lut.h"

Lut create_lut(unsigned int n){
    Lut lut;

    lut.n = n;
    lut.tab = (byte*)malloc(n * sizeof(byte));

    if (lut.tab == NULL) {
        perror("Erreur d'allocation mémoire pour le tableau de la Lut");
        exit(1);
    }
    
    return lut;

}

void clean_lut(Lut lut){
    if (! is_empty_lut(lut)){
        free(lut.tab); //Libère le tableau de valeurs
    }
}

int is_empty_lut(Lut lut){
    if (lut.n == 0 || lut.tab == NULL){
        return 1;
    }
    return 0;
}

Picture apply_lut(Lut lut, Picture p){

    if (is_empty_lut(lut) || is_empty_picture(p)){
        fprintf(stderr, "La lut ou l'image est vide\n");
        exit(1);       
    }

    unsigned int size = p.width*p.height*p.channels;

    Picture lut_p = create_picture(p.width, p.height, p.channels);

    for (unsigned int i = 0; i < size; i++){
        lut_p.content[i] = lut.tab[p.content[i]];
    }  

    return lut_p;
}


