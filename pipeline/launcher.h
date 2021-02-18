#ifndef LAUNCHER_H
#define LAUNCHER_H

void pipeline(int numChildren, char *childName);
void beChild(char *childName, int *newfds, int oldread);
void beParent(int *newfds, int oldread);
void exitFailure();
void printLoop(int oldread);

#endif
