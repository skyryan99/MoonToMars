To run executable (Usage): a.out -p N childName

--Where "N" is a number of child processes to create in the pipeline and "childName" is a path for the child executable.


Files: 

pipeline.c - main program. Contains command line parsing logic, dictates number of allowable child processes. (2 <= N <= 10 in this implementation)

launcher.c - pipeline logic. Contains fork and exec calls to children. Also contains information processing. (Printing to stdout in this implementation)

launcher.h - header for launcher.c. Self explanatory


Not Included:

Child program - contains logic for child program. Tested by randomizing integers.
  --Expected Implementation: read and write float sensor readings to output file in external memory to be processed later with seperate processing software. (Rebecca's algorithm)
