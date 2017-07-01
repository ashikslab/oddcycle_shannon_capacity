#include "rep.h"
#include<cstring>
#define MAXC 22450


int is_conj(int e1[5], int e2[5]) {
  for (int i1 = 0; i1<18; i1++) {
    for(int i2 = 0; i2<18; i2++) {
      for (int i3 = 0; i3<18; i3++) {
	for(int i4 = 0; i4<18; i4++) {
	  for(int j1 = 0; j1<24; j1++) {
	    int g[5];
	    g[0] = i1, g[1] = i2, g[2] = i3, g[3] = i4, g[4] = j1;

	    int ge1[5];
	    perform_groupop(g, e1, ge1);

	    int ginv[5];
	    inverse(g, ginv);
	    int ge1ginv[5];
	    perform_groupop(ge1, ginv, ge1ginv);
	    if(is_same(ge1ginv, e2)) {
	      return 1;
	    }  
	  }
	}
      }
    }
  }
  return 0;
}
int main(int argc, char *argv[]) {
  if(argc<2) {
    printf("usage: %s <filename>\n", argv[0]);
    exit(-1);
  }

  FILE *infile = fopen(argv[1], "r");
  int inputs[MAXC][6];
  int rqinputs[MAXC][5];
  char line[128];
  int linenum = 0;
  init_rep();
  while ( fgets ( line, sizeof line, infile ) != NULL ) /* read a line */
    {
      linenum++;
      char delims[] = ", ";
      char *pc = NULL;
      pc = strtok(line, delims);

      int incount = 0;
      while(pc != NULL) {
	int el = atoi(pc);
	inputs[linenum-1][incount] = el;
	incount++;
	pc = strtok( NULL, delims );
      }
      if(incount!=6) {
	printf("ERROR in input file, incount= %d\n",
	       incount);
	exit(-1);
      }
      for(int ii = 0; ii<5; ii++) {
	rqinputs[linenum-1][ii] = inputs[linenum-1][ii];
      }
      //inputs contain the generator of the cyclic group that we need
    }
  int alreadyconj[MAXC];
  for(int i=0; i<MAXC; i++) {
    alreadyconj[i] = 0;
  }
    
  for(int i=0; i<linenum; i++) {
    printf("i=%d\n", i); fflush(stdout);
    if(!alreadyconj[i]) {
      for(int j=i+1; j<linenum; j++) {
	printf("j = %d\n", j);
	//	    int retvals[5];
	if(!alreadyconj[j]) {
	  if(is_conj(rqinputs[i], rqinputs[j])) {
	    alreadyconj[i] = 1;
	    alreadyconj[j] = 1;
	  }
	}
      }
      char ofname[128];
      snprintf(ofname, sizeof(ofname), "%s-op.out", argv[1]);
      FILE *of = fopen(ofname, "a+");
      fprintf(of, "%d, %d, %d, %d, %d\n", rqinputs[i][0], rqinputs[i][1], rqinputs[i][2], rqinputs[i][3], rqinputs[i][4]);
      fclose(of);
    }
  }
}
