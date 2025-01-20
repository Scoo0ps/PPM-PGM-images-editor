CC = gcc -Wall -Wextra -ansi -std=c99   # Options de compilation
LDFLAGS = -lm                          # Options de liaison (ajout de la bibliothèque mathématique)
OBJ = pictures.o pixels.o lut.o filename.o main.o

# Règle par défaut pour compiler le programme
all: projet

# Compilation des fichiers objets
pictures.o: pictures.h pictures.c
	$(CC) -c pictures.c -o pictures.o

pixels.o: pixels.h pixels.c
	$(CC) -c pixels.c -o pixels.o

lut.o: lut.h lut.c
	$(CC) -c lut.c -o lut.o

filename.o: filename.h filename.c
	$(CC) -c filename.c -o filename.o

main.o: main.c
	$(CC) -c main.c -o main.o

# Lien des fichiers objets pour créer l'exécutable
projet: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

# Règle de nettoyage
clean:
	rm -f $(OBJ) projet
