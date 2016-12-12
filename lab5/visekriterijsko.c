#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int t;
int do_kud_sam_stigao=0;		//do koje dretve smo dosli u nizu novi
int brojac_dretvi=0;
typedef struct{
	int id;
	int p;		//prostalo vijeme rada
	int prio;	//prioritet
	int rasp;	//način raspoređivanja
}dretva;

#define 	MAX_DRETVI		7
dretva*	P[MAX_DRETVI];		//red pripravnih dretvi

#define 	DRETVI 			6
int nove[DRETVI][5] = {
	{1, 3, 5, 3, 1},
	{3, 5, 6, 5, 1},
	{7, 2, 3, 5, 0},
	{12, 1, 5, 3, 0},
	{20, 6, 3, 6, 1},
	{20, 7, 4, 7, 1}
};

void ispis_stanja ( int ispisi_zaglavlje )
{
	int i;

	if ( ispisi_zaglavlje ) {
		printf ( "  t    AKT" );
		for ( i = 1; i < MAX_DRETVI; i++ )
			printf ( "     PR%d", i );
		printf ( "\n" );
	}

	printf ( "%3d ", t );
	for ( i = 0; i < MAX_DRETVI; i++ )
		if ( P[i] != NULL )
			printf ( "  %d/%d/%d ",
				 P[i]->id, P[i]->prio, P[i]->p );
		else
			printf ( "  -/-/- " );
	printf ( "\n");
}
//dodaje novu dretvu na pravo mjesto
void dodaj_na_pravo_mjesto(int index){
	int kamo = 0;
	while(P[kamo]!=NULL && P[kamo]->prio >= nove[index][3] )
		++kamo;
	for(int k = brojac_dretvi; k > kamo; --k)
		P[k] = P[k-1];
	P[kamo] = malloc( sizeof(dretva) );
	P[kamo]->id = nove[index][1];
	P[kamo]->p = nove[index][2];
	P[kamo]->prio = nove[index][3];
	P[kamo]->rasp = nove[index][4];
}

//funcije ubacuje nove dretve u red pripravnih ako dolaze
//u trenutnom vremenu
void  ubaci_nove(){
	while( t == nove[do_kud_sam_stigao][0] ){ //dolazi nam nove dretva
		printf( "  %d -- nova dretva id=%d, p=%d, prio=%d rasp=%d\n", t, nove[do_kud_sam_stigao][1],
						nove[do_kud_sam_stigao][2], nove[do_kud_sam_stigao][3], nove[do_kud_sam_stigao][4] );
		dodaj_na_pravo_mjesto(do_kud_sam_stigao);
		//dodaj_na_kraj(do_kud_sam_stigao);
		ispis_stanja(0);
		++do_kud_sam_stigao;
		++brojac_dretvi;
	}

}

//funcija popravlja redoslijed dretvi u redu
void popravi_redoslijed(){
	if( P[0] == NULL ){
		for(int i=0; i < brojac_dretvi-1; ++i)
			P[i]=P[i+1];
		P[brojac_dretvi-1]=NULL;
		--brojac_dretvi;
	}
	else{
		dretva *tmp = P[0];
		if( P[0]->rasp == 1 ){
			int i = 0;
			while( i+1 < brojac_dretvi && P[i]->prio == P[i+1]->prio ){
				P[i] = P[i+1];
				++i;
			}
			P[i] = tmp;
		}
	}
}

int main () {
	for(int i=0; i<MAX_DRETVI; ++i)
		P[i]=NULL;
	t = 0; //vrijeme
	while(1) {
		//printf("novi krug\n");
		if( P[0] != NULL ){ //ima aktivna dretva
			P[0]->p--; //odrađena jedinica vremena
			if( P[0]->p == 0 ){
				printf("dretva %d je zavrsila\n", P[0]->id);
				P[0] = NULL;
			}
			popravi_redoslijed();
		}
		ispis_stanja(!t);

		ubaci_nove();

		sleep(1); //simulacija rada dretve
		t++; //prošla jedinica vremena
	}
}
