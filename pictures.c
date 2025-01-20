#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <math.h>

#include "pictures.h"
#include "lut.h"

#define MAX_LINE_LENGTH 50

/*image nulle*/
Picture null_p = {0, 0, 0, NULL};

int is_file_pgm(char* filename){
    if(strcmp(ext_from_path(filename), "pgm") == 0){
        return 1;
    }
    return 0;
}

int is_file_ppm(char* filename){
    if(strcmp(ext_from_path(filename), "ppm") == 0){
        return 1;
    }
    return 0;
}

int is_empty_picture(Picture p){
    if (p.content == NULL || p.channels == 0 || p.height == 0 || p.width == 0){
        return 1;
    }
    return 0;
}

int is_file_picture(char* filename){
    if (is_file_pgm(filename) || is_file_ppm(filename)){
        return 1;
    }
    return 0;
}

Picture create_picture(unsigned int width, unsigned int height, unsigned int channels){

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Dimensions invalides\n");
        return null_p;
    }

    if (channels != 1 && channels != 3){
        fprintf(stderr, "Nombre de canaux incorrect\n");
        return null_p;
    }


    Picture p;
    
    p.width = width;
    p.height = height;
    p.channels = channels;
    
    int content_size = width*height*channels;
    
    p.content = (byte*)malloc(content_size * sizeof(byte));

    if (p.content == NULL) {
        fprintf(stderr,"Erreur d'allocation mémoire pour les données des pixels\n");
        return null_p;
    }

    /*Initialiser les pixels à 0 (image noire ou vide)*/
    for (int i = 0; i < content_size; i++) {
        p.content[i] = 0;
    }
 
    return p;
}

void checkforhashtags(FILE* fptr){
    if (fptr == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
        exit(1);
    }

    int c;
    while ((c = fgetc(fptr)) == '#') { 
        // Ligne de commentaire détectée
        // Ignorer jusqu'à la fin de ligne
        while ((c = fgetc(fptr)) != '\n' && c != EOF); 
    }
    if (c != EOF) {
        // Replacer le caractère non commenté dans le flux
        ungetc(c, fptr); 
    }
}

Picture read_picture(char* filename){

    if (!is_file_picture(filename)){  //verification du bon format du fichier
        fprintf(stderr, "Lecture impossible : l'image n'a pas un format correct %s\n", filename);
        exit(1);
    } 

    FILE* fptr = fopen(filename, "rb"); 
    if (fptr == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(1);
    }
    
    // Lire l'en-tête du fichier
    char magicNum[3]; 
    unsigned int width, height, channels, max_value;
    
    //On accède aux valeurs en ignorant les commentaires
    fscanf(fptr,"%s\n", magicNum);   
    checkforhashtags(fptr); //on avance jusqu'à la nouvelle ligne sans commentaires 

    fscanf(fptr,"%d %d\n", &width, &height);
    checkforhashtags(fptr); 

    fscanf(fptr,"%d\n", &max_value);

    if (strcmp(magicNum, "P5") == 0 && is_file_pgm(filename)){
        channels = 1;
    }
    else if (strcmp(magicNum, "P6") == 0 && is_file_ppm(filename)){
        channels = 3;
    }
    else {
        fprintf(stderr, "Impossible de lire l'image : le format ou l'extension de l'image est incorrect\n");
        fclose(fptr);
        exit(1);
    }

    if (height <= 0 || width <= 0){
        fprintf(stderr, "Impossible de lire l'image : dimensions incorrectes (%d x %d)\n", width, height);
        fclose(fptr);
        exit(1);
    }
    if (max_value <= 0 || max_value > 255){
        fprintf(stderr, "Impossible de lire l'image : valeur max des pixels incorrecte (%d)\n", max_value);
        fclose(fptr);
        exit(1);
    }

    //Le curseur du fichier est au niveau du début des binaries pixel, la taille calculée sera celle des pixels
    long header_offset = ftell(fptr);
    fseek(fptr, 0, SEEK_END); //on met le curseur à la fin du fichier
    long file_size = ftell(fptr);
    fseek(fptr, header_offset, SEEK_SET); //on remet le curseur au début des données pixels pour la suite

    //On doit retrancher à la taille du fichier la taille de l'en tête pour avoir celle des données (pixels)
    size_t binaries_size = (size_t) (file_size - header_offset);
    size_t pixels_size = (size_t) (width * height * channels);

    if (binaries_size < pixels_size){
        fprintf(stderr, "Nombre incorrect de pixels : attendu %zu octets, trouvé %zu octets\n", pixels_size, binaries_size);
        fclose(fptr);
        exit(1);
    }

    //Lecture des données binaires : un pixel gris n'alloue pas la même mémoire qu'un pixel RGB
    byte* content = malloc(pixels_size * sizeof(byte));
    size_t read_size = fread(content, sizeof(byte), pixels_size, fptr);

    if (read_size != pixels_size) {
        fprintf(stderr, "Erreur de lecture des données binaires : attendu %zu octets, lu %zu octets\n", pixels_size, read_size);
        fclose(fptr);
        return null_p;
    }

    for (size_t i = 0; i < pixels_size; i++){
        int val = (int)round(content[i] * (255/max_value));
        val = arrange_pixel_value(val);
        content[i] = (byte)val;
    }
    
    Picture p;
    p.width = width;
    p.height = height;
    p.channels = channels;
    p.content = content;

    fclose(fptr);
    return p;

}

