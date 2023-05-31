//
// Created by tobby on 30/05/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "fonction.h"
#define MAX_FILENAME_LENGTH 512
#define MAX_LINE_LENGTH 512
char* fichier_log_f = "log/log.txt";


void copier_fichiers(const char *dossierSource, const char *dossierDestination) {
    // Ouvrir le dossier source
    DIR *dirSource = opendir(dossierSource);
    if (dirSource == NULL) {
        printf("Impossible d'ouvrir le dossier source.\n");
        return;
    }

    // Créer le dossier destination s'il n'existe pas
    mkdir(dossierDestination, 0777);

    struct dirent *ent;

    // Lire tous les fichiers du dossier source
    while ((ent = readdir(dirSource)) != NULL) {
        if (ent->d_type == DT_REG) {
            // Construire les chemins complets pour le fichier source et le fichier destination
            char cheminSource[PATH_MAX];
            char cheminDestination[PATH_MAX];
            sprintf(cheminSource, "%s/%s", dossierSource, ent->d_name);
            sprintf(cheminDestination, "%s/%s", dossierDestination, ent->d_name);

            // Ouvrir le fichier source en lecture binaire
            FILE *fichierSource = fopen(cheminSource, "rb");
            if (fichierSource == NULL) {
                printf("Impossible d'ouvrir le fichier source : %s\n", cheminSource);
                continue;
            }

            // Ouvrir le fichier destination en écriture binaire
            FILE *fichierDestination = fopen(cheminDestination, "wb");
            if (fichierDestination == NULL) {
                printf("Impossible d'ouvrir le fichier destination : %s\n", cheminDestination);
                fclose(fichierSource);
                continue;
            }

            // Copier les données du fichier source vers le fichier destination
            int caractere;
            while ((caractere = fgetc(fichierSource)) != EOF) {
                fputc(caractere, fichierDestination);
            }

            // Fermer les fichiers source et destination
            fclose(fichierSource);
            fclose(fichierDestination);

            printf("Fichier copié : %s\n", ent->d_name);
        }
    }

    // Fermer le dossier source
    closedir(dirSource);
}


int save_data_log(const char *fichier_log, const char *data){
    FILE* fichier = NULL;
    time_t temps_actuel;
    struct tm *heure_locale;
    // Obtenir le temps actuel
    temps_actuel = time(NULL);
    fichier = fopen(fichier_log, "a");
    if(fichier == NULL){
        printf("Erreur lors de l'ouverture de fichier\n.");
        return EXIT_FAILURE;
    }else{
        fprintf(fichier, "%s\t", data);
        // Récupérer la date de modification du fichier
        char dateTimeString[MAX_LINE_LENGTH];
        struct tm *modificationTime = localtime(&temps_actuel);
        strftime(dateTimeString, sizeof(dateTimeString), "%Y-%m-%d %H:%M:%S", modificationTime);

        fprintf(fichier, "--- %s\n",dateTimeString);
    }

    fclose(fichier);

    return EXIT_SUCCESS;
}


FILE* synchroList(const char* file1, const char* file2) {

    FILE* fp1 = fopen(file1, "r");

    FILE* fp2 = fopen(file2, "r");

    printf("%s\n",file1);
    printf("%s\n",file2);

    if (fp1 == NULL || fp2 == NULL ) {

        printf("Erreur lors de l'ouverture des fichiers.\n");
        save_data_log(fichier_log_f, "Module Synchro : Erreur lors de l'ouverture des deux fichiers.");
        return NULL;

    }

    char line1[256];

    char line2[256];

    int found;

    // Create the output file

    FILE* fpOutput = fopen("listeAcopier.txt", "w");

    if (fpOutput == NULL) {

        printf("Erreur lors de la création du fichier de sortie.\n");
        save_data_log(fichier_log_f, "Module Synchro : Erreur lors de la création du fichier de sortie.");
        fclose(fp1);

        fclose(fp2);

        return NULL;
    }

    // Parcourir chaque ligne du premier fichier

    while (fgets(line1, sizeof(line1), fp1) != NULL) {

        found = 0;

        // Parcourir chaque ligne du deuxième fichier

        rewind(fp2); // Réinitialiser la position du curseur au début du fichier

        while (fgets(line2, sizeof(line2), fp2) != NULL) {

            // Comparer les lignes des deux fichiers (ignorer les sauts de ligne)

            if (strcmp(line1, line2) == 0) {

                found = 1;

                break;

            }

        }

        // Si la ligne n'a pas été trouvée dans le deuxième fichier, l'écrire dans le fichier de sortie

        if (!found) {

            fputs(line1, fpOutput);
        }
    }
    // Fermer les fichiers

    fclose(fp1);

    fclose(fp2);

    fclose(fpOutput);

    printf("Comparaison terminée. Le résultat a été écrit dans le fichier listeAcopier\n");
    save_data_log(fichier_log_f, "Module Synchro : Comparaison terminée. Le résultat a été écrit dans le fichier listeAcopier.");

    return fpOutput;

}


