#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pictures.h"
#include "lut.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: need 2 image files - lenna_gray and lenna_color ; got %d \n", argc - 1);
        return 1;
    }

    char* first_filename = argv[1];
    char* second_filename = argv[2];

    char* lenna_gray_filename;
    char* lenna_color_filename;

    if (is_file_pgm(first_filename) && is_file_ppm(second_filename)) {
        lenna_gray_filename = first_filename;
        lenna_color_filename = second_filename;
    }
    else if (is_file_ppm(first_filename) && is_file_pgm(second_filename)) {
        lenna_gray_filename = second_filename;
        lenna_color_filename = first_filename;
    }
    else {
        fprintf(stderr, "Usage: incorrect format from image files : need lenna_gray (.pgm) and lenna_color (.ppm) ; got %s and %s \n", first_filename, second_filename);
        return 1;
    }

    printf("Chargé %s en tant que Lenna_gray\n", lenna_gray_filename);
    printf("Chargé %s  en tant que Lenna_color\n\n", lenna_color_filename);

    printf("Les images suivantes seront sauvegardées dans le répertoire courant\n\n");

    Picture lenna_gray = read_picture(lenna_gray_filename);
    Picture lenna_color = read_picture(lenna_color_filename);

    Picture lenna_gray_convert_color = convert_to_color_picture(lenna_gray);
    Picture lenna_color_convert_gray = convert_to_gray_picture(lenna_color);

    Picture* lenna_color_splitted = split_picture(lenna_color);
    Picture lenna_color_red = lenna_color_splitted[0];
    Picture lenna_color_green = lenna_color_splitted[1];
    Picture lenna_color_blue = lenna_color_splitted[2];

    double brighten_factor = 1.5;

    Picture lenna_color_brightened = brighten_picture(lenna_color, brighten_factor);
    Picture lenna_gray_brightened = brighten_picture(lenna_gray, brighten_factor);

    Picture lenna_color_melted = melt_picture(lenna_color, lenna_color.width*lenna_color.height*lenna_color.channels);
    Picture lenna_gray_melted = melt_picture(lenna_gray, lenna_gray.width*lenna_gray.height*lenna_gray.channels);

    Picture lenna_color_inverse = inverse_picture(lenna_color);
    Picture lenna_gray_inverse = inverse_picture(lenna_gray);

    Picture lenna_gray_dynamic = normalize_dynamic_picture(lenna_gray);
    Picture lenna_color_dynamic = normalize_dynamic_picture(lenna_color);

    //LEVELS NE FONCTIONNE PAS VRAIMENT
    int levels = 9;
    Picture lenna_gray_levels = set_levels_picture(lenna_gray, levels);
    Picture lenna_color_levels = set_levels_picture(lenna_color, levels);

    double resize_factor = 1.36;

    unsigned int gray_smaller_width = (unsigned int)round(lenna_gray.width / resize_factor);
    unsigned int gray_larger_width = (unsigned int)round(lenna_gray.width * resize_factor);
    unsigned int gray_smaller_height = (unsigned int)round(lenna_gray.height / resize_factor);
    unsigned int gray_larger_height = (unsigned int)round(lenna_gray.height * resize_factor);

    unsigned int color_smaller_width = (unsigned int)round(lenna_color.width / resize_factor);
    unsigned int color_larger_width = (unsigned int)round(lenna_color.width * resize_factor);
    unsigned int color_smaller_height = (unsigned int)round(lenna_color.height / resize_factor);
    unsigned int color_larger_height = (unsigned int)round(lenna_color.height * resize_factor);

    Picture lenna_gray_smaller_nearest = resample_picture_nearest(lenna_gray, gray_smaller_width, gray_smaller_height);
    Picture lenna_gray_larger_nearest = resample_picture_nearest(lenna_gray, gray_larger_width, gray_larger_height);

    Picture lenna_color_smaller_nearest = resample_picture_nearest(lenna_color, color_smaller_width, color_smaller_height);
    Picture lenna_color_larger_nearest = resample_picture_nearest(lenna_color, color_larger_width, color_larger_height);

    Picture lenna_gray_smaller_bilinear = resample_picture_bilinear(lenna_gray, gray_smaller_width, gray_smaller_height);
    Picture lenna_gray_larger_bilinear = resample_picture_bilinear(lenna_gray, gray_larger_width, gray_larger_height);

    Picture lenna_color_smaller_bilinear = resample_picture_bilinear(lenna_color, color_smaller_width, color_smaller_height);
    Picture lenna_color_larger_bilinear = resample_picture_bilinear(lenna_color, color_larger_width, color_larger_height);

    Picture lenna_gray_difference = normalize_dynamic_picture(difference_picture(lenna_gray_larger_nearest, lenna_gray_larger_bilinear));
    Picture lenna_color_difference = normalize_dynamic_picture(difference_picture(lenna_color_larger_nearest, lenna_color_larger_bilinear));

    Picture lenna_BW = read_picture("Lenna_BW.pgm");

    Picture lenna_gray_product = mult_picture(lenna_gray, lenna_BW);
    Picture lenna_color_product = mult_picture(lenna_color, lenna_BW);

    Picture lenna_gray_mixture = mix_picture(lenna_gray_inverse, lenna_gray, lenna_BW);
    Picture lenna_color_mixture = mix_picture(lenna_color_inverse, lenna_color, lenna_BW);

    Picture lenna_gray_brightened_lut = brighten_picture_lut(lenna_gray, brighten_factor);
    Picture lenna_color_brightened_lut = brighten_picture_lut(lenna_color, brighten_factor);


    write_picture(lenna_gray_convert_color, "Lenna_gray_convert_color.ppm");
    write_picture(lenna_color_convert_gray, "Lenna_color_convert_gray.pgm");

    write_picture(lenna_color_red, "Lenna_color_red.pgm");
    write_picture(lenna_color_green, "Lenna_color_green.pgm");
    write_picture(lenna_color_blue, "Lenna_color_blue.pgm");

    write_picture(lenna_color_brightened, "Lenna_color_brightened.ppm");
    write_picture(lenna_gray_brightened, "Lenna_gray_brightened.pgm");

    write_picture(lenna_color_melted, "Lenna_color_melted.ppm");
    write_picture(lenna_gray_melted, "Lenna_gray_melted.pgm");

    write_picture(lenna_color_inverse, "Lenna_color_inverse.ppm");
    write_picture(lenna_gray_inverse, "Lenna_gray_inverse.pgm");

    write_picture(lenna_gray_dynamic, "Lenna_gray_dynamic.pgm");
    write_picture(lenna_color_dynamic, "Lenna_color_dynamic.ppm");

    write_picture(lenna_gray_levels, "Lenna_gray_levels.pgm");
    write_picture(lenna_color_levels, "Lenna_color_levels.ppm");

    write_picture(lenna_gray_smaller_nearest, "Lenna_gray_smaller_nearest.pgm");
    write_picture(lenna_gray_larger_nearest, "Lenna_gray_larger_nearest.pgm");
    write_picture(lenna_color_smaller_nearest, "Lenna_color_smaller_nearest.ppm");
    write_picture(lenna_color_larger_nearest, "Lenna_color_larger_nearest.ppm");

    write_picture(lenna_gray_smaller_bilinear, "Lenna_gray_smaller_bilinear.pgm");
    write_picture(lenna_gray_larger_bilinear, "Lenna_gray_larger_bilinear.pgm");
    write_picture(lenna_color_smaller_bilinear, "Lenna_color_smaller_bilinear.ppm");
    write_picture(lenna_color_larger_bilinear, "Lenna_color_larger_bilinear.ppm");

    write_picture(lenna_gray_difference, "Lenna_gray_difference.pgm");
    write_picture(lenna_color_difference, "Lenna_color_difference.ppm");

    write_picture(lenna_gray_product, "Lenna_gray_product.pgm");
    write_picture(lenna_color_product, "Lenna_color_product.ppm");

    write_picture(lenna_gray_mixture, "Lenna_gray_mixture.pgm");
    write_picture(lenna_color_mixture, "Lenna_color_mixture.ppm");

    write_picture(lenna_gray_brightened_lut, "Lenna_gray_brightened_lut.pgm");
    write_picture(lenna_color_brightened_lut, "Lenna_color_brightened_lut.ppm");



    // Nettoyer les images après utilisation
    clean_picture(lenna_gray);
    clean_picture(lenna_color);
    clean_picture(lenna_gray_convert_color);
    clean_picture(lenna_color_convert_gray);
    clean_picture(lenna_color_red);
    clean_picture(lenna_color_green);
    clean_picture(lenna_color_blue);
    clean_picture(lenna_color_brightened);
    clean_picture(lenna_gray_brightened);
    clean_picture(lenna_color_melted);
    clean_picture(lenna_gray_melted);
    clean_picture(lenna_color_inverse);
    clean_picture(lenna_gray_inverse);
    clean_picture(lenna_gray_dynamic);
    clean_picture(lenna_color_dynamic);
    clean_picture(lenna_gray_levels);
    clean_picture(lenna_color_levels);
    clean_picture(lenna_gray_smaller_nearest);
    clean_picture(lenna_gray_larger_nearest);
    clean_picture(lenna_color_smaller_nearest);
    clean_picture(lenna_color_larger_nearest);
    clean_picture(lenna_gray_smaller_bilinear);
    clean_picture(lenna_gray_larger_bilinear);
    clean_picture(lenna_color_smaller_bilinear);
    clean_picture(lenna_color_larger_bilinear);
    clean_picture(lenna_gray_difference);
    clean_picture(lenna_color_difference);
    clean_picture(lenna_BW);
    clean_picture(lenna_gray_product);
    clean_picture(lenna_color_product);
    clean_picture(lenna_gray_mixture);
    clean_picture(lenna_color_mixture);
    clean_picture(lenna_gray_brightened_lut);
    clean_picture(lenna_color_brightened_lut);

    // Libérer la mémoire allouée pour le tableau de couleurs séparées
    free(lenna_color_splitted);

    return 0;
}