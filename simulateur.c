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

void read(File *file); 
Directory *create_root(); 
int create(char *nom, char *contenu); 
void ls(Directory *dir); 




int main(void)
{
    return 0; 
}


Directory *create_root()
{
    Directory *root = malloc(sizeof(Directory));
    if(root == NULL)
    {
        return NULL; 
    }
    root->nom = malloc(strlen("root") + 1); 
    strcpy(root->nom, "root"); 
    
    root->fichiers = NULL; 
    root->nb_fichiers = 0; 
    root->sous_dossiers = NULL; 
    root->parent = NULL; 

    return root; 
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

Directory *mkdir(char *nom, Directory *parent)
{
    Directory *dir = malloc(sizeof(Directory)); 
    if (dir == NULL)
    {
        return NULL; 
    }

    dir->parent = parent; 


    if (parent != NULL)
    {
        parent->nb_sous_dossiers ++; 
        parent->sous_dossiers = realloc(parent->sous_dossiers,
                                        parent->nb_sous_dossiers * sizeof(Directory *)); 
                                        
        parent->sous_dossiers[parent->nb_sous_dossiers -1] = dir; 
    }
                                    
    dir->nb_sous_dossiers = 0; 
    dir->sous_dossiers = NULL;    
    
    dir->nb_fichiers = 0; 
    dir->fichiers = NULL; 

    dir->nom = malloc(sizeof(nom)); 
    strcpy(dir->nom, nom); 


    return dir; 
}


void ls(Directory *dir)
{

    int nb_subdir = dir->nb_sous_dossiers; 
    int nb_fichiers = dir->nb_fichiers; 

    if(nb_subdir !=0)
    {
        for (int i = 0; i < nb_subdir; i ++)
        {
            Directory **subdir = dir->sous_dossiers; 
            char *nom = subdir[i]->nom;
            
            printf("Dossier \t\t \t\t%s", nom); 
        }
    }

    if (nb_fichiers != 0)
    {
        for (int i = 0; i < nb_fichiers; i++)
        {
            File **files = dir->fichiers; 

            char *file_name = files[i]->nom; 
            BYTE taille = files[i]->taille; 
            printf("fichier \t\t%u \t\t%s",taille,file_name); 
        }
    } 
    return; 
}


void read(File *file)
{
    printf("%s",file->contenu);
    return; 
}


void mv() 
{
    
}