#include <locale.h>
#include "controller.h"

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "");

  start_game();
  
  return 0;
}
