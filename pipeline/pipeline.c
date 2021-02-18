#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "launcher.h"

void showUsage()
{
   fprintf(stderr, "Usage: pipeline path -p processes\n");
   fprintf(stderr, "Where: path is some \"random child\" and processes "
      "is 2 to 10, inclusive\n");
   exit(EXIT_FAILURE);
}

int getProcNum(char *arg)
{
   int i = 0,  num = 0;

   while (arg[i] != '\0' && isdigit(arg[i])) {
      /*Numericalize it and shift decimal if necessary*/
      num = num * 10 + (arg[i] - '0');
      i++;
   }

   return num;
}

int checkArgs(int argc, char *argv[], int *pflag)
{
   int proc = 0;

   /*Make sure there are 4 args*/
   if (argc != 4)
      showUsage();

   /*Make sure argv[1] or argv[2] is "-p"*/
   if (!strcmp(argv[1], "-p"))
      *pflag = 1;
   else if (!strcmp(argv[2], "-p"))
      *pflag = 2;
   else
      showUsage();

   proc = getProcNum(argv[*pflag + 1]); 

   /*Make sure argv[3] is 2 <= x <= 10; also catches non-numeric inputs*/
   if (proc < 2 || proc > 10)
      showUsage();

   /*Resturn this if its valid*/
   return proc;
}

int main(int argc, char *argv[])
{
   int argflag, pflag, procNum = checkArgs(argc, argv, &pflag);
   
   /*Use pflag to locate path*/
   if (pflag == 1)
      argflag = 3;
   else if (pflag == 2)
      argflag = 1;
   
   /*Make a call here to launcher.c: pipeline()*/
   pipeline(procNum, argv[argflag]);

   return 0;
}
