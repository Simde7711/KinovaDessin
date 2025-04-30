#include <cstdio>
#include <ctime>
#include <cstdlib>
int main(int argc, char **argv) {
  int frequency;
  int timer_max;
  if(argc != 3 || sscanf(argv[1],"%d",&frequency)==EOF || sscanf(argv[2],"%d",&timer_max)==EOF ){printf("usage: \nclock [frequency] [time]\nwhere frequency is a integer representing hertz\n");exit(1);}
  int i = 0;
  timespec t_first;
  timespec t_last;

  int max = timer_max * ((float)frequency / (float)1000); // convertir kHz et un temps (en ms) en nombre d'iterations. C'est
                                             // plus optimiser comme sa que avec un autre timer.
  frequency= 1000000000 / frequency; // convertir Hertz en intervalles de nanosecondes
  while (i<max) {
    clock_gettime(CLOCK_MONOTONIC,&t_first);

    // DO THINGS UNDER
    printf("%d\n",i);
    // DO THINGS ABOVE
    
    clock_gettime(CLOCK_MONOTONIC, &t_last);
    while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < frequency) {
      clock_gettime(CLOCK_MONOTONIC, &t_last);
    }
    i++;
  }

}
