#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
 
// pri pokretanju iz terminala treba -lm zbog sqrt 
int  broj_dretvi;
long long  brojac = 2, zadnji_prosti = 0;
int zastavica = 0;
pthread_t *thr;
int *ulaz, *broj, zadnji = 0;
void periodicki_posao ( int sig ){

   printf("zadnji prosti broj = %lld\n", zadnji_prosti);
   
}

void signal1( int sig ){
   
    if( zastavica == 0 ){
        zastavica = 1;
        printf("pauziram ...\n");
    }
    else{
		int i;
        zastavica = 0;
        printf("nastavljam ...\n");
        for( i = 0; i < broj_dretvi; ++i )
			pthread_kill( thr[i], SIGUSR1);
    }
    
}

void signal2( int sig ){
    periodicki_posao( sig );
    exit(0);
}

void udi_u_KO( int index_dretve ){

    ulaz[index_dretve] = 1;
    zadnji++;
    broj[index_dretve] = zadnji;
    ulaz[index_dretve] = 0;

    int i;
    for( i=0; i < broj_dretvi; ++i ){

        while( ulaz[i] == 1 );

        while( broj[i] != 0 && ( broj[i] < broj[index_dretve] )
                    || ( broj[i] == broj[index_dretve] && i < index_dretve ) );

    }

}

void izadji_iz_KO( int index_dretve ){

    broj[index_dretve] = 0;

}

//funcija provjerava da li je broj prost
int je_li_prost( int broj ){
		
		int i = 0;
		for( i = 2; i < sqrt( broj ) + 1; ++i )
			if( broj % i == 0 )
				return 0;
		return 1;

}

void pokreni_se( int sig ){
	
	printf("Nastavak rada jedne dretve\n");

}
	
//posao koji obavlja svaka dretva
void *trazi_proste( void *x ){

	sigset( SIGUSR1, pokreni_se );
	int moj_broj;
	int moj_index = *( (int *) x );
	
	while(1){
		if( zastavica )
			pause();
		udi_u_KO( moj_index );
		moj_broj = brojac;
		brojac++;
		izadji_iz_KO( moj_index );
		
		int test = je_li_prost( moj_broj );
		
		udi_u_KO( moj_index );
		if( test && moj_broj > zadnji_prosti )
			zadnji_prosti = moj_broj;
		izadji_iz_KO( moj_index );
			
	}
	
}
	
int main ( int argv, char** argc )
{
	
	//postavljanje alarma
	struct itimerval t;
    int i;
	/* povezivanje obrade signala SIGALRM sa funkcijom "periodicki_posao" */
	sigset ( SIGALRM, periodicki_posao );
	sigset( SIGINT, signal1 );
	sigset( SIGTERM, signal2 );

	/* definiranje periodičkog slanja signala */
	/* prvi puta nakon: */
	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;
	/* nakon prvog puta, periodicki sa periodom: */
	t.it_interval.tv_sec = 1;
	t.it_interval.tv_usec = 0;
    setitimer ( ITIMER_REAL, &t, NULL );
	
	//broj dretvi dobivamo iz komandne linije
	if( argv != 2 ){
		
		printf( "Krivi unos s komandne linje\n" );
		exit( 1 );
	
	}
	
	broj_dretvi = atoi( argc[1] );
	
	//alociramo potrebnu memoriju
	thr = malloc( broj_dretvi * sizeof( pthread_t ) );
	int *indexi = malloc( broj_dretvi * sizeof( int ) );
	ulaz = calloc( broj_dretvi, sizeof( int ) );
	broj = calloc( broj_dretvi, sizeof( int ) );
	
	
	//stvaramo dretve
	for( i = 0; i < broj_dretvi; ++i ){
		indexi[i] = i;
		if( pthread_create( &thr[i], NULL, trazi_proste, &indexi[i] ) ){
			printf( "Greska: pthread_create\n " );
			exit( 1 );
		}
	}
	
	for( i = 0; i < broj_dretvi; ++i )
        pthread_join( thr[i], NULL );

	
	/*while (1){
		if(zastavica == 0){
		    ne = 1;
		    for( i=2; i<brojac/2+1; ++i)
		        if( brojac%i == 0 )
		            ne = 0;
		    if( ne )
		        zadnji_prosti = brojac;
		    ++brojac;
		}
		else
			pause();
	}*/

	return 0;
}

