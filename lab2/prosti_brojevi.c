#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
 #include <stdlib.h>
 
 
int zadnji_prosti;
int zastavica = 0;
void periodicki_posao ( int sig )
{
   printf("zadnji prosti broj = %d\n", zadnji_prosti);
}

void signal1( int sig ){
    if( zastavica == 0 ){
        zastavica = 1;
        printf("pauziram ...\n");
    }
    else{
        zastavica = 0;
        printf("nastavljam ...\n");
    }
}

void signal2( int sig ){
    printf("zadnji prosti broj = %d\n", zadnji_prosti);
    exit(0);
}
int main ()
{
	struct itimerval t;
    int i, ne, brojac = 2;
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

	


	while (1){
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
	}

	return 0;
}