void clean_picture(Picture p) {
    if (p.content != NULL) {
        free(p.content);
        p.content = NULL;
    }
    p.width = 0;
    p.height = 0;
    p.channels = 0;
}

Picture copy_picture(Picture p) {

    Picture copy_p = create_picture(p.width, p.height, p.channels);

    int size = p.width * p.height * p.channels;
    for (int i = 0; i < size; i++){
        copy_p.content[i] = p.content[i];
    }

    return copy_p;
}

int is_gray(Picture p){
    if (p.channels == 1){
        return 1;
    }
    return 0;
}

int is_color(Picture p){
    if (p.channels == 3){
        return 1;
    }
    return 0;
}

void info_picture(Picture p){
    printf("(%ux%ux%u)", p.width, p.height, p.channels);
    return;
}

Picture convert_to_color_picture(Picture p){
    if (is_empty_picture(p)){
        fprintf(stderr,"L'image fournie est vide, elle ne peut être convertie en couleur\n");
        return null_p;
    }
    if (is_color(p)){
        return copy_picture(p);
    }
    if (is_gray(p)){

        Picture color_p = create_picture(p.width, p.height, 3);

        for (unsigned int i = 0; i < p.height; i++){
            for (unsigned int j = 0; j < p.width; j++){
                    int index_color = (i * p.width + j)*3;
                    int index_gray = i * p.width + j;

                    byte gray = p.content[index_gray];
                    color_p.content[index_color] = gray;
                    color_p.content[index_color + 1] = gray;
                    color_p.content[index_color + 2] = gray;
               
            } 
        }
        return color_p;
    }
    return null_p;

    
}

Picture convert_to_gray_picture(Picture p){
    if (is_empty_picture(p)){
        fprintf(stderr,"L'image fournie est vide, elle ne peut être convertie en niveaux de gris\n");
        return null_p;
    }
    if (is_gray(p)){
        return copy_picture(p);
    }
    if (is_color(p)){

        //obligé de dimensionner l'image résultat car chaque pixel a trois composantes en couleur
        Picture gray_p = create_picture(p.width, p.height, 1); 

        for (unsigned int i = 0; i < p.height; i++){
            for (unsigned int j = 0; j < p.width; j++){
                    int index_color = (i * p.width + j)*3;
                    int index_gray = i*p.width + j;

                    byte tmp_r = p.content[index_color];
                    byte tmp_g = p.content[index_color + 1];
                    byte tmp_b = p.content[index_color + 2];

                    int grey_value = (int)round(0.299*tmp_r + 0.587*tmp_g + 0.114*tmp_b);

                    //Pour que le pixel ne depasse pas 255
                    grey_value = arrange_pixel_value(grey_value);

                    gray_p.content[index_gray] = (byte)grey_value;
               
            } 
        }
        return gray_p;
    }

    return null_p;
}

