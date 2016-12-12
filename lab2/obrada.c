#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
int polje[5]; ///cuvamo koliko ima prekida pojedine vrste
int dosao[6]; ///do kud sam stao s pojedinim signalom, dosao[5] pamti do kud smo dosli u gl.p

void periodicki_posao(){
    int i, pisemo_pocetak = 0, pisemo_kraj = 0;
    for( i = 4; i >= 0; --i){
        if( polje[i] != 0 || dosao[i] != 0)
            break;      ///prvi signal koji moramo obraditi
    }
    if( dosao[i] == 0 && i != -1 ){
        polje[i]--;     ///signal koji smo zapoceli brisemo iz pamcenja
        dosao[i]++;
        pisemo_pocetak = 1;
    }
    if( dosao[i] == 6 && i != -1 )
        pisemo_kraj = 1;
    if( i == 4){
        if( pisemo_pocetak )
           printf("-  -  -  -  -  P\n");
        else if( pisemo_kraj )
                printf("-  -  -  -  -  K\n");
        else {
            printf("-  -  -  -  -  %d\n", dosao[i]);
            ++dosao[i];
        }
    }
    if( i == 3){
        if( pisemo_pocetak )
           printf("-  -  -  -  P  -\n");
        else if( pisemo_kraj )
            printf("-  -  -  -  K  -\n");
        else {
            printf("-  -  -  -  %d  -\n", dosao[i]);
            ++dosao[i];
        }
    }
    if( i == 2){
        if( pisemo_pocetak )
           printf("-  -  -  P  -  -\n");
        else if( pisemo_kraj )
            printf("-  -  -  K  -  -\n");
        else{
            printf("-  -  -  %d  -  -\n", dosao[i]);
            ++dosao[i];
        }
    }
    if( i == 1){
        if( pisemo_pocetak )
           printf("-  -  P  -  -  -\n");
        else if( pisemo_kraj )
                printf("-  -  K  -  -  -\n");
        else{
            printf("-  -  %d  -  -  -\n", dosao[i]);
            ++dosao[i];
        }
    }
    if( i == 0){
        if( pisemo_pocetak )
           printf("-  P  -  -  -  -\n");
        else if( pisemo_kraj )
            printf("-  K  -  -  -  -\n");
        else{
            printf("-  %d  -  -  -  -\n", dosao[i]);
            ++dosao[i];
        }
    }
    if(i != -1 && dosao[i] == 6 && pisemo_kraj){
        dosao[i] = 0;
    }
    if( i == -1 ){ ///ovo znaci da nemamo signala za obradu
        ++dosao[5];
        printf("%d  -  -  -  -  -\n", dosao[5]);
    }


}


void obradi_signal1(){
    printf("-  X  -  -  -  -\n");
    ///zapamtimo da smo dobili signal
    ++polje[0];
}
void obradi_signal2(){
    printf("-  -  X  -  -  -\n");
    ///zapamtimo da smo dobili signal
    ++polje[1];
}
void obradi_signal3(){
    printf("-  -  -  X  -  -\n");
    ///zapamtimo da smo dobili signal
    ++polje[2];
}
void obradi_signal4(){
    printf("-  -  -  -  X  -\n");
    ///zapamtimo da smo dobili signal
    ++polje[3];
}
void obradi_signal5(){
    printf("-  -  -  -  -  X\n");
    ///zapamtimo da smo dobili signal
    ++polje[4];
}
int main(){
    //ipisemo id procesa
    struct itimerval t;
    int i;

    for( i = 0; i < 5; ++i)
        polje[i] = 0;
    for( i = 0; i < 6; ++i)
        dosao[i] = 0;

    printf("Proces obrade prekida, PID=%d\n", getpid());

    sigset ( SIGUSR1, obradi_signal1 );
    sigset ( SIGUSR2, obradi_signal2 );
    sigset ( SIGQUIT, obradi_signal3 );
    sigset ( SIGTSTP, obradi_signal4 );
    sigset ( SIGINT, obradi_signal5 );
    sigset ( SIGALRM, periodicki_posao );

	t.it_value.tv_sec = 1;
	t.it_value.tv_usec = 0;
	t.it_interval.tv_sec = 1;
	t.it_interval.tv_usec = 0;
    setitimer ( ITIMER_REAL, &t, NULL );


    while(1){

    }

    return 0;

}

