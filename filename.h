/*
 * filename.h
 * Contient des fonctions pour décomposer un nom de fichier
 * <dirname>/<name>.<ext> en:
 *  - dirname = chemin avant le nom du fichier
 *  - name = nom du fichier : <dirname>/<name>.<ext>
 *  - ext = l'extension du fichier
 */

#ifndef __FILENAME__
#define __FILENAME__

#ifdef _WIN32
    #define PATH_SEP '\\'
#else
    #define PATH_SEP '/'
#endif

/**
 * Extraction du chemin avant le nom depuis le path d'un fichier
 * @param [in] path le chemin complet du fichier
 * @return le répertoire du fichier
 */
char * dir_from_path(char * path);

/**
 * Extraction du nom depuis le path d'un fichier
 * @param [in] path le chemin complet du fichier
 * @return le nom du fichier sans l'extension
 */
char * name_from_path(char * path);

/**
 * Extraction de l'extension du fichier depuis le path d'un fichier
 * @param [in] path le chemin complet du fichier
 * @return l'extension du fichier ou bien NULL si path ne contient aucune
 * extension
 */
char * ext_from_path(char * path);

/**
 * Concaténation des diverses parties d'un chemin pour composer un nouveau
 * chemin: <dir><name>_<op>.<ext>
 * @param [in] dir la partie répertoire du chemin
 * @param [in] name la partie nom du chemin
 * @param [in] op la partie opération du chemin
 * @param [in] ext la partie extension du chemin
 * @return une nouvelle chaîne dynamiquement allouée contenant le chemin
 * composé des différentes parties
 */
char * concat_parts(char * dir, char * name, char * op, char * ext);

#endif  // __FILENAME__