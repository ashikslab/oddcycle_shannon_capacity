/* tabu3.c tabu search program for clique finding
   by Patric Ostergard, 15.05.2014 */

/* make -f makefile3 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cliquer.h"
char infilename[128], outfilename[128];
#define MAXN 7000        /* order of graph */

int debug_level = 0;

extern long int lrand48();

int gr[MAXN][MAXN]; /* distance matrix */
int wt[MAXN];     /* orbit size */
int a60=0;

int d(int a,int b)
{
  int c,e,i;
 
  c = a^b;
  e = 0;
  for(i=0;i<16;i++)
    if(((c>>i)&1)==1)
      e++;
  return e;
}

int err_message(int i)
{
  printf("Usage: tabu d1 d2 threshold sim rounds seed (error %d)\n",i); 
  exit(1);
}

int main(argc,argv)
int argc;
char *argv[]; 
/* arguments: d1 d2 lambda sim rd tabu seed */
{
   int n,k,m,total;
   int ii[MAXN],jj[MAXN],nr,posit,sim,rd,rct=0;
   int tabu[MAXN],pp,npos;
   int absval,count,count2,p,size,temp,temp2;
   int timeout;

   int i,j,k2,tabo,seed;
   int weight,weight2,ok,ant,ant2,mx,gsize;
   int gtab[MAXN],htab[MAXN],value[MAXN],v2[MAXN];
   int d1,d2,d3;
   int high,threshold;
   int max=0;
   graph_t *g;
   set_t st;

/* no timer */
   clique_default_options->time_function = NULL; 

   if(argc<8) 
      err_message(10);
   if((d1 = atoi(argv[1]))==0)
      err_message(1);
   if((d2 = atoi(argv[2]))==0)
      err_message(2);
   if((threshold = atoi(argv[3]))==0)
      err_message(3);
   if((sim = atoi(argv[4]))==0)
      err_message(4);
   if((rd = atoi(argv[5]))==0)
      err_message(5);
   seed = atoi(argv[6]);
   strncpy(infilename, argv[7], sizeof(infilename));
   if(argc>8) debug_level = atoi(argv[8]);

/* INIT graph (now: testing)*/
   int pidx;
   for(pidx = 0; pidx<8; pidx++) {
     printf("%s ", argv[pidx]);
   }
   printf("\n");
#if 0
   n = 2048;
   for(i=0;i<n;i++) {
     wt[i] = 1;
     for(j=0;j<n;j++) {
       gr[i][j] = d(i,j);
     }
   }
#endif
   FILE *infile = fopen(infilename, "r");
   if(fscanf(infile, "%d",&n)==EOF)
     err_message(6);     /* second integer not used (EOF instead) */
   if(n==0) {
     printf("0 rows in matrix. exiting\n"); exit(0);
   }
   for(i=0;i<n;i++) {
     if(fscanf(infile, "%d",&wt[i])==EOF)
       err_message(7);
     for(j=0;j<n;j++)
       if(fscanf(infile, "%d",&gr[i][j])==EOF) 
         err_message(8);
   }

/* INITIALIZE */

 again: /* start of outer loop */

   weight = 0;
   srand48(time(0)%100000 + seed++);
   /* random vertices until n guesses does not go right */
   ant = 0;
   for(i=0;;i++) {
     temp = 0;
   anyo: if(temp>n) break;
     value[i] = lrand48()%n;
     for(j=0;j<i;j++)
       if(gr[value[i]][value[j]]<d1) {
         temp++;
         goto anyo;
       }
     weight += wt[value[i]];
     ant++;
   }

   i = 0;
   
   /* complete clique in a greedy manner */
   
   do {
     ok = -1;
     for(j=0;j<ant;j++)
       if(gr[value[j]][i]<d1) {
         ok = 0;
         break;
       }
     if(ok) {
       value[ant++] = i;
       weight += wt[i];
     }
     i++;
   } while(i<n);
   if(debug_level > 0) {
     printf("Initial weight/size: %d/%d\n",weight,ant);
   } 

   int ct = 0;
   timeout = 1;
   for(;;) {
     /*     if(timeout%5==0) 
	    printf("T: %d\n",timeout); */
     if(timeout>1000) {  /* now we are stuck, RESTART */
       count2 = 0;
       if(weight>max) 
         max = weight;
       goto again;
     }
     high = -1;
     nr = 0;
     if(ct<100) {   /* different nbrhood in the beginning, ct rounds */
       d3 = d1;
       ct++; 
     }
     else
       d3 = d2;
     for(;;) {
       i = (lrand48()%ant); /* remove vertex */  
       /* if(tabu[i]) continue; */
       /* find vertices (indices) that are close to the removed one */
       for(j=0;j<ant;j++)
         htab[j] = 0;
       ant2 = 0;
       weight2 = 0;
       for(j=0;j<ant;j++) {
	 if(gr[value[i]][value[j]]>d3)  continue;  /* order matters! */
         htab[j] = -1;
         ant2++;
         weight2 += wt[value[j]];
       }
       /*       printf("%d %d\n",ant2,weight2); for testing */
       /* run clique search */
       gsize = 0;
       for(j=0;j<n;j++) {
         /* if(j==value[i]) continue; */ /* don't include the specified vertex */
         ok = -1;
         for(k=0;k<ant;k++) {
           if(htab[k]) continue;
           if(gr[j][value[k]]<d1) {
             ok = 0;
             break;
	   }
	 }
         if(ok) {
           gtab[gsize++] = j;
	 }
       }
       if(gsize==0) continue;
       g = graph_new(gsize);
       for(j=0;j<gsize;j++) {
         g->weights[j] = wt[gtab[j]]; 
         for(k=j+1;k<gsize;k++)
           if(gr[gtab[j]][gtab[k]]>=d1)
             GRAPH_ADD_EDGE(g,j,k);
       }
       mx = clique_max_weight(g,clique_default_options);
       graph_free(g);
       total = mx-weight2;
       if(total > high) {
         nr = 1;
         ii[0] = i;
         jj[0] = mx;
         high = total;
       }
       else if(total==high) {
         ii[nr] = i;
         jj[nr++] = mx;
       }
       break;
     }
     posit = (lrand48()%nr);

/* MAKE CHANGE */

     for(j=0;j<ant;j++)
       htab[j] = 0;
     ant2 = 0;
     weight2 = 0;
     temp = 0;
     for(j=0;j<ant;j++) {
       if(gr[value[ii[posit]]][value[j]]>d3) {     /* order matters */
         v2[temp++] = value[j];  /* save */
         continue;
       }
       htab[j] = -1;
       ant2++;
       weight2 += wt[value[j]];
     }
     /* run clique search */
     gsize = 0;
     for(j=0;j<n;j++) {
       if(high==0&&j==value[ii[posit]]) continue; /* don't include specified vertex */
       ok = -1;
       for(k=0;k<ant;k++) {
         if(htab[k]) continue;
         if(gr[j][value[k]]<d1) {
           ok = 0;
           break;
     	   }
     	 }
       if(ok) {
         gtab[gsize++] = j;
     	 }
     }
     if(gsize==0) {
       timeout++;
       continue;
     }
     g = graph_new(gsize);
     for(j=0;j<gsize;j++) {
       g->weights[j] = wt[gtab[j]];
       for(k=j+1;k<gsize;k++)
         if(gr[gtab[j]][gtab[k]]>=d1)
           GRAPH_ADD_EDGE(g,j,k);
     }
     st = clique_find_single(g,jj[posit],jj[posit],FALSE,clique_default_options);
     graph_free(g);
     if(st==NULL) {
       timeout++;
       continue;
     }
     i = -1;
     while ((i=set_return_next(st,i)) >= 0) {
       v2[temp++] = gtab[i];
     }
     ant = temp;
     weight += high;
     for(i=0;i<ant;i++)
       value[i] = v2[i];
     /*     for(i=0;i<ant;i++)
       printf("%d ",value[i]);
       printf("\n"); */
    
     /* restart timout counter */
     timeout = 1;

     if(weight>=threshold) {
       /* YES! */
       printf("Yess!!! %d %d\n",ant,weight);
       for(i=0;i<ant;i++)
         printf("%d ",value[i]);
       printf("\n");
       snprintf(outfilename, sizeof(outfilename), "%s.yes", infilename);
       FILE *ofile = fopen(outfilename, "w+");
       fclose(ofile);
       exit(0);
     }
     if(++count%10 == 0) {
       if(debug_level >0) {
	 printf("%d/%d %d %d (max %d) %d/%d\n",count2,sim,ant,weight,max,a60,rct);
       } 
       if(count2++==sim) {
         count2 = 0;
         if(weight>max) 
           max = weight;
         if(weight>=threshold-1) a60++;
         if(++rct < rd)
           goto again;
         exit(0);
       }
     }
   }
}