Picture* split_picture(Picture p){
    if (is_gray(p)){
        Picture* t = malloc(sizeof(Picture));
        if (t == NULL){
            perror("Erreur lors de l'allocation mémoire du tableau d'images");
            return NULL;
        }
        *t = p;
        return t;
    }
    if (is_empty_picture(p)){
        return NULL;
    }

    Picture p1 = create_picture(p.width, p.height, 1);
    Picture p2 = create_picture(p.width, p.height, 1);
    Picture p3 = create_picture(p.width, p.height, 1);

    Picture* t = malloc(3 * sizeof(Picture));
    for (unsigned int i = 0; i < p.height; i++){
            for (unsigned int j = 0; j < p.width; j++){
                    int index_color = (i * p.width + j)*3;
                    int index_gray = i * p.width + j;

                    byte tmp_r = p.content[index_color];
                    byte tmp_g = p.content[index_color + 1];
                    byte tmp_b = p.content[index_color + 2];

                    int grey1_value = (int)round(0.299*tmp_r);
                    int grey2_value = (int)round(0.587*tmp_g);
                    int grey3_value = (int)round(0.114*tmp_b);

                    // Pour que aucune des valeurs ne dépasse 255
                    grey1_value = arrange_pixel_value(grey1_value);
                    grey2_value = arrange_pixel_value(grey2_value);
                    grey3_value = arrange_pixel_value(grey3_value);

                    p1.content[index_gray] = (byte)grey1_value;
                    p2.content[index_gray] = (byte)grey2_value;
                    p3.content[index_gray] = (byte)grey3_value;
               
            } 
        }
        t[0] = p1;
        t[1] = p2;
        t[2] = p3;
        return t;
}

Picture merge_picture(Picture red_p, Picture green_p, Picture blue_p){
    if (is_empty_picture(red_p) || is_empty_picture(green_p) || is_empty_picture(blue_p)){
        fprintf(stderr,"Une ou plusieurs des trois images composites est vide\n");
        return null_p;
    }
    if (red_p.width != blue_p.width || red_p.height != blue_p.height || red_p.width != blue_p.width || red_p.height != blue_p.height){
        fprintf(stderr,"Les trois images composites n'ont pas toutes la même taille\n");
        return null_p;
    }
    if (is_color(red_p) || is_color(green_p) || is_color(blue_p)){
        fprintf(stderr,"Il y a au moins une image en couleur parmi les images composites\n");
        return null_p;
    }

    Picture merged_p = create_picture(red_p.width, red_p.height, 3);

    for (unsigned int i = 0; i < merged_p.height; i++){
            for (unsigned int j = 0; j < merged_p.width; j++){
                    int index_color = (i * merged_p.width + j) * 3;
                    int index_gray = i * merged_p.width + j;

                    byte gray_r = red_p.content[index_gray];
                    byte gray_g = green_p.content[index_gray];
                    byte gray_b = blue_p.content[index_gray];

                    merged_p.content[index_color] = gray_r;
                    merged_p.content[index_color + 1] = gray_g;
                    merged_p.content[index_color + 2] = gray_b;
               
            } 
    }

    return merged_p;

}