int copy_list(char* nomfichier, char*nomdossiersource, char* nomdossierdestination){

    FILE* f = NULL ;
    char ligne[50] ;
    char nom_fic[50] ;

    f = fopen(nomfichier, "r");

    if(f == NULL){
        printf("Erreur lors de l'ouverture du fichier : %s\n", nomfichier);
        save_data_log(fichier_log_f, "Module Copy : Erreur lors de l'ouverture du fichier %s.");
        return 1;
    }

    while(fgets(ligne, 50, f)){

        ligne[strcspn(ligne, "\n")] = '\0';

        sscanf(ligne, "%s", nom_fic);

        printf("%s\n", nom_fic);
        save_data_log(fichier_log_f,  "Module Copy : copie d'un fichier. ");
        if(copier_fichier_vers_dossier(nom_fic, nomdossiersource, nomdossierdestination) != 0){
            return 0 ;
        }
    }

    return 0 ;
}

int copier_fichier_vers_dossier(char* nomfichier, char* nomdossiersource, char* nomdossierdestination){

    FILE* fsource = NULL ;
    FILE* fdestination = NULL ;
    char c ;

    char* nom_fichier_source = malloc(sizeof(char)*strlen(nomfichier) + sizeof(char)*strlen(nomdossiersource) + 1);
    char* nom_fichier_destination = malloc(sizeof(char)*strlen(nomfichier) + sizeof(char)*strlen(nomdossierdestination) + 1);

    strcpy(nom_fichier_source, nomdossiersource);
    strcat(nom_fichier_source, "/");
    strcat(nom_fichier_source, nomfichier);

    strcpy(nom_fichier_destination, nomdossierdestination);
    strcat(nom_fichier_destination, "/");
    strcat(nom_fichier_destination, nomfichier);

    printf("Nom du fichier => %s\n", nomfichier);

    fsource = fopen(nom_fichier_source, "r") ;

    if(fsource == NULL){
        printf("Erreur lors de l'ouverture du fichier source : %s\n", nom_fichier_source);
        save_data_log(fichier_log_f, "Module copy: Erreur lors de l'ouverture du fichier source.");
        return 1 ;
    }

    fdestination = fopen(nom_fichier_destination, "w") ;

    if(fdestination == NULL){
        printf("Erreur lors de l'ouverture du fichier destination : %s\n", nom_fichier_destination);
        save_data_log(fichier_log_f, "Module copy: Erreur lors de l'ouverture de du fichier destination.");
        return 1 ;
    }

    while((c = fgetc(fsource)) != EOF){
        fputc(c, fdestination);
    }

    fclose(fsource);
    fclose(fdestination);

    return 0 ;
}


int fichierExisteDeja(const char *nomFichier, const char *listFile) {
    FILE *file = fopen(listFile, "r");
    if (file == NULL) {
        return 0; // Si le fichier listProd.txt n'existe pas
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0'; // POur supprimer le saut de ligne à la fin de la ligne

        if (strstr(line, nomFichier) != NULL) {
            fclose(file);
            return 1; // Si e fichier existe déjà dans listProd.txt
        }
    }

    fclose(file);
    return 0; // Si le fichier n'existe pas dans listProd.txt
}


// Fonction qui va créer listProd.txt si elle n'existe pas qui va contenir une liste des fichiers avec leur date
void creerListProd(const char *productionRepertoire, const char *listFile) {
    FILE *file = fopen(listFile, "a");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    DIR *dir = opendir(productionRepertoire);
    if (dir == NULL) {
        printf("Erreur lors de l'ouverture du repertoire %s.\n", productionRepertoire);
        return;
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char filePath[MAX_FILENAME_LENGTH];
        snprintf(filePath, sizeof(filePath), "%s/%s", productionRepertoire, ent->d_name);

        struct stat fileStat;
        if (stat(filePath, &fileStat) == -1) {
            printf("Erreur lors de la récuperation des informations du fichier %s.\n", filePath);
            continue;
        }

        // Vérifier si le fichier existe déjà dans listProd.txt
        if (fichierExisteDeja(ent->d_name, listFile)) {
            printf("Le fichier %s existe deja dans %s. Il ne sera pas ajoute.\n", ent->d_name, listFile);
            continue;
        }

        // Récupérer la date de modification du fichier
        char dateTimeString[MAX_LINE_LENGTH];
        struct tm *modificationTime = localtime(&fileStat.st_mtime);
        strftime(dateTimeString, sizeof(dateTimeString), "%Y-%m-%d %H:%M:%S", modificationTime);

        fprintf(file, "%s - %s\n", ent->d_name, dateTimeString);
    }


    closedir(dir);


    fclose(file);
    printf("Le fichier %s a ete mis a jour avec succes.\n", listFile);
}


