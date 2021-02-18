#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

void exitFailure()
{
   perror(NULL);
   exit(EXIT_FAILURE);
}

void beChild(char *childName, int *newfds, int oldread)
{
   char rfd[2], wfd[2];
   
   /*Close read end of pipe that is being written to*/
   close(newfds[READ]);

   /*Convert fds to strings for exec call*/
   sprintf(wfd, "%d", newfds[WRITE]);
   sprintf(rfd, "%d", oldread);

   /*Exec the child*/
   execl(childName, childName, "-i", rfd, "-o", wfd, NULL);

   /*Error check exec*/
   exitFailure();
}

void beParent(int newfd, int oldread)
{
   /*Close parent copy of write end of pipe*/
   close(newfd);

   /*Close parent copy of read end of previous pipe*/
   if (oldread != STDIN_FILENO)
      close(oldread);
}

void printLoop(int oldread)
{
   int ret;
   char c;

   while(1) {
      if (0 == (ret = read(oldread, &c, 1)))
         break;
      else if (ret == -1)
         exitFailure();
      printf("%c", c);
   }

   /*Close Pipe-end*/
   close(oldread);
}

void forkIt(char *childName, int *newfds, int oldread, pid_t pid)
{
   /*Fork Error*/
   if (-1 == (pid = fork())) 
      exitFailure();

   /*Child*/
   else if (pid == 0)
      beChild(childName, newfds, oldread);
      
   /*Parent post-fork*/
   else
      beParent(newfds[WRITE], oldread);
}

void pipeline(int numChildren, char *childName)
{
   int i, status, newfds[2];
   int oldread = STDIN_FILENO;
   pid_t pid = 0;

   for (i = 0; i < numChildren; i++) {
      /*Create new pipe*/
      pipe(newfds);
     
      /*Fork*/
      forkIt(childName, newfds, oldread, pid);
      
      /*Exchange oldread with newfd*/
      oldread = newfds[READ];
   }

   /*Print to STDOUT*/
   printLoop(oldread);
 
   /*Wait for children to terminate*/
   for (i = 0; i < numChildren; i++) {
      wait(&status);
   }
}