Picture brighten_picture(Picture p, double factor){
    if (is_empty_picture(p)){
        fprintf(stderr, "L'image fournie est vide\n");
        return null_p;
    }
    if (factor < 1){
        //si factor > 0 et < 1, l'image ne sera pas éclaircie mais assombrie ; ce n'est pas ce qu'on veut
        fprintf(stderr, "Le facteur renseigné (%f) est incorrect, il doit être strictement supérieur ou égal à 1\n", factor);
        return null_p;
    }

    Picture brightened_p = create_picture(p.width, p.height, p.channels);

    if (is_gray(p)){

        for (unsigned int i = 0; i < p.height; i++){
            for (unsigned int j = 0; j < p.width; j++){
                    int index_gray = (i * p.width + j);

                    byte gray = p.content[index_gray];
                    int val = (int)round(gray*factor);

                    // Pour que le pixel ne dépasse pas 255
                    val = arrange_pixel_value(val);
                    brightened_p.content[index_gray] = (byte)val;
                    
               
            } 
        }
    }

    else if (is_color(p)){
        
        for (unsigned int i = 0; i < p.height; i++){
            for (unsigned int j = 0; j < p.width; j++){
                    int index_color = (i * p.width + j)*3;

                    int new_r = (int)round(p.content[index_color] * factor);
                    int new_g = (int)round(p.content[index_color + 1] * factor);
                    int new_b = (int)round(p.content[index_color + 2] * factor); 
                    
                    //Pour que les pixels ne dépassent pas 255
                    new_r = arrange_pixel_value(new_r);
                    new_g = arrange_pixel_value(new_g);
                    new_b = arrange_pixel_value(new_b);

                    brightened_p.content[index_color] = (byte)new_r;
                    brightened_p.content[index_color + 1] = (byte)new_g;
                    brightened_p.content[index_color + 2] = (byte)new_b;
                    
               
            } 
        }

    }

    else {
        brightened_p = null_p;
    }

    return brightened_p;


}

Picture melt_picture(Picture p, int number){
    if (is_empty_picture(p)){
        fprintf(stderr, "L'image fournie est vide\n");
        return null_p;
    }
    if (number == 0){
        return p;
    }

    Picture melted_p = copy_picture(p);

    if (is_gray(p)) {
        for (int k = 0; k < number; k++){
        //random_index est compris entre 1 et hauteur-1
        int i = rand() % (p.height - 1) + 1;
        int j = rand() % p.width;

        //Index actuel et au-dessus
        int current = i*p.height + j;
        int top = (i-1)*p.height + j;

        //Comparaison direct des valeurs du pixel actuel et celui au dessus
        if (p.content[current] > p.content[top]){

            melted_p.content[current] = p.content[top];

            }
        }
    }

    else if (is_color(p)) {
        for (int k = 0; k < number; k++){
        //random_index est compris entre 1 et hauteur-1
        int i = rand() % (p.height - 1) + 1;
        int j = rand() % p.width;

        //Index actuel et au-dessus : il y a 3 canaux 
        int current = (i*p.height + j) * 3;
        int top = ((i-1)*p.height + j) * 3;

        //Calcul des valeurs de gris pour comparaison
        int gray_current = (int)round(0.299 * p.content[current] + 0.587 * p.content[current + 1] + 0.114 * p.content[current + 2]);
        int gray_top = (int)round(0.299 * p.content[top] + 0.587 * p.content[top + 1] + 0.114 * p.content[top + 2]);

        //Comparaison des valeurs de gris du pixel actuel et celui au-dessus
        if (gray_current > gray_top){

            melted_p.content[current] = p.content[top];
            melted_p.content[current + 1] = p.content[top + 1];
            melted_p.content[current + 2] = p.content[top + 2];

            }
        }
    }

    else {
        melted_p = null_p;
    } 

    return melted_p;
}

int write_picture(Picture p, char* filename){
    if (filename == NULL) {
        fprintf(stderr, "Le nom de fichier ne peut pas être nul\n");
        return 1; 
    }
    if (p.content == NULL || p.width <= 0 || p.height <= 0) {
        fprintf(stderr, "L'image fournie (%d x %d) n'est pas écrivable\n", p.width, p.height);
        return 2; 
    }

    char* extension = strrchr(filename, '.');
    if (!extension || extension == filename) {
        fprintf(stderr, "Fichier n'a pas une extension valide\n");
        return 3;
    }

    FILE* fptr = fopen(filename, "wb");
    if (!fptr) {
        fprintf(stderr, "Impossible d'ouvrir le fichier\n");
        return 4;
    }

    char format[3] = {'P','6','\0'}; // par défaut image RGB (.ppm)
    if (p.channels == 1) { format[1] = '5'; }


    char header[64];
    // stock l'en tête dans header
    int header_length = sprintf(header, "%s\n%d %d\n255\n", format, p.width, p.height); 

    if (header_length < 0){
        fprintf(stderr, "Erreur lors de la création de l'en-tête\n");
        fclose(fptr);
        return 5;
    }

    //ecriture de l'en tête
    if (fwrite(header, sizeof(char), header_length, fptr) != (size_t)header_length){
        fprintf(stderr, "Erreur lors de l'écriture de l'en-tête\n");
        fclose(fptr);
        return 6;
    }
    
    //ecriture des pixels de l'image
    size_t content_size = (size_t)p.width * p.height * p.channels;
    size_t written_size = fwrite(p.content, sizeof(byte), content_size, fptr);

    if (written_size != content_size){
        fprintf(stderr, "Erreur lors de l'écriture des pixels de l'image");
        fclose(fptr);
        return 7;
    }

    fclose(fptr);
    printf("%s écrit correctement\n", filename);
    return 0;   
}

