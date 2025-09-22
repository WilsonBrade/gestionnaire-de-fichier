#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define MAGENTA "\033[1;35m"
#define MAX_INPUT 256

typedef uint8_t BYTE;

typedef struct File {
    char *nom;
    char *contenu;
    BYTE taille;
} File;

typedef struct Directory {
    char *nom;
    File **fichiers;                 // tableau de pointeurs vers File
    int nb_fichiers;
    struct Directory **sous_dossiers; // tableau de pointeurs vers Directory
    int nb_sous_dossiers;
    struct Directory *parent;
} Directory;

/* prototypes */
Directory *create_root(void);
Directory *mkdir(char *nom, Directory *parent);
File *create_file(char *nom, char *contenu);
int add_file_to_dir(Directory *dir, File *file);
int remove_entry(Directory *dir, const char *name); // rm
void tree(Directory *dir, int level); 
void read_file(File *file);
void ls(Directory *dir);
void free_directory(Directory *dir);
void free_file(File *file);
void welcome(void);

/* ------------ main --------------- */
int main(void)
{
    welcome();
    Directory *root = create_root();
    Directory *current = root;

    char input[MAX_INPUT];

    while (1)
    {
        printf(">>> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) continue;
        input[strcspn(input, "\n")] = 0; // retirer \n

        if (strlen(input) == 0) continue;

        // quitter
        if (strcmp(input, "exit") == 0) {
            printf("Au revoir !\n");
            break;
        }

        // mkdir
        if (strncmp(input, "mkdir", 5) == 0) {
            char *nom = strtok(input + 6, " ");
            if (nom == NULL) {
                printf(RED "Usage : mkdir [nom]\n" RESET);
            } else {
                Directory *newdir = mkdir(nom, current);
                if (newdir) {
                    printf(GREEN "Dossier '%s' créé avec succès\n" RESET, nom);
                } else {
                    printf(RED "Erreur création dossier\n" RESET);
                }
            }
        }

        // ls
        else if (strcmp(input, "ls") == 0) {
            ls(current);
        }

        // touch
        else if (strncmp(input, "touch", 5) == 0) {
            char *nom = strtok(input + 6, " ");
            if (nom == NULL) {
                printf(RED "Usage : touch [nom]\n" RESET);
            } else {
                File *f = create_file(nom, ""); // fichier vide
                if (f == NULL) {
                    printf(RED "Erreur création fichier\n" RESET);
                } else {
                    if (add_file_to_dir(current, f) == 0) {
                        printf(GREEN "Fichier '%s' créé avec succès\n" RESET, nom);
                    } else {
                        printf(RED "Erreur ajout fichier au répertoire\n" RESET);
                        free_file(f);
                    }
                }
            }
        }
        // tree
        else if (strcmp(input, "tree") == 0) {
            tree(current, 0);
        }

        // rm
        else if (strncmp(input, "rm", 2) == 0) {
            char *nom = strtok(input + 3, " ");
            if (nom == NULL) {
                printf(RED "Usage : rm [nom]\n" RESET);
            } else {
                if (remove_entry(current, nom)) {
                    printf(GREEN "✅ '%s' supprimé\n" RESET, nom);
                } else {
                    printf(RED "'%s' introuvable\n" RESET, nom);
                }
            }
        }

        // commande inconnue
        else {
            printf(RED "Commande inconnue : %s\n" RESET, input);
        }
    }

    free_directory(root);
    return 0;
}

/* ------------ implémentations --------------- */

Directory *create_root(void)
{
    Directory *root = malloc(sizeof(Directory));
    if (root == NULL) return NULL;

    root->nom = malloc(strlen("root") + 1);
    strcpy(root->nom, "root");

    root->fichiers = NULL;
    root->nb_fichiers = 0;
    root->sous_dossiers = NULL;
    root->nb_sous_dossiers = 0;
    root->parent = NULL;

    return root;
}

File *create_file(char *nom, char *contenu)
{
    if (nom == NULL || contenu == NULL) return NULL;

    File *file = malloc(sizeof(File));
    if (file == NULL) {
        return NULL;
    }

    file->nom = malloc(strlen(nom) + 1);
    if (file->nom == NULL) { free(file); return NULL; }
    strcpy(file->nom, nom);

    file->contenu = malloc(strlen(contenu) + 1);
    if (file->contenu == NULL) { free(file->nom); free(file); return NULL; }
    strcpy(file->contenu, contenu);

    file->taille = (BYTE) strlen(contenu);
    return file;
}

int add_file_to_dir(Directory *dir, File *file)
{
    if (dir == NULL || file == NULL) return -1;

    int new_n = dir->nb_fichiers + 1;
    File **tmp = realloc(dir->fichiers, new_n * sizeof(File *));
    if (tmp == NULL) {
        return -1; // échec allocation
    }
    dir->fichiers = tmp;
    dir->fichiers[dir->nb_fichiers] = file;
    dir->nb_fichiers = new_n;
    return 0;
}

Directory *mkdir(char *nom, Directory *parent)
{
    if (nom == NULL) return NULL;

    Directory *dir = malloc(sizeof(Directory));
    if (dir == NULL) return NULL;

    dir->nom = malloc(strlen(nom) + 1);
    if (dir->nom == NULL) { free(dir); return NULL; }
    strcpy(dir->nom, nom);

    dir->parent = parent;
    dir->nb_sous_dossiers = 0;
    dir->sous_dossiers = NULL;
    dir->nb_fichiers = 0;
    dir->fichiers = NULL;

    if (parent != NULL)
    {
        int new_n = parent->nb_sous_dossiers + 1;
        Directory **tmp = realloc(parent->sous_dossiers, new_n * sizeof(Directory *));
        if (tmp == NULL) {
            // si échec, on nettoie et on retourne NULL
            free(dir->nom);
            free(dir);
            return NULL;
        }
        parent->sous_dossiers = tmp;
        parent->sous_dossiers[parent->nb_sous_dossiers] = dir;
        parent->nb_sous_dossiers = new_n;
    }

    return dir;
}

void ls(Directory *dir)
{
    if (dir == NULL) return;

    int nb_subdir = dir->nb_sous_dossiers;
    int nb_fichiers = dir->nb_fichiers;

    if (nb_subdir != 0)
    {
        for (int i = 0; i < nb_subdir; i++)
        {
            char *nom = dir->sous_dossiers[i]->nom;
            printf("Dossier\t\t%s\n", nom);
        }
    }

    if (nb_fichiers != 0)
    {
        for (int i = 0; i < nb_fichiers; i++)
        {
            char *file_name = dir->fichiers[i]->nom;
            BYTE taille = dir->fichiers[i]->taille;
            printf("Fichier\t\t%s\t%u\n", file_name, (unsigned)taille);
        }
    }
    return;
}

void read_file(File *file)
{
    if (file == NULL) return;
    printf("%s", file->contenu);
    return;
}

void free_file(File *file)
{
    if (file == NULL) return;
    free(file->nom);
    free(file->contenu);
    free(file);
    return;
}

/* remove_entry : supprime un fichier ou un sous-dossier du répertoire donné.
   Retourne 1 si suppression faite, 0 si introuvable, -1 en cas d'erreur. */
int remove_entry(Directory *dir, const char *name)
{
    if (dir == NULL || name == NULL) return 0;

    // --- Cherche dans les fichiers ---
    for (int i = 0; i < dir->nb_fichiers; i++) {
        if (strcmp(dir->fichiers[i]->nom, name) == 0) {
            File *tofree = dir->fichiers[i];

            // décale les pointeurs suivants vers la gauche
            if (dir->nb_fichiers - i - 1 > 0) {
                memmove(&dir->fichiers[i], &dir->fichiers[i+1],
                        (dir->nb_fichiers - i - 1) * sizeof(File *));
            }
            dir->nb_fichiers--;

            if (dir->nb_fichiers == 0) {
                free(dir->fichiers);
                dir->fichiers = NULL;
            } else {
                File **tmp = realloc(dir->fichiers, dir->nb_fichiers * sizeof(File *));
                if (tmp) dir->fichiers = tmp;
                // si tmp == NULL on garde l'ancien bloc (taille plus grande), encore valide
            }

            free_file(tofree);
            return 1;
        }
    }

    // --- Cherche dans les sous-dossiers ---
    for (int i = 0; i < dir->nb_sous_dossiers; i++) {
        if (strcmp(dir->sous_dossiers[i]->nom, name) == 0) {
            Directory *tofree = dir->sous_dossiers[i];

            // décale les pointeurs suivants vers la gauche
            if (dir->nb_sous_dossiers - i - 1 > 0) {
                memmove(&dir->sous_dossiers[i], &dir->sous_dossiers[i+1],
                        (dir->nb_sous_dossiers - i - 1) * sizeof(Directory *));
            }
            dir->nb_sous_dossiers--;

            if (dir->nb_sous_dossiers == 0) {
                free(dir->sous_dossiers);
                dir->sous_dossiers = NULL;
            } else {
                Directory **tmp = realloc(dir->sous_dossiers, dir->nb_sous_dossiers * sizeof(Directory *));
                if (tmp) dir->sous_dossiers = tmp;
            }

            // libération récursive du sous-dossier
            free_directory(tofree);
            return 1;
        }
    }

    return 0; // introuvable
}

void free_directory(Directory *dir)
{
    if (dir == NULL) return;

    // Libérer tous les fichiers
    for (int i = 0; i < dir->nb_fichiers; i++) {
        File *f = dir->fichiers[i];
        if (f != NULL) free_file(f);
    }
    free(dir->fichiers);

    // Libérer récursivement les sous-dossiers
    for (int i = 0; i < dir->nb_sous_dossiers; i++) {
        Directory *subdir = dir->sous_dossiers[i];
        free_directory(subdir);
    }
    free(dir->sous_dossiers);

    // Libérer le nom du dossier
    free(dir->nom);

    // Enfin libérer la structure elle-même
    free(dir);
}


void tree(Directory *dir, int level) 
{
    if (dir == NULL) return;

    // Indentation
    for (int i = 0; i < level; i++) {
        printf("   ");
    }
    printf("%s\n", dir->nom);

    // Afficher les fichiers du dossier
    for (int i = 0; i < dir->nb_fichiers; i++) {
        for (int j = 0; j < level + 1; j++) {
            printf("   ");
        }
        printf("%s (taille: %u)\n", dir->fichiers[i]->nom, dir->fichiers[i]->taille);
    }

    // Appel récursif pour les sous-dossiers
    for (int i = 0; i < dir->nb_sous_dossiers; i++) {
        tree(dir->sous_dossiers[i], level + 1);
    }
}


void welcome(void) {
    printf(CYAN "=============================================\n" RESET);
    printf(GREEN "   Bienvenue sur ton simulateur de fichiers \n" RESET);
    printf(CYAN "=============================================\n" RESET);

    printf(YELLOW " Commandes disponibles :\n" RESET);
    printf(BLUE   "   - mkdir [nom]   " RESET CYAN ": creer un dossier\n" RESET);
    printf(BLUE   "   - touch [nom]   " RESET CYAN ": creer un fichier\n" RESET);
    printf(BLUE   "   - ls            " RESET CYAN ": lister le contenu du dossier\n" RESET);
    printf(BLUE   "   - rm [nom]      " RESET CYAN ": supprimer fichier ou dossier\n" RESET);
    printf(BLUE   "   - mv [src] [dst]" RESET CYAN ": deplacer un fichier ou dossier (à implémenter)\n" RESET);
    printf(BLUE   "   - exit          " RESET CYAN ": quitter le simulateur\n" RESET);

    printf(CYAN "=============================================\n\n" RESET);
}
