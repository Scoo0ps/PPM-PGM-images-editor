#include "filename.h"
#include <libgen.h> // for pattern matching
#include <stdlib.h> // for alloc funcs
#include <string.h>	// for strlen

/*
 * Extraction du chemin avant le nom depuis le path d'un fichier
 * @param [in] path le chemin complet du fichier
 * @return le répertoire du fichier
 */
char * dir_from_path(char * path)
{
	return dirname(strdup(path));
}

/**
 * Extrait une sous chaîne d'une chaîne
 * @param [in] string la chaîne dont on veut extraire une sous chaîne
 * @param [in] start l'index du début de l'extraction (start inclus)
 * @param [in] end l'index au delà de la fin de l'extraction (end exclus)
 * @return une sous-chaîne dynamiquement allouée contenant end - start + 1
 * caractères (avec le `\0` à la fin)
 */
char * substr(char * string, size_t start, size_t end)
{
	size_t len = strlen(string);
	if (start >= end || start > len || end > len)
	{
		return NULL;
	}
	char * result = calloc(end - start + 1, sizeof(char));
	for (size_t i = start, j = 0; i < end; i++, j++)
	{
		result[j] = string[i];
	}

	return result;
}

/**
 * Index de la dernière occurrence du caractère c dans la chaîne s
 * @param [in] s la chaîne s à explorer
 * @param [in] c le caractère à rechercher
 * @return l'index de la dernière occurrence du caractère c dans la chaîne s
 * ou bien -1 si aucune occurrence n'a été trouvée
 */
int last_index_of(char * s, char c)
{
	int index = -1;
	size_t i = 0;

	while (s[i] != '\0')
	{
		if (s[i] == c)
		{
			index = (int) i;
		}
		i++;
	}

	return index;
}

/*
 * Extraction du nom depuis le path d'un fichier
 * @param [in] path le chemin complet du fichier
 * @return le nom du fichier sans l'extension
 */
char * name_from_path(char * path)
{
	char * file_name = basename(path);
	int dot_index = last_index_of(file_name, '.');
	if (dot_index == -1)
	{
		return file_name;
	}
	return substr(file_name, 0, dot_index);
}

/*
 * Extraction de l'extension du fichier depuis le path d'un fichier
 * @param [in] path le chemin complet du fichier
 * @return l'extension du fichier ou bien NULL si path ne contient aucune
 * extension
 */
char * ext_from_path(char * path)
{
	char * file_name = basename(path);
	const size_t full_length = strlen(file_name);
	int dot_index = last_index_of(file_name, '.');
	if (dot_index == -1)
	{
		return NULL;
	}
	return substr(file_name, dot_index + 1, full_length);
}

/*
 * Concaténation des diverses parties d'un chemin pour composer un nouveau
 * chemin: <dir><name>_<op>.<ext>
 * @param [in] dir la partie répertoire du chemin
 * @param [in] name la partie nom du chemin
 * @param [in] op la partie opération du chemin
 * @param [in] ext la partie extension du chemin
 * @return une nouvelle chaîne dynamiquement allouée contenant le chemin
 * composé des différentes parties
 */
char * concat_parts(char * dir, char * name, char * op, char * ext)
{
	size_t total_length = strlen(dir);
    total_length += strlen(name);
    total_length += strlen(op);
    total_length += strlen(ext);
    total_length += 4; // for '/' + '_' + '.' + trailing '\0'
	char * result = (char *) calloc(total_length, sizeof(char));
	const char z = '\0';
	size_t i = 0;
	while (*dir != z)
	{
		result[i++] = *dir;
		dir++;
	}
    result[i++] = PATH_SEP;
	while (*name != z)
	{
		result[i++] = *name;
		name++;
	}
	result[i++] = '_';
	while (*op != z)
	{
		result[i++] = *op;
		op++;
	}
	result[i++] = '.';
	while (*ext != z)
	{
		result[i++] = *ext;
		ext++;
	}
	return result;
}