Picture inverse_picture(Picture p){
    if (is_empty_picture(p)){
        return null_p;
    }
    //création de la LUT d'inverse
    Lut inverse_lut = create_lut(256);

    for (int i = 0; i < 256; i++) {
        inverse_lut.tab[i] = 255 - i; 
    }
    
    Picture inverse_p = apply_lut(inverse_lut, p);
    clean_lut(inverse_lut);
    return inverse_p;
}

Picture normalize_dynamic_picture(Picture p){
    if (is_empty_picture(p)){
        return null_p;
    }

    int size = p.width*p.height*p.channels;

    byte min = 255;
    byte max = 0;

    for (int i = 0; i < size; i++){
        if (p.content[i] < min) {
            min = p.content[i];
        }
        if (p.content[i] > max) {
            max = p.content[i];
        }
    }

    if (min == max){
        printf("Tous les pixels de l'image valent : %u. Pas de normalisation\n", min);
        return p;   
    }

    Lut normalization_lut = create_lut(256);

    for (int i = 0; i < 256; i++) {
        if (i < min) {
            normalization_lut.tab[i] = 0;
        } 
        else if (i > max) {
            normalization_lut.tab[i] = 255;
        } 
        else {
            normalization_lut.tab[i] = (byte)(int)round(((i - min) * 255) / (max - min));
        }
    }

    Picture normalized_img = apply_lut(normalization_lut,p);
    clean_lut(normalization_lut);
    return normalized_img;
    
}

Picture set_levels_picture(Picture p, byte nb_levels){
    if (is_empty_picture(p)){
        fprintf(stderr,"L'image est vide\n");
        return null_p;
    }
    if (nb_levels < 2) {
        fprintf(stderr,"Le nombre de niveaux doit être compris entre 2 et 255\n");
        return null_p;
    }

    //taille de la lut : correspond au nombre de plages de valeurs
    int lut_size = (int)round(255/nb_levels);

    //lut plus petite pour permettre le groupement de pixels
    Lut levels_lut = create_lut(lut_size);

    for (int i = 0; i < nb_levels; i++) {
        levels_lut.tab[i] = (byte)i;
    }

    Picture levels_p = apply_lut(levels_lut, p);
    clean_lut(levels_lut);
    return levels_p;
}

int have_same_w_and_h(Picture p1, Picture p2){
    if (p1.width == p2.width && p1.height == p2.height) {
        return 1; 
    } 
    return 0;
}

Picture difference_picture(Picture p1, Picture p2){
    if (is_empty_picture(p1) || is_empty_picture(p2)){
        fprintf(stderr, "L'une des 2 images est nulle\n");
        return null_p;
    }

    if (!have_same_w_and_h(p1 , p2)){
        fprintf(stderr, "Les 2 images n'ont pas les mêmes dimensions (peu importe les canaux)\n");
        return null_p; 
    }

    if (p1.channels != p2.channels){
        fprintf(stderr, "Les 2 images n'ont pas le même nombre de canaux\n");
        return null_p; 
    }

    /*on normalise les 2 images pour faire par la suite la différence (on a besoin que les pixels soient sur la même plage de valeurs)*/
    Picture normalized_p1 = normalize_dynamic_picture(p1);
    Picture normalized_p2 = normalize_dynamic_picture(p2);

    int size = p1.width*p1.height*p1.channels;

    //result_p sera une image normalisée comme différence d'images normalisées 
    Picture result_p = create_picture(p1.width, p1.height, p1.channels);
    
    for (int i = 0; i < size; i++){
        result_p.content[i] = (byte)abs(normalized_p1.content[i] - normalized_p2.content[i]);
    }

    return result_p;   
}

