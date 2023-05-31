//
// Created by tobby on 30/05/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "fonction.h"
#include <semaphore.h>
#define P(sem) sem_wait(&sem)
#define V(sem) sem_post(&sem)
#define P(sem2) sem_wait(&sem2)
#define V(sem2) sem_post(&sem2)
#define P(mutex) sem_wait(&mutex)
#define V(mutex) sem_post(&mutex)
#define P(mutex2) sem_wait(&mutex2)
#define V(mutex2) sem_post(&mutex2)
sem_t mutex;
sem_t mutex2;
sem_t sem;
sem_t sem2;
int actif = 0, prod = 0, back = 0;
const char* fichierProd = "prodList.txt";
const char* fichierBack = "backList.txt";
const char* fichierInt = "intList.txt";
char* fichier_log = "log/log.txt";
char* nom_fichier_copie = "listeAcopier.txt";
FILE *fichier_copie = NULL;

void * production(void * arg){
    P(mutex);
    prod = 1;
    back = 0;
    creerListProd("Production", fichierProd);
    V(mutex2);
    sleep(120);
    V(mutex);
    return NULL;
}

void * backup(void * arg){
    P(mutex);
    prod = 0;
    back = 1;
    creerListProd("Backup", fichierBack);
    V(mutex2);
    sleep(120);
    V(mutex);
    return NULL;
}

void * test_disponibilite_thread(void *arg){
    P(mutex2);
     if(prod == 1){
         actif = 1;
         printf("Serveur de production actif...\n");
         save_data_log(fichier_log, "Module test dispo: Serveur de production actif pour le transfère de donnée.");
     }else{
         actif = 2;
         printf("Serveur de backup actif...\n");
         save_data_log(fichier_log, "Module test dispo: Serveur de backup actif pour le transfère de donnée.");
     }
     V(sem);
     return NULL;
}

void * synchro_list_thread(void *arg){
    P(sem);
    fichier_copie = synchroList(fichierProd, fichierInt);
    V(sem);
    V(sem2);
    return NULL;
}

void * copy_list_thread(void *arg){
    int valeur = 0;
    P(sem2);
    P(sem);
    if(actif == 1){
        valeur = copy_list(nom_fichier_copie, "Production", "Integration");
    }else{
        valeur = copy_list(nom_fichier_copie, "Backup", "Integration");
    }
    if(valeur==0){
        printf("Copy de list de fichier avec succès\n");
        save_data_log(fichier_log, "Module copy: Copy de list de fichier avec succès");
    }else{
        printf("Erreur lors de la copie du fichier\n");
        save_data_log(fichier_log, "Module copy: Erreur lors de la copie du fichier");
    }
    V(sem);
    V(sem2);
    return NULL;
}

void * integration(void *arg){
    sem_init(&sem, PTHREAD_PROCESS_SHARED, 0);
    sem_init(&sem2, PTHREAD_PROCESS_SHARED, 0);
    pthread_t thread_test;
    pthread_t thread_synchro;
    pthread_t thread_backup;
    int *ptr1;
    int *ptr2;
    int *ptr3;

    if(pthread_create(&thread_test, NULL,test_disponibilite_thread,NULL)!=0){
        fprintf(stderr, "Erreur lors de la création du thread de test de disponibilité\n");
        save_data_log(fichier_log, "Module integration: Erreur lors de la création du thread de test de disponibilité");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&thread_backup, NULL,copy_list_thread,NULL)!=0){
        fprintf(stderr, "Erreur lors de la création du thread de copy liste\n");
        save_data_log(fichier_log, "Module integration: Erreur lors de la création du thread de copy liste");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&thread_synchro, NULL,synchro_list_thread,NULL)!=0){
        fprintf(stderr, "Erreur lors de la création du thread de synchro\n");
        save_data_log(fichier_log, "Module integration: Erreur lors de la création du thread de synchro");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread_test, (void **) &ptr1);
    pthread_join(thread_synchro, (void **) &ptr2);
    pthread_join(thread_backup, (void **) &ptr3);
}

int main(){
    int *ptr1;
    int *ptr2;
    int *ptr3;
    pthread_t integration_t;
    pthread_t production_t;
    pthread_t backup_t;
    sem_init(&mutex, PTHREAD_PROCESS_SHARED, 1);
    sem_init(&mutex2, PTHREAD_PROCESS_SHARED, 0);
    if(pthread_create(&integration_t, NULL, (void *(*)(void *)) integration, NULL) != 0 ){
        fprintf(stderr, "Erreur lors de la création du thread d'intégration\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&production_t, NULL, (void *(*)(void *)) production, NULL) != 0 ){
        fprintf(stderr, "Erreur lors de la création du thread de production\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&backup_t, NULL, (void *(*)(void *)) backup, NULL) != 0 ){
        fprintf(stderr, "Erreur lors de la création du thread de backup\n");
        exit(EXIT_FAILURE);
    }
    pthread_join(integration_t, (void **) &ptr1);
    pthread_join(production_t, (void **) &ptr2);
    pthread_join(backup_t, (void **) &ptr3);
    return EXIT_SUCCESS;
}
