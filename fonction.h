//
// Created by tobby on 30/05/23.
//

#ifndef PROJETSYSTEME_FONCTION_H
#define PROJETSYSTEME_FONCTION_H

int save_data_log(const char *fichier_log, const char *data);
void copier_fichiers(const char *dossierSource, const char *dossierDestination);
void *test_dispo(void *arg);

FILE* synchroList(const char* file1, const char* file2);

int copy_list(char* nomfichier, char*nomdossiersource, char* nomdossierdestination);
int copier_fichier_vers_dossier(char* nomfichier, char* nomdossiersource, char* nomdossierdestination);
int fichierExisteDeja(const char *nomFichier, const char *listFile);
void creerListProd(const char *productionRepertoire, const char *listFile);
int get_nombre_erreurs();
int get_nombre_fichier_recu();
int nombre_ligne_fichier(char* nomfichier);
int enregistrer_erreur(char* descriptif);
int enregistrer_fichier_recu(char* nomfichier);
int ecrire_ligne_fin_fichier_stat(char* ligne, char* nom_fichier);

#endif //PROJETSYSTEME_FONCTION_H