Picture mult_picture(Picture p1, Picture p2){
    if (is_empty_picture(p1) || is_empty_picture(p2)){
        fprintf(stderr, "L'une des 2 images est nulle\n");
        return null_p;
    }

    if (!have_same_w_and_h(p1 , p2)){
        fprintf(stderr, "Les 2 images n'ont pas les mêmes dimensions (peu importe les canaux)\n");
        return null_p; 
    }

    //si les 2 images sont du même type
    if (p1.channels == p2.channels){

        int size = p1.width*p1.height*p1.channels;

        Picture mult_p = create_picture(p1.width, p1.height, p1.channels);
        
        for (int i = 0; i < size; i++){
            //Toute valeur supérieure à 255 sera ramenée dans la plage valide[0,255] en effectuant une réduction modulo 256.
            mult_p.content[i] = (byte)((p1.content[i] * p2.content[i]) % 256);
        }

        return mult_p;   
    }
    //si les 2 images sont de type différents
    else{

        //l'image résultante sera RVB
        Picture mult_p = create_picture(p1.width, p1.height, 3);

        //p1 couleur et p2 gris
        if (is_color(p1)){

            for (unsigned int i = 0; i < p1.height; i++){
                for (unsigned int j = 0; j < p1.width; j++){

                    int gray_index = (i * p1.width + j);
                    int color_index = (i * p1.width + j) * 3;
                    
                    //Toute valeur supérieure à 255 sera ramenée dans la plage valide[0,255] en effectuant une réduction modulo 256.
                    mult_p.content[color_index] = (byte)((p1.content[color_index] * p2.content[gray_index]) % 256);
                    mult_p.content[color_index + 1] = (byte)((p1.content[color_index + 1] * p2.content[gray_index]) % 256);
                    mult_p.content[color_index + 2] = (byte)((p1.content[color_index + 2] * p2.content[gray_index]) % 256);

                }
                
            }       

        }

        //p1 gris et p2 couleur
        else {

            for (unsigned int i = 0; i < p1.height; i++){
                for (unsigned int j = 0; j < p1.width; j++){

                    int gray_index = (i * p1.width + j);
                    int color_index = (i * p1.width + j) * 3;
                    
                    //Toute valeur supérieure à 255 sera ramenée dans la plage valide[0,255] en effectuant une réduction modulo 256.
                    mult_p.content[color_index] = (byte)((p1.content[gray_index] * p2.content[color_index]) % 256);
                    mult_p.content[color_index + 1] = (byte)((p1.content[gray_index] * p2.content[color_index + 1]) % 256);
                    mult_p.content[color_index + 2] = (byte)((p1.content[gray_index] * p2.content[color_index + 2]) % 256);

                }
                
            }    

        }
    
    return mult_p;   


    }

    }

Picture mix_picture(Picture p1, Picture p2, Picture p3){
     if (is_empty_picture(p1) || is_empty_picture(p2) || is_empty_picture(p3)){
        fprintf(stderr, "L'une des 3 images est nulle\n");
        return null_p;
    }

    if (!have_same_w_and_h(p1 , p2) || !have_same_w_and_h(p1,p3) || !have_same_w_and_h(p2,p3)){
        fprintf(stderr, "Les 3 images n'ont pas les mêmes dimensions (peu importe les canaux)\n");
        return null_p; 
    }

    Picture p1_color = normalize_dynamic_picture(convert_to_color_picture(p1));
    Picture p2_color = normalize_dynamic_picture(convert_to_color_picture(p2));
    Picture p3_color = normalize_dynamic_picture(convert_to_color_picture(p3));

    int size = p1.width*p1.height*3;
 
    Picture result_p = create_picture(p1.width, p1.height, 3);
    
    for (int i = 0; i < size; i++){

        int alpha = p3_color.content[i]/255;
        result_p.content[i] = (byte)((1 - alpha) * p1_color.content[i] + alpha * p2_color.content[i]);
        result_p.content[i] = arrange_pixel_value(result_p.content[i]);

    }

    return result_p;   
}

