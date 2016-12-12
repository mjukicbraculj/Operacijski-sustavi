#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

int pid;
void obradi_signal( int sig ){
    printf("saljem signal kill\n");
    kill( pid, SIGKILL);
    kill( getpid(), SIGKILL);
}

int main( int argc, char* argv[] ){

    int min_vrijeme = 2, max_vrijeme = 7;
    int min_index = 0, max_index = 4;
    pid = atoi(argv[1]);

    if( argc != 2 ){
        printf("Greska u unosu s komandne linije\n");
    }

    sigset ( SIGINT, obradi_signal );

    int polje_signala[]={SIGUSR1, SIGUSR2, SIGQUIT, SIGTSTP};

    while(1){
        int cekam = (rand() % (max_vrijeme+1-min_vrijeme))+min_vrijeme;
        int index_signala = (rand() % (max_index+1-min_index))+min_index;
        printf("saljem index %d i spavao sam %d\n", index_signala, cekam);
        kill( pid, polje_signala[index_signala]);
        sleep(cekam);
    }


}
