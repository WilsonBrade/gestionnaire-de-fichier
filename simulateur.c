#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t BYTE; 

typedef struct File 
{
    char *nom; 
    char *contenu; 
    BYTE taille; 
} File; 


typedef struct Directory {
    char *nom;
    File **fichiers;           // tableau de pointeurs vers File
    int nb_fichiers;
    struct Directory **sous_dossiers; // tableau de pointeurs vers Directory
    int nb_sous_dossiers;
    struct Directory *parent;
} Directory;


int main(void)
{
    return 0; 
}


void create_root()
{
    Directory root;
    strcpy(root.nom, "root"); 
    root.fichiers = NULL; 
    root.nb_fichiers = 0; 
    root.sous_dossiers = NULL; 
    root.parent = NULL; 
}


int create(char *nom, char *contenu)
{
    BYTE taille = strlen(contenu); 
    File *file = malloc(sizeof(File));
    if (file == NULL)
    {
        return 1; 
    } 

    file->nom = malloc(strlen(nom) + 1); 
    strcpy(file-> nom, nom); 

    file-> contenu = malloc(strlen(contenu) + 1); 
    strcpy(file->contenu, contenu); 
    file->taille = taille; 

    return 0; 
}

void mkdir()
{

}
