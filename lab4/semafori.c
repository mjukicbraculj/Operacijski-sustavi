#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

sem_t pisi, pun, prazan;
int ulaz=0, izlaz=0; 
int M[5];
int duljina=0;
int broj_proizv;

//struktura  koja cuva podatke za dretvu proizvodjac
typedef struct{
	char *s;
	int index;
}podaci;


//dretva proizvodjac cita podatke iz dobivenog stringa
//pise ih u zajednicki spremnik sve dok ne dodje do kraja stringa
void * proizvodjac(void *podatak){
	podaci* p = (podaci*) podatak;
	char *s = p->s;
	int index = p->index;
	int i = 0;
	while( 1 ){
		sem_wait( &pun );
		sem_wait( &pisi );
		M[ulaz] = s[i];
		printf("PROIZVODJAČ%d -> %c\n", index, s[i]);
		ulaz = ( ulaz + 1 ) % 5;
		sem_post( &pisi );
		sem_post( &prazan );
		if(s[i]=='\0')
			break;
		i += 1;
		sleep( 1 );
	}
}

//dretva potrosac cita podake iz zajednickog spreminka
//sprema podatke u string koji ce na kraju ispisati
void* potrosac(void *podaci){
	char *s = malloc(duljina);
	int i = 0;
	while( 1 ){
		sem_wait( &prazan );
		if(M[izlaz]!='\0')
			s[i++] = M[izlaz];
		else 
			--broj_proizv;
		printf( "POTROŠAC <- %c\n", M[izlaz]);
		izlaz = ( izlaz + 1 ) % 5;
		sem_post( &pun );
		if( broj_proizv == 0)
			break;
	}
	s[i] = '\0';
	printf("Primljeno je: %s\n", s);
	free(podaci);
}


int main(int argc, char **argv){


	if( argc < 2 ){
		printf("Treba biti vise argumenata komandne linije\n");
		exit( 1 );
	}

	sem_init(&pisi, 0, 1);
	sem_init(&pun, 0, 5);
	sem_init(&prazan, 0, 0);

	broj_proizv = argc-1;
	
	int i;

	for( i = 1; i < argc; ++i )
		duljina += strlen( argv[i] ) + 1;
	
	pthread_t *dretve = malloc( ( argc -1 ) * sizeof( pthread_t ) );
	podaci* polje = malloc( (argc -1 ) * sizeof( podaci ) );

	for( i = 0; i < argc-1; ++i ){
		polje[i].s = argv[i+1];
		polje[i].index = i;
		if( pthread_create( &dretve[i], NULL, proizvodjac, &polje[i] ) ){
			printf("Greska: pthrad_create\n");
			exit( 1 );
		}
	}

	pthread_t *potr = malloc( sizeof( pthread_t ) );
	pthread_create( &potr[0], NULL, potrosac, NULL );
	
	for( i=0; i < argc-1; ++i )
		pthread_join(dretve[i], NULL);
	pthread_join(potr[0], NULL);
	

	return 0;	
}
