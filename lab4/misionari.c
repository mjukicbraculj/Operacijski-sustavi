#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

///desna stana obale je oznacena s 1, lijeva s 0

pthread_mutex_t kljuc = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t d = PTHREAD_COND_INITIALIZER;
pthread_cond_t l = PTHREAD_COND_INITIALIZER;
pthread_cond_t plovi = PTHREAD_COND_INITIALIZER;

typedef struct{
	int obala;
	int index;
}podaci;

int obala_camac = 1;
int broj_kanibala_na_camcu = 0;
int broj_misionara_na_camcu = 0;

//funkcija vraca 1 ako kanibal ne smije na camac
//inace vrati 0
ulazak_kanibala(int k, int m){
	
	return !( k + m + 1 < 8 && ( k + 1 < 4 || m == 0) ); 
}



//funkcija vraca 1 ako misionar ne smije na camac 
//inace vrati 0
ulazak_misionara(int k, int m){

	return !( k + m + 1 < 8 && ( k < 4 ) );
}

//dretva kanibal oznacava jednog kanibala
//kanibal moze uci na camac ako stanje na camcu to dopusta
//iance mora cekati u redu
void* kanibal( void * id_ ){

	podaci* id = (podaci*) id_;
	int index = id->index;
	int obala = id->obala;
	pthread_mutex_lock( &kljuc );

	while( obala != obala_camac ||
		 ulazak_kanibala(broj_kanibala_na_camcu, broj_misionara_na_camcu) ) {
			
		if(obala == 1)
			pthread_cond_wait( &d, &kljuc );
		else
			pthread_cond_wait( &l, &kljuc );
	}
	broj_kanibala_na_camcu++;
	int k = broj_kanibala_na_camcu, m = broj_misionara_na_camcu, o = obala_camac;
	printf("kanibal%d se utrpao na camac na %d obali ... (%d, %d, %d)\n", index, obala, m, k, o);
	if( (broj_kanibala_na_camcu <= broj_misionara_na_camcu 
				|| broj_misionara_na_camcu == 0) 
						&& broj_kanibala_na_camcu + broj_misionara_na_camcu > 2 ){
		//printf("camac moze ploviti kaze kanibal%d\n", index);
		pthread_cond_signal( &plovi );
	}

	pthread_mutex_unlock( &kljuc );
	free(id_);
}

//dretva misionar oznacava jednog misionara
//misionar moze uci u camac ako stanje na camcu to dopusta
//inace mora cekati u redu
void* misionar( void * id_ ){

	podaci* id = (podaci*) id_;
	int index = id->index;
	int obala = id->obala;
	pthread_mutex_lock( &kljuc );

	while( obala != obala_camac ||
		 ulazak_misionara(broj_kanibala_na_camcu, broj_misionara_na_camcu)  ){
		//printf("misionar%d ceka na %d obali\n", index, obala);
		if(obala == 1)
			pthread_cond_wait( &d, &kljuc );
		else
			pthread_cond_wait( &l, &kljuc );
	}
	broj_misionara_na_camcu++;
	int k = broj_kanibala_na_camcu, m = broj_misionara_na_camcu, o = obala_camac;
	printf("misionar%d se utrpao na camac na %d obali....(%d, %d, %d)\n", index, obala, m, k, o);
	if( (broj_kanibala_na_camcu <= broj_misionara_na_camcu  
			|| broj_misionara_na_camcu == 0 )&& 
				broj_kanibala_na_camcu + broj_misionara_na_camcu > 2 ){
		pthread_cond_signal( &plovi );
		//printf("camac moze ploviti kaze misionar%d\n", index);
	}

	pthread_mutex_unlock( &kljuc );
	free(id_);
}

void* camac( void* a){

	while(1){
		pthread_mutex_lock(&kljuc);
		while( broj_kanibala_na_camcu + broj_misionara_na_camcu < 3 
				|| ( broj_misionara_na_camcu < broj_kanibala_na_camcu
						&& broj_misionara_na_camcu != 0 ) ){
			pthread_cond_wait(&plovi, &kljuc);
			//printf("camac mora cekati\n" );
		}
		//printf("camac moze ploviti %d i %d je sastav\n", broj_kanibala_na_camcu, broj_misionara_na_camcu);
		sleep(1);	
		printf("prevozim s %d obale na %d obalu %d misionara i %d kanibala\n", 
			obala_camac, 1-obala_camac,	broj_misionara_na_camcu, broj_kanibala_na_camcu);
		obala_camac = 1 - obala_camac;
		broj_misionara_na_camcu = 0;
		broj_kanibala_na_camcu = 0;
		//camac_spreman = 0;
		sleep( 2 );
		if(obala_camac == 0){
			//printf("oslobadjam %d obalu\n", obala_camac);
			pthread_cond_broadcast( &l );
		}
		else{
			pthread_cond_broadcast( &d );
			//printf("oslobadjam %d obalu\n", obala_camac);
		}
		pthread_mutex_unlock(&kljuc);
	}
}


int main(){

	pthread_t dretva;
	pthread_attr_t atribut;
	int br_misionari = 0, br_kanibali = 0;
	podaci* id;


	pthread_attr_init(&atribut);
	pthread_attr_setdetachstate(&atribut, PTHREAD_CREATE_DETACHED);

	pthread_create(&dretva, &atribut, camac, NULL);

	while( 1 ){

		id = (podaci*)malloc( sizeof( podaci ) );
		id->index = br_kanibali;
		id->obala = rand() % 2;
		//printf( "novi kanibal%d na %d obali \n", br_kanibali, id->obala );
		pthread_create( &dretva, &atribut, kanibal, (void*)id );
		br_kanibali++;
		id = malloc( sizeof( podaci ) );
		id->index = br_misionari;
		id->obala = rand() % 2;
		//printf( "novi misionar%d na %d obali\n", br_misionari, id->obala );
		pthread_create( &dretva, &atribut, misionar, (void*)id );
		br_misionari++;
		sleep( 1 );
		id = malloc( sizeof( podaci ) );
		id->index = br_kanibali;
		id->obala = rand() % 2;
		//printf( "novi kanibal%d na %d obali \n", br_kanibali, id->obala );
		pthread_create( &dretva, &atribut, kanibal, (void*)id );
		br_kanibali++;
		sleep( 1 );
	}

	return 0;

}