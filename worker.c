#include <stdio.h>
#include <unistd.h>
#include "worker.h"

void init_monitor_worker(struct MonitorWorker * p) {
    init_monitor(&(p->m), 2);
    p->elaborazioni_completate = 0;
    p->elaborazioni_da_completare = 0;
}

void delete_monitor_worker(struct MonitorWorker * p) {
    remove_monitor(&(p->m));
}

void preleva_risultati(struct MonitorWorker * p, int * vettore) {
    enter_monitor(&(p->m));

    while(p->elaborazioni_completate < N_WORKER) {
        wait_condition(&(p->m), VAR_MANAGER);
    }

    int vettore_medie[DIM];

    for(int i=0; i<DIM; i++) {
        vettore_medie[i] = 0;
    }
    
    for(int j=0; j<N_WORKER; j++) {
        for(int i=0; i<DIM; i++) {
            vettore_medie[i] += p->vettori_risultati[j][i];
        }
    }

    for(int i=0; i<DIM; i++) {
        vettore_medie[i] /= N_WORKER;
        vettore[i] = vettore_medie[i];
    }

    p->elaborazioni_completate = 0;

    leave_monitor(&(p->m));
}

void scrivi_vettore_condiviso(struct MonitorWorker * p, int * vettore) {
    enter_monitor(&(p->m));

    for(int i=0; i<DIM; i++) {
        p->vettore_condiviso[i] = vettore[i];
    }

    p->elaborazioni_da_completare = N_WORKER;

    for(int i=0; i<N_WORKER; i++) {
        signal_condition(&(p->m), VAR_WORKER);
    }

    leave_monitor(&(p->m));
}

void preleva_vettore_condiviso(struct MonitorWorker * p, int * vettore) {
    enter_monitor(&(p->m));

    while(p->elaborazioni_da_completare == 0) {
        wait_condition(&(p->m), VAR_WORKER);
    }

    for(int i=0; i<DIM; i++) {
        vettore[i] = p->vettore_condiviso[i];
    }

    p->elaborazioni_da_completare--;

    leave_monitor(&(p->m));
}

void scrivi_risultati(struct MonitorWorker * p, int * vettore, int id_worker) {
    enter_monitor(&(p->m));

    for(int i=0; i<DIM; i++) {
        p->vettori_risultati[id_worker][i] = vettore[i];
    }

    p->elaborazioni_completate++;

    if(p->elaborazioni_completate == N_WORKER) {
        signal_condition(&(p->m), VAR_MANAGER);
    }

    leave_monitor(&(p->m));
}

void stampa_vettore(int * vettore) {
    printf("[ ");
    for(int i=0; i<DIM; i++) {
        printf("%d ", vettore[i]);
    }
    printf("]\n");
}