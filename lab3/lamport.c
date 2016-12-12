#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int broj_stolova, broj_dretvi;
int *stolovi, *ulaz, *broj, zadnji = 0;
char *string;


//funkcija simulira slučajan odabir slobodnog stola
//izbrojimo koliko ima slobodnih stolova i odaberemo slucajan broj iz tog raspona
int random_slobodni_stol(){

    int brojac = 0, i, slucajan_broj;
    for( i = 0; i < broj_stolova; ++i )
        if( stolovi[i] == 0 )
            brojac++;

	if( brojac == 0 )
		return -1;
    //na ovaj nacin dobijemo broj od 0 do brojac-1
    slucajan_broj = rand() % brojac;

    for( i = 0; i < broj_stolova; ++i ){
        if( stolovi[i] == 0 )
            slucajan_broj--;
        if(slucajan_broj == -1)
            return i;
    }

}

//funcija obavlja rezervaciju stola i ispisuje odgovarajucu poruku korisniku
void rezerviraj_stol( int slobodni_stol, int index_dretve ){

    int i;
    stolovi[slobodni_stol] = index_dretve;
    char *poruka = malloc( ( broj_stolova + 50 )  * sizeof( char ) );
    sprintf( poruka, "Dretva %d: rezerviram stol %d \n", index_dretve, slobodni_stol );
    int kraj = strlen(poruka);
    for( i = 0; i < broj_stolova; ++i )
        if( stolovi[i] == 0 )
            poruka[i+kraj] = '-';
        else
            sprintf( &poruka[i+kraj], "%d", stolovi[i] );
    puts( poruka );
}

void ne_mogu_rezervirati( int slobodni_stol, int index_dretve ){

    int i;
    char *poruka = malloc( ( broj_stolova + 70 )  * sizeof( char ) );
    sprintf( poruka, "Dretva %d: neuspjela rezervacija stola %d \n", index_dretve, slobodni_stol );
    int kraj = strlen(poruka);
    for( i = 0; i < broj_stolova; ++i )
        if( stolovi[i] == 0 )
            poruka[i+kraj] = '-';
        else
            sprintf( &poruka[i+kraj], "%d", stolovi[i] );
    puts( poruka );

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

bool je_li_slobodan( int slobodni_stol ){

    if( stolovi[slobodni_stol] == 0 )
        return true;
    return false;

}
void izadji_iz_KO( int index_dretve ){

    broj[index_dretve] = 0;
}

void *dretva( void *x ){

    int index_dretve = *( ( int* )x );
    int slobodni_stol;      //varijabla cuva vrijednost odabranog slobodnog stola

    //provjerimo ima li slobodnih stolova, stajemo cim smo nasli jedan od njih
    while( 1 ){

		sleep(1);
		
        slobodni_stol = random_slobodni_stol();
        if(slobodni_stol == -1 ) break;				//nema vise slobodnih stolova

        printf( "Dretva %d: Odabirem stol %d\n", index_dretve, slobodni_stol );

        udi_u_KO( index_dretve );
        if( je_li_slobodan( slobodni_stol ) )
            rezerviraj_stol( slobodni_stol, index_dretve );
        else
            ne_mogu_rezervirati( slobodni_stol, index_dretve );
        izadji_iz_KO( index_dretve );

    }
}

int main( int argv, char **argc ){

    //provjera unosa s komandne linije
    if( argv != 3 ){
        printf( "Krivi unos s komandne linije\n" );
        exit(1);
    }

    //citanje brojeva unesenih s komande linije
    int i;
    broj_dretvi = atoi( argc[1] );
    broj_stolova = atoi( argc[2] );

    //alokacija polja indexi za cuvanje indexa dretve i polja dretvi
    //alokacija polja stolovi, na početku = 0;
    int *indexi = malloc( broj_dretvi * sizeof( int ) );
    pthread_t * thr = malloc( broj_dretvi * sizeof( pthread_t ) );
    stolovi = calloc( broj_stolova, sizeof( int ) );
    ulaz = calloc( broj_dretvi, sizeof( int ) );
    broj = calloc( broj_dretvi, sizeof( int ) );
    string = malloc ( broj_stolova * sizeof( char ) );

    //kreiramo dretve
    for( i = 0; i < broj_dretvi; ++i ){

        indexi[i] = i+1;
        if( pthread_create( &thr[i], NULL, dretva, &indexi[i] ) ){
            printf( "Greska: pthread_crate\n" );
            exit(1);
        }

    }

    //cekamo da sve dretve zavrse s radom
    for( i = 0; i < broj_dretvi; ++i )
        pthread_join( thr[i], NULL );

    return 0;
}
