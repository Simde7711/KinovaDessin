#include <cstdio>
#include <ctime>
#include <cstdlib>
int main(int argc, char **argv) {
  int frequency;
  if(argc != 2 || sscanf(argv[1],"%d",&frequency)==EOF){printf("usage: \nclock [frequency]\nwhere frequency is a integer representing hertz\n");exit(1);}
  int i = 0;
  timespec t_first;
  timespec t_last;

  frequency=1000000000/frequency; // convertir Hertz en intervalles de nanosecondes
  while (1) {
    clock_gettime(CLOCK_MONOTONIC,&t_first);

    // DO THINGS UNDER
    printf("%d\n",i);
    i++;
    // DO THINGS ABOVE
    
    clock_gettime(CLOCK_MONOTONIC, &t_last);
    while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < frequency) {
      clock_gettime(CLOCK_MONOTONIC, &t_last);
    }
  }

}
