#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "worker.h"


void main_manager(struct MonitorWorker * p);
void main_worker(struct MonitorWorker * p);
void worker(struct MonitorWorker * p, int id);

int main() {

    printf("[START] Inizializzazione risorse\n");

    /* TBD: Creare una istanza della struttura dati */

    struct MonitorWorker * p = (struct MonitorWorker *) shmat(IPC_PRIVATE, NULL, 0);
    if(p == (void *)-1) {
        perror("Errore shmat");
        exit(1);
    }
    
    /* TBD: Inizializzazione delle risorse */

    init_monitor_worker(p);

    printf("[START] Avvio manager e worker\n");

    /* TBD: Avviare due processi figli,
            fargli eseguire le funzioni
            main_manager() e main_worker()
     */
    pid_t pid;
    pid = fork();

    if(pid == 0){
        main_manager(p);
        exit(0);
    }else if(pid < 0){
        perror("errore fork manager");
        exit(1);
    }


    pid = fork();

    if(pid == 0){
        main_worker(p);
        exit(0);
    }else if(pid < 0){
        perror("errore fork worker");
        exit(1);
    }

    printf("[START] Attesa manager e worker\n");

    /* TBD: Attendere la terminazione dei processi figli */

    int status;
    
    for(int i=0; i<2; i++){
        wait(&status);
    }

    printf("[START] Deallocazione risorse\n");

    /* TBD: Deallocazione delle risorse */

    delete_monitor_worker(p);
    shmdt(p);
    shmctl(IPC_PRIVATE, IPC_RMID, NULL);

    return 0;
}


void main_manager(struct MonitorWorker * p) {

    int vettore[DIM];

    // Inizializza il vettore condiviso

    for(int i=0; i<DIM; i++) {
        vettore[i] = 0;
    }

    printf("[MANAGER] Inizializza vettore condiviso = ");
    stampa_vettore(vettore);

    
    /* TBD: inserire il vettore [0,0,...0]
            con la funzione scrivi_vettore_condiviso() */

        scrivi_vettore_condiviso(p, vettore);

    // Preleva i risultati dei worker, 
    // poi calcola il vettore dei valori medi, 
    // e infine lo scrive nel vettore condiviso

    for(int k=0; k<ITERAZIONI; k++) {

        printf("[MANAGER] In attesa di risultati...\n");

        /* TBD: Prelevare i risultati con la funzione preleva_risultati() */

        preleva_risultati(p, vettore);

        printf("[MANAGER] Nuovo vettore condiviso = ");
        stampa_vettore(vettore);
        
        /* TBD: Aggiornare il vettore condiviso con
                la funzione scrivi_vettore_condiviso() */

        scrivi_vettore_condiviso(p, vettore);

    }


    printf("[MANAGER] Vettore finale = ");
    stampa_vettore(vettore);

}




void main_worker(struct MonitorWorker * p) {


    printf("[WORKER] Avvio processi figli\n");

    /* TBD: Avviare ulteriori processi figli,
            che eseguano la funzione worker().
            Utilizzare l'indice del ciclo come identificativo
            da passare al processo figlio (variabile "id").
     */

    for(int i = 0; i < N_WORKER; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            worker(p, i);
            exit(0);
        } else if (pid < 0) {
            perror("Errore fork processi worker");
            exit(1);
        }
    }

    printf("[WORKER] Attesa processi figli\n");

    /* TBD: Attendere la terminazione dei figli */

    int status;

    for(int i = 0; i < N_WORKER; i++) {
        wait(&status);
    }

}


void worker(struct MonitorWorker * p, int id) {

    printf("[WORKER] Avvio worker %d\n", id);

    srand(getpid());

    int vettore[DIM];

    for(int k=0; k<ITERAZIONI; k++) {

        /* TBD: Prelevare i dati nel vettore condiviso,
                con la funzione preleva_vettore_condiviso() */

        preleva_vettore_condiviso(p, vettore);

        printf("[WORKER] id=%d vettore prelevato = ", id);
        stampa_vettore(vettore);

        sleep(rand() % 3 + 1);

        for(int i=0; i<DIM; i++) {

            vettore[i] += (-2 + rand()%6);
        }

        printf("[WORKER] id=%d vettore inserito = ", id);
        stampa_vettore(vettore);

        /* TBD: Inserire i risultati nel vettore dei risultati
                (riga indicata dall'id del worker),
                con la funzione scrivi_risultati()
         */

        scrivi_risultati(p, vettore, id); 

    }
}
