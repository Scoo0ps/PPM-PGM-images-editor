#include <stdio.h>
#include <stdint.h>
#include "pixels.h"
#include "filename.h"

#ifndef PICTURES_H
#define PICTURES_H

typedef struct {
    unsigned int width, height;
    unsigned channels;
    byte* content; 
} Picture;


/*@requires hauteur, largeur et canal de l'image que l'on veut initialiser
@assigns rien
@ensures succès : retourne la nouvelle image crée ; sinon échec : retourne NULL*/
Picture create_picture(unsigned int, unsigned int, unsigned int);

/*@requires path d'un fichier image
@assigns rien
@ensures succès : l'image est maintenant retranscrite par une structure de type Picture contenant toutes les informations de l'image ; sinon échec : une structure de type Picture vide est retounée.*/
Picture read_picture(char*);

/*@requires une image à copier
@assigns rien
@ensures succès : l'image est libérée et ses champs sont rénitialisés ; échec : l'image source a un contenu nul et une image nulle est retournée*/
Picture copy_picture(Picture);

/*@requires le path d'un fichier
@assigns rien
@ensures succès : retourne 0 , le fichier passé en argument est régulier + il a une extension d'image valide (.pgm ou .ppm) ; sinon échec : retourne 1*/
int is_file_picture(char*);

/*@requires une image, le path du futur fichier image
@assigns rien
@ensures succès : retourne 0 , le fichier a correctement été écrit dans le path passé en paramètre ; sinon échec : retourne un entier différent de 0*/
int write_picture(Picture, char*);

/*@requires une image
@assigns rien
@ensures succès : l'image a un des champs nul, l'image est vide et 1 est retourné ; échec : 0 sinon*/
int is_empty_picture(Picture);

/*@requires une image à inspecter
@assigns rien
@ensures succès : l'image est en niveaux de gris et 1 est retourné ; 0 sinon*/
int is_gray(Picture);

/*@requires une image à inspecter
@assigns rien
@ensures succès : l'image est en couleurs et 1 est retourné ; 0 sinon*/
int is_color(Picture);

/*@requires 2 images
@assigns rien
@ensures succès : est renvoyée 1 si les 2 images ont même hauteur et largeur ; échec : est renvoyée 0 */
int have_same_w_and_h(Picture, Picture);

/*@requires une image à copier
@assigns rien
@ensures succès : une copie de l'image passée en paramètre est renvoyée ; échec : on retourne une image nulle*/
Picture copy_picture(Picture);

/*@requires une image à convertir
@assigns rien
@ensures succès : l'image est convertie en couleur ; échec : on retourne une image nulle*/
Picture convert_to_color_picture(Picture);

/*@requires une image à convertir
@assigns rien
@ensures succès : l'image est convertie en niveaux de gris ; échec : on retourne une image nulle*/
Picture convert_to_gray_picture(Picture);

/*@requires une image
@assigns rien
@ensures succès : est renvoyé un tableau de 3 images content les valeurs R, V et B | si l'image est en niveaux de gris est renvoyé un tableau contenant elle-même ; échec : on retourne NULL*/
Picture* split_picture(Picture);

/*@requires trois images en niveau de gris
@assigns rien
@ensures succès : est renvoyée une image en couleur issue de l'union des 3 images en niveau de gris ; échec : est renvoyée une image nulle*/
Picture merge_picture(Picture, Picture, Picture);

/*@requires une image et un facteur d'éclaississement >= 1 
@assigns rien
@ensures succès : est renvoyée une image éclaircie en fonction du facteur ; échec : est renvoyée une image nulle*/
Picture brighten_picture(Picture, double);

/*@requires une image et un nombre positif
@assigns rien
@ensures succès : est renvoyée une image "fondue vers le bas" si cela est possible (i.e n>=1) ; échec : est renvoyée une image nulle ou l'image en elle même (cas n=0)*/
Picture melt_picture(Picture, int);

/*@requires une image
@assigns rien
@ensures succès : est renvoyée une image inversée ; échec : est renvoyée une image nulle */
Picture inverse_picture(Picture p);

/*@requires une image
@assigns rien
@ensures succès : est renvoyée une image normalisée dont les pixels ont été normalisés ; échec : est renvoyée une image nulle */
Picture normalize_dynamic_picture(Picture);

/*@requires une image et un nombre de niveaux compris entre 2 et 256
@assigns rien
@ensures succès : est renvoyée une image où le nombre de niveaux a été réduit en nb_levels ; échec : est renvoyée une image nulle */
Picture set_levels_picture(Picture, byte);

/*@requires 2 images
@assigns rien
@ensures succès : est renvoyée une image normalisée et en niveaux de gris résultant de la différence (en valeur absolue) de p1 et p2 ; échec : est renvoyée une image nulle */
Picture difference_picture(Picture, Picture);

/*@requires Picture à nettoyer
@assigns rien
@ensures succès : l'image est nétoyée et ses champs sont rénitialisés*/
void clean_picture(Picture);

/*@requires une image
@assigns rien
@ensures rien*/
void info_picture(Picture img);

/*@requires une image, une nouvelle hauteur et une nouvelle largeur
@assigns rien
@ensures succès : est renvoyée une image redimensionnée à la largeur et hauteur passées en paramètre en utilisant la méthode des plus proches voisins ; échec : est renvoyée une image nulle*/
Picture resample_picture_nearest(Picture, unsigned int, unsigned int);

/*@requires une image, une nouvelle hauteur et une nouvelle largeur
@assigns rien
@ensures succès : est renvoyée une image redimensionnée à la largeur et hauteur passées en paramètre en utilisant d’interpolation bi-linéaire ; échec : est renvoyée une image nulle*/
Picture resample_picture_bilinear(Picture, unsigned int, unsigned int);

/*@requires deux images
@assigns rien
@ensures succès : est renvoyée une image, résultat du produit des deux images passées en paramètre ; échec : est renvoyée une image nulle*/
Picture mult_picture(Picture, Picture);

/*@requires trois images : les 2 premières sont les images à mélanger, la 3eme est l'image qui va servir de masque
@assigns rien
@ensures succès : est renvoyée une image, résultat du mélange des deux premières images passées en paramètre suivant la 3eme image ; échec : est renvoyée une image nulle*/
Picture mix_picture(Picture, Picture, Picture);

/*@requires une image et un facteur d'éclaississement >= 1 (sinon l'image sera assombrie)
@assigns rien
@ensures succès : est renvoyée une image éclaircie du paramètre d'éclaircissement ; échec : est renvoyée une image nulle*/
Picture brighten_picture_lut(Picture, double);

/*@requires le path d'une image
@assigns rien
@ensures l'image est bien une image en .pgm (renvoie 1) ; sinon 0*/
int is_file_pgm(char*);


/*@requires le path d'une image
@assigns rien
@ensures l'image est bien une image en .ppm (renvoie 1) ; sinon 0*/
int is_file_ppm(char*);

/*@requires le pointeur d'un fichier
@assigns le pointeur passé en paramètre
@ensures le pointeur pointe désormais vers la prochaine partie du fichier sans commentaire ; sinon rien*/
void checkforhashtags(FILE* fptr);

#endif