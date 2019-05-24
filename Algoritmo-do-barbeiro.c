/*Por Débora Bianca Taveira de Moura
O Código foi feito com o auxílio dos links referenciados abaixo
Referências: 	http://ces33.wikidot.com/relas:misael-barbeiro
                http://ces33.blogspot.com/2009/05/o-problema-do-barbeiro-dorminhoco-com_07.html
COMO EXECUTAR:  1º possuir todas as bibliotecas necessárias;
                2º abrir terminal linux;
                3º gere o executável: gcc barbeiro.c -o barbeiro pthread
                4º execute e seja feliz: ./barbeiro
*/

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"
#include "semaphore.h"

#define CADEIRAS 3                /* número de cadeiras*/
#define TRUE 1

sem_t customers;                /* clientes esperando de atendimento */
sem_t barbers;                  /* número de barbeiros à espera de clientes */
sem_t mutex;                    /* para exclusão mútua */
int waiting = 0;                /* clientes que estão esperando (não estão cortando) */

void* barber(void *arg) {
	while(TRUE) {
		sem_wait(&customers);   /* dorme se o número de clientes for 0 */
		sem_wait(&mutex);
		waiting--;              /* descresce um do contador de clientes à espera */
		sem_post(&barbers);     /* barbeiro está pronto para cortar cabelo */
		sem_post(&mutex);       /* libera 'waiting' */
		cut_hair();             /* corta o cabelo (fora da região crítica) */
	}
	pthread_exit(NULL);
}

void* customer(void *arg) {
	sem_wait(&mutex);

	if(waiting < CADEIRAS) {      /* se não houver cadeiras vazias, saia */
		customer_arrived();
		waiting++;              /* incrementa o contador de clientes à espera */
		sem_post(&customers);   /* acorda o barbeiro se necessário */
		sem_post(&mutex);       /* libera o acesso a 'waiting' */
		sem_wait(&barbers);     /* vai dormir se o número de barbeiros livres for 0 */
		get_haircut();          /* sentado e sendo servido */
	}else{
		sem_post(&mutex);       /* a barbearia está cheia; não espera */
		giveup_haircut();
	}
	pthread_exit(NULL);
}

void cut_hair() {
	printf("Barbeiro esta cortando o cabelo do cliente!\n");
	sleep(2);
}

void customer_arrived() {
	printf("O cliente chegou para cortar cabelo!\n");
}
void get_haircut() {
	printf("O cliente esta tendo o cabelo cortado!\n");
}

void giveup_haircut() {
	printf("O salao esta cheio :[\n");
}

int main() {
    sem_init(&customers, TRUE, 0);
    sem_init(&barbers, TRUE, 0);
    sem_init(&mutex, TRUE, 1);
    pthread_t b, c;
     /* criando barbeiro */
     pthread_create(&b, NULL, (void *) barber, NULL);
     /* criação de clientes */
    while(TRUE) {
        pthread_create(&c, NULL, (void *) customer, NULL);
        sleep(1);
    }
    return 0;
}
