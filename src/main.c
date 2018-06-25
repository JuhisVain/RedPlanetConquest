#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include "controller.h"

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");
  srand((unsigned int) time(NULL) ); //Init rand with time
  
  start_game();
  
  return 0;
}
