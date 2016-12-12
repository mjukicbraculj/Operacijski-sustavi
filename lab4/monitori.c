#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t kljuc;
pthread_cond_t pun, prazan;

int ulaz=0, izlaz=0; 
int M[5];
int duljina=0;
int pun_brojac=0;
int prazan_brojac=1;
int broj_proiz;
int kolicina_u_spremniku=0;

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
		pthread_mutex_lock(&kljuc);
		while( kolicina_u_spremniku == 5 ){
			//printf("%d ide na cekanje\n", index);
			pthread_cond_wait(&pun, &kljuc);
		}
		M[ulaz] = s[i];
		printf("PROIZVODJAČ%d -> %c\n", index, s[i]);
		ulaz = ( ulaz + 1 ) % 5;
		kolicina_u_spremniku++;
		if( kolicina_u_spremniku == 1 ){
			//printf("hej citac\n");
			pthread_cond_signal(&prazan);		
		}
		pthread_mutex_unlock(&kljuc);
		if( s[i] == '\0' )
			break;
		i += 1;
		sleep( 1 );
	}
}


//dretva potrosac cita podake iz zajednickog spreminka
//sprema podatke u string koji ce na kraju ispisati
void* potrosac(void *podaci){
	char *s = malloc(duljina+1);
	int i = 0;
	while( 1 ){
		pthread_mutex_lock(&kljuc);
		while( kolicina_u_spremniku == 0 ){
			pthread_cond_wait(&prazan, &kljuc);
			//printf("potrosac ide na cekanje\n");
		}
		if( M[izlaz] != '\0')
			s[i++] = M[izlaz];
		else
			broj_proiz--;
		printf( "POTROŠAC <- %c\n", M[izlaz]);
		izlaz = ( izlaz + 1 ) % 5;
		--kolicina_u_spremniku;
		if( kolicina_u_spremniku == 4 ){
			pthread_cond_broadcast(&pun);
			//("mozete pisati\n");
		}
		pthread_mutex_unlock(&kljuc);
		if( broj_proiz == 0 )
			break;
	}
	s[i] = '\0';
	printf("Primljeno je: %s\n", s);
}


int main(int argc, char **argv){


	if( argc < 2 ){
		printf("Treba biti vise argumenata komandne linije\n");
		exit( 1 );
	}

	if (pthread_mutex_init(&kljuc, NULL) != 0)
    {
        printf("Mutex init greska!\n");
        return 1;
    }
    if(pthread_cond_init(&pun, NULL) != 0)
    {
    	printf("Pun init greska!\n");
    	return 1;
    }
    if(pthread_cond_init(&prazan, NULL) != 0)
    {
    	printf("Prazan init greska!\n");
    	return 1;
    }
	
	int i;
	broj_proiz = argc - 1;
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
	
	pthread_mutex_destroy(&kljuc);
	free(polje);
	free(dretve);
	free(potr);

	return 0;	
}