Picture resample_picture_nearest(Picture image, unsigned int width, unsigned int height){

    Picture result_p = create_picture(width, height, image.channels);

    double scale_x = (double)image.width/width;
    double scale_y = (double)image.height/height;
    
    for (unsigned int i=0; i < width; i++){
        for (unsigned int j=0; j < height; j++){
            //Calcul des coordonnées du pixel source le plus proche
            unsigned int prev_xi = (int)round(i * scale_x);
            unsigned int prev_yj = (int)round(j * scale_y);
            
            //Limiter les valeurs aux dimensions de l'image actuelle
            if (prev_xi >= image.width) { prev_xi = image.width - 1; }
            if (prev_yj >= image.width) { prev_yj = image.height - 1; }

            for (unsigned int c = 0; c < image.channels; c++) {

                unsigned int prev_index = (prev_yj * image.width + prev_xi) * image.channels + c;
                unsigned int new_index = (j * width + i) * image.channels + c;
                
                result_p.content[new_index] = image.content[prev_index];
            }
        }
        
    }
    return result_p;
}

Picture resample_picture_bilinear(Picture image, unsigned int width, unsigned int height){

    Picture result_p = create_picture(width, height, image.channels);

    double scale_x = (double)image.width / width;
    double scale_y = (double)image.height / height;


    for (unsigned int i=0; i < width; i++){
        for (unsigned int j=0; j < height; j++){
            double src_xi = i * scale_x;
            double src_yj = j * scale_y;

            //Indices des pixels autour
            unsigned int x1 = (unsigned int)floor(src_xi);
            unsigned int y1 = (unsigned int)floor(src_yj);
            unsigned int x2 = x1 + 1;
            unsigned int y2 = y1 + 1;

            //Ne pas depasser les bordures de l'image
            if (x2 >= image.width) { x2 = image.width - 1; }
            if (y2 >= image.height) { y2 = image.height - 1; }

            double alpha = src_xi - x1;
            double beta = src_yj - y1;

            for (unsigned int c = 0; c < image.channels; c++){
                //Indices des pixels autours du pixel source
                unsigned int next11_index = (y1 * image.width + x1) * image.channels + c;
                unsigned int next21_index = (y1 * image.width + x2) * image.channels + c;
                unsigned int next12_index = (y2 * image.width + x1) * image.channels + c;
                unsigned int next22_index = (y2 * image.width + x2) * image.channels + c;

                byte next11 = image.content[next11_index];
                byte next12 = image.content[next12_index];
                byte next21 = image.content[next21_index];
                byte next22 = image.content[next22_index];

                //On applique la formule d'interpolation-bilinéaire sur les pixels environnants pour obtenir le pixel resultant
                byte interpolated = round((1 - alpha)*(1 - beta)*next11 + alpha*(1 - beta)*next21 + (1 - alpha)*beta*next12 + alpha*beta*next22);

                //Index du pixel d'interpolation dans la nouvelle image
                unsigned int interpolated_index = (j*width + i)*image.channels + c;

                result_p.content[interpolated_index] = interpolated;
            }

        }
    }

    return result_p;

}

Picture brighten_picture_lut(Picture p, double factor){
    if (is_empty_picture(p)){
        return null_p;
    }
    //création de la LUT d'inverse
    Lut brighten_lut = create_lut(256);

    for (unsigned int i = 0; i < brighten_lut.n; i++) {
        int val = (int)round(i * factor); 
        val = arrange_pixel_value(val);
        brighten_lut.tab[i] = (byte)val;
    }
    
    Picture brightened_p = apply_lut(brighten_lut, p);
    clean_lut(brighten_lut);
    return brightened_p;
}



