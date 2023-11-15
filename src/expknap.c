
/* ======================================================================
	     EXPKNAP.c, David Pisinger   march 1992, modified april 1994
   ====================================================================== */

/* This is the C-code corresponding to the paper:
 *
 *   D. Pisinger
 *   An expanding-core algorithm for the exact 0-1 Knapsack Problem
 *   European Journal of Operational Research, 87 (1995), 175-187
 *
 * Further details on the project can also be found in
 *
 *   D. Pisinger
 *   Algorithms for Knapsack Problems
 *   Report 95/1, DIKU, University of Copenhagen
 *   Universitetsparken 1
 *   DK-2100 Copenhagen
 *
 * The current code is intended for performing extensive tests with
 * randomly generated instances. It should however be easy to derive
 * the "plain" expknap algorithm from the listing by stripping several
 * test routines.
 *
 * The code has been tested on a hp9000/735, and conforms with the
 * ANSI-C standard apart from some of the timing routines (which may
 * be removed). To compile the code use:
 *
 *   cc -Aa -O -o expknap expknap.c -lm
 * 
 * The code is run by issuing the command
 *
 *   expknap n r type
 *
 * where n: number of item_2s,
 *       r: range of coefficients, 
 *       type: 1=uncorr., 2=weakly corr., 3=strongly corr., 4=subset sum
 * output will be appended to the file "trace.exp".
 *
 * Please do not re-distribute. A new copy can be obtained by contacting
 * the author at the adress below. Errors and questions are refered to:
 *
 *   David Pisinger, associate professor
 *   DIKU, University of Copenhagen,
 *   Universitetsparken 1,
 *   DK-2100 Copenhagen.
 *   e-mail: pisinger@diku.dk
 *   fax: +45 35 32 14 01
 */


/* ======================================================================
	                          definitions
   ====================================================================== */

#if defined(_WIN32) || defined(_WIN64)
#include "..\include\expknap.h"
#else
#include "../include/expknap.h"
#endif

#define TESTS    50 /* Number of test instances to be generated */

//#include <iostream>
//#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <math.h>
//#include <malloc.h>
#include <time.h>
typedef long long int64;
typedef unsigned long long uint64;


/* ======================================================================
				     macros
   ====================================================================== */

#define srand(x)     srand(x)
//#define random(x)    (lrand48() % (x))
#define random(x)    rand() / RAND_MAX % (x)

#define DET(a1, a2, b1, b2)        ((a1) * (long) (b2) - (a2) * (long) (b1))
#define SWAP(a, b)             { register item_2 t; t = *a; *a = *b; *b = t; }
#define NO(f,i)                                            ((int) ((i+1)-f))
#define TRUE  1
#define FALSE 0
#define SORTSTACK 200
#define MIN_TEST_TIME 0.25
#define CONV_TO_MS 10000
#define MAX_SOLVE_TIME 10


/* ======================================================================
				 type declarations
   ====================================================================== */

///*typedef int  boolean;*/ /* boolean variables */
//typedef long itype;   /* item_2 profits and weights */
//typedef long  stype;   /* sum of pofit or weight */
//typedef int           BOOL; /* logical variable         */
//typedef int           boolean; /* logical variable         */
//
///* item_2 as seen externally */
//typedef struct {
//	itype   p;     /* profit */
//	itype   w;     /* weight */
//	BOOL x;     /* solution variable */
//} exitem_2;

///* item_2 as seen internally */
typedef struct {
	itype   p;     /* profit */
	itype   w;     /* weight */
	BOOL   *x;    /* pointer to original solution variable */
} item_2;

/* interval stack */
typedef struct istk {
  item_2    *f;
  item_2    *l;
  stype   ws;
} istack;


/* ======================================================================
				 global variables
   ====================================================================== */

item_2    *br;        /* break item_2 */
stype   wsb;        /* profit and wight sum up to break item_2 */
stype   psb;
stype   c;          /* total capacity */
itype   z;          /* lower bound */
item_2    *fsort;
item_2    *lsort;
istack  s;          /* sorted interval returned by partsort */

istack  *ihead1, *stack1, *iend1;  /* interval stack, i < br */
istack  *ihead2, *stack2, *iend2;  /* interval stack, i > br */
boolean **ehead, **estack;         /* exception stack */


/* ======================================================================
				 test variables
   ====================================================================== */

FILE *trace;
long iterations;
long heur;
long dantzig;
long touch;
long redu;
long sorts;


/* ======================================================================
                        timing routines for hp9000/735
   ====================================================================== */

/*#define  _INCLUDE_POSIX_SOURCE
#include <sys/times.h>
#include <unistd.h>


struct tms timestart, timeend;

void starttime(void)
{
  times(&timestart);
}

void endtime(long *time)
{
  double t1, t2;
  times(&timeend);
  t1 = (double) (timeend.tms_utime-timestart.tms_utime) / sysconf(_SC_CLK_TCK);
  t2 = (double) (timeend.tms_stime-timestart.tms_stime) / sysconf(_SC_CLK_TCK);
  *time = t1 * 1000;
}*/


stype expknap(exitem_2 *f, exitem_2 *l, stype cap);

/* ======================================================================
                                   sumdata
   ====================================================================== */

void sumdata(int n1, int r1, int type1, long iterations, long heur, long z,
         long c, long dantzig, long touch, long redu, long sorts, long time)
{
  static long n;
  static long r;
  static long t;
  static long iterates  = 0;
  static long touched   = 0;
  static long simpred   = 0;
  static long coresize  = 0;
  static long greedygap = 0;
  static long gap       = 0;
  static long tottime   = 0;
  static long zsum      = 0;
  static long csum      = 0;
  static double sqtime  = 0.0;
  double mean, variance, stddev;

  if (n1 == 0) {
    mean     = tottime / (1000 * (double) TESTS);
    variance = sqtime / TESTS - mean * mean;
    stddev   = sqrt(variance);
    fprintf(trace,"n           = %ld\n", n);
    fprintf(trace,"r           = %ld\n", r);
    fprintf(trace,"t           = %ld\n", t);
    fprintf(trace,"iterations  = %.0lf\n", iterates    / (double)    TESTS);
    fprintf(trace,"touched     = %.1lf\n", touched     / (double)    TESTS);
    fprintf(trace,"touchedpct  = %.1lf\n", 100*touched / ((double) n*TESTS));
    fprintf(trace,"simpreduced = %.1lf\n", simpred     / (double)    TESTS);
    fprintf(trace,"coresize    = %.0lf\n", coresize    / (double)    TESTS);
    fprintf(trace,"corepct     = %.2lf\n", 100*coresize/ ((double) n*TESTS));
    fprintf(trace,"greedygap   = %.1lf\n", greedygap   / (double)    TESTS);
    fprintf(trace,"gap         = %.1lf\n", gap         / (double)    TESTS);
    fprintf(trace,"zsum        = %.0lf\n", zsum        / (double)        1);
    fprintf(trace,"csum        = %.0lf\n", csum        / (double)        1);
    fprintf(trace,"time        = %.2lf\n", mean        / (double)        1);
    fprintf(trace,"variance    = %.2lf\n", variance    / (double)        1);
    fprintf(trace,"stddev      = %.2lf\n", stddev      / (double)        1);
  } else {
    n          = n1;
    r          = r1;
    t          = type1;
    iterates  += iterations;          /* iterations by branch-and-bound */
    touched   += touch;               /* variables touched by reduction */
    simpred   += redu;                /* variables reduced by reduction */
    coresize  += sorts;               /* sorted core size               */
    greedygap += z-heur;              /* gap for greedy bound           */
    gap       += dantzig-z;           /* gap between LP and IP optimum  */
    tottime   += time;                /* total computational time       */
    zsum       = ((zsum+z) % 1000);   /* controle sum of all solutions  */
    csum       = ((csum+c) % 1000);   /* controle sum of all capacities */
    sqtime    += (time / (double) 1000) * (time / (double) 1000);
  }
}


/* ======================================================================
        		         error
   ====================================================================== */

void error(char *str, ...)
{
  va_list args;

  va_start(args, str);
  vprintf(str, args); printf("\n");
  vfprintf(trace, str, args); fprintf(trace, "\n");
  va_end(args);
  printf("STOP !!!\n\n"); fprintf(trace, "STOP !!!\n\n");
  fclose(trace);
  exit(-1);
}


/* ======================================================================
				  palloc
   ====================================================================== */

void pfree_exp(void *p)
{
  if (p == NULL) error("freeing null");
  free(p);
}


void * palloc(size_t no, size_t each)
{
  long size;
  long *p;

  size = no * (long) each;
  if (size == 0) size = 1;
  if (size != (size_t) size) error("alloc too big %ld", size);
  p = (long int*) malloc(size);
  if (p == NULL) error("no memory size %ld", size); 
  return p;
}


/* ======================================================================
				showitem_2s
   ====================================================================== */

void showitem_2s(item_2 *f, item_2 *l)
{
  item_2 *i;
  stype ps, ws;

  printf("showitem_2s");
  ps = 0; ws = 0;
  for (i = f; i <= l; i++) {
    printf("%d: (%2hd,%2hd) %8lf %1hd\n", 
           (int) ((i+1)-f), i->p, i->w, i->p / (double) i->w, *(i->x));
    if (i->x) { ps += i->p; ws += i->w; }
  }
  printf("showitem_2s (%ld,%ld)", ps, ws);
}


/* ======================================================================
				maketest
   ====================================================================== */

stype maketest(exitem_2 *f, exitem_2 *l, int type, int r, int v)
{
  register exitem_2 *j;
  register stype sum;
  stype c;
  itype r1;

  sum = 0; r1 = r/10;
  for (j = f; j <= l; j++) {

    j->w = random(r) + 1;
    switch (type) {
      case 1: j->p = random(r) + 1;
	      break;
      case 2: j->p = random(2*r1+1) + j->w - r1;
	      if (j->p <= 0) j->p = 1;
	      break;
      case 3: j->p = j->w + 10;
	      break;
      case 4: j->p = j->w;
	      break;
    }

    sum += j->w;
  }
  c = sum / 2;
  return c;
}


/* ======================================================================
				testinstance
   ====================================================================== */

void testinstance(exitem_2 **f, exitem_2 **l, int n,
                  int r, int type, int v)
{
  exitem_2 *a;

  /* allocate space for test example */
  a = (exitem_2 *) palloc(n, sizeof(exitem_2));
  *f = a; *l = &a[n-1];

  /* make test instance */
  c = maketest(*f, *l, type, r, v);
}


/* ======================================================================
				freeinstance
   ====================================================================== */

void freeinstance(exitem_2 *f)
{
  pfree_exp(f);
}


/* ======================================================================
				definesolution
   ====================================================================== */

void definesolution(void)
{
	register boolean **j;

  for (j = ehead; j < estack; j++) **j = 1 - **j;
}


/* ======================================================================
				checksol
   ====================================================================== */

long checksol(exitem_2 *f, exitem_2 *l, long c, long z)
{
  register exitem_2 *i;
  register stype sump, sumw;

  sump = 0; sumw = 0;
  for (i = f; i <= l; i++) {
    if (i->x) { sump += i->p; sumw += i->w; }
  }
  /* printf("sump %ld, z %ld, sumw %ld, c %ld\n", sump, z, sumw, c); */
  if ((sumw > c) || (sump != z)) error("wrong solution"); 
  return sump;
}


/* ======================================================================
				pushe
   ====================================================================== */

void cleare()
{
  estack = ehead;
}


void pushe(item_2 *i)
{
  *estack = (boolean *)i->x;
  estack++;
}


/* ======================================================================
				pushi
   ====================================================================== */

void pushi(istack **stack, item_2 *f, item_2 *l, stype ws)
{
  register istack *pos;
  pos = *stack;
  pos->f = f; pos ->l = l; pos->ws = ws;
  (*stack)++;
  if ((pos == iend1) || (pos == iend2)) error("istack full");
}


/* =========================================================================
                                   reduce
   ========================================================================= */

void reduce(item_2 **f, item_2 **l)
{
  register item_2 *i, *j, *k;
  register itype pb, wb;
  register stype q;

  pb = br->p; wb = br->w;
  q = DET(z+1, c-wsb, pb, wb);
  i = *f; j = *l;
  if (i <= br) {
    k = fsort - 1;
    while (i <= j) {
      touch++;
      if (DET(-j->p, -j->w, pb, wb) < q) {
        redu++;
        SWAP(i, j); i++;       /* not feasible */
      } else {
        SWAP(j, k); j--; k--;  /* feasible */
      }
    }
    if (k == fsort - 1) { SWAP(*f, k); k--; } /* return at least one */
    *l = fsort - 1; *f = k + 1;
  } else {
    k = lsort + 1;
    while (i <= j) {
      touch++;
      if (DET(i->p, i->w, pb, wb) < q) {
        redu++;
        SWAP(i, j); j--;       /* not feasible */
      } else {
        SWAP(i, k); i++; k++;  /* feasible */
      }
    }
    if (k == lsort + 1) { SWAP(*l, k); k++; }  /* return at least one */
    *f = lsort + 1; *l = k - 1;
  }
}


/* ======================================================================
				partsort
   ====================================================================== */

void partsort(item_2 *f, item_2 *l, stype ws)
{
  register itype mp, mw;
  register item_2 *i, *j, *m;
  register stype wi;
  int d;

  d = l - f + 1;
  if (d > 1) {
    m = f + d / 2;
    if (DET(f->p, f->w, m->p, m->w) < 0) SWAP(f, m);
    if (d > 2) {
      if (DET(m->p, m->w, l->p, l->w) < 0) {
	SWAP(m, l);
	if (DET(f->p, f->w, m->p, m->w) < 0) SWAP(f, m);
      }
    }
  }
  if (d <= 3) {
    s.f = f; s.l = l; s.ws = ws;
  } else {
    mp = m->p; mw = m->w;
    i = f; j = l; wi = ws;
    for (;;) {
      do { wi += i->w; i++; } while (DET(i->p, i->w, mp, mw) > 0);
      do {                j--; } while (DET(j->p, j->w, mp, mw) < 0);
      if (i > j) break;
      SWAP(i, j);
    }
    if (wi > c) {
      pushi(&stack2, i,   l, wi); partsort(f, i-1, ws);
    } else {
      pushi(&stack1, f, i-1, ws); partsort(i,   l, wi);
    }
  }
}


/* ======================================================================
				sorti
   ====================================================================== */

boolean sorti(istack **stack)
/* returns TRUE if expansion succeeded, FALSE if no more intervals */
{
  register istack *pos;

  if ((*stack == ihead1) || (*stack == ihead2)) return FALSE;
  (*stack)--;
  pos = *stack;
  reduce(&(pos->f), &(pos->l));
  partsort(pos->f, pos->l, pos->ws);
  if (s.f < fsort) fsort = s.f;
  if (s.l > lsort) lsort = s.l;
  return TRUE;
}


/* ======================================================================
				elebranch
   ====================================================================== */

short elebranch(itype ps, itype ws, item_2 *s, item_2 *t, struct timespec *start)
{
  short improved;

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  if ((now.tv_sec - start->tv_sec) + (now.tv_nsec - start->tv_nsec) / 1000000000.0 > MAX_SOLVE_TIME)
  {
	  return -1;
  }
  
  iterations++;
  improved = 0;
  if (ws <= 0) {
    if (ps > z) {
      improved = 1;
      z = ps;
      cleare();
    }
    for (;;) {
      if (t > lsort) { if (!sorti(&stack2)) break; }
      if (DET(ps-(z+1), ws, t->p, t->w) < 0) break;
	  short out = elebranch(ps + t->p, ws + t->w, s, t+1, start);
	  if (out < 0) {
		  return -1;
	  }
      if (out > 0) {
		improved = 1; pushe(t);
      }
      t++;
    }
  } else {
    for (;;) {
      if (s < fsort) { if (!sorti(&stack1)) break; }
      if (DET(ps-(z+1), ws, s->p, s->w) < 0) break;
	  short out = elebranch(ps - s->p, ws - s->w, s-1, t, start);
	  if (out < 0) {
		  return -1;
	  }
      if (out > 0) {
		improved = 1; pushe(s);
      }
      s--;
    }
  }
  return improved;
}


/* ======================================================================
				heuristic
   ====================================================================== */

stype heuristic(item_2 *f, item_2 *l)
{
  register item_2 *i;
  register stype ps, ws;
  register itype r, z, pb, dz;

  ps = 0; ws = c;
  for (i = f; i->w <= ws; i++) {
    ws -= i->w;
    ps += i->p;
    *(i->x) = 1;
  }
  br = i; wsb = c-ws; psb = ps;

  /* determine dantzig bound, and use it as upper bound on z */
  dz = (c - wsb) * br->p / br->w;
  dantzig = psb + dz;

  /* define initial solution */
  cleare();
  z = 0; if (z == dz) return z;

  /* forward greedy solution */
  r = c - wsb;
  for (i = br; i <= l; i++) {
    if ((i->w <= r) && (i->p > z)) {
      cleare(); pushe(i);
      z = i->p; if (z == dz) return z;
    }
  }

  /* backward greedy solution */
  r = (wsb + br->w) - c; pb = br->p;
  for (i = br-1; i >= f; i--) {
    if ((i->w >= r) && (pb - i->p > z)) {
      cleare(); pushe(br); pushe(i);
      z = pb - i->p; if (z == dz) return z;
    }
  }

  return z;
}


/* ======================================================================
				expknap
   ====================================================================== */

stype expknap(exitem_2 *f, exitem_2 *l, stype cap)
{
  register item_2 *j;
  register exitem_2 *i;
  item_2 *fitem_2, *litem_2;
  int n;

  n = l-f+1;
  c = cap;
  ihead1 = (istack   *) palloc(SORTSTACK, sizeof(istack));
  ihead2 = (istack   *) palloc(SORTSTACK, sizeof(istack));
  ehead = (boolean **)palloc(n, sizeof(boolean *));
  fitem_2  = (item_2     *) palloc(n, sizeof(item_2));
  litem_2  = fitem_2 + n - 1;
  stack1 = ihead1; iend1 = ihead1 + SORTSTACK;
  stack2 = ihead2; iend2 = ihead2 + SORTSTACK;

  /* copy item_2s to own array */
  for (i = f, j = fitem_2; i <= l; i++, j++) {
    j->p = i->p; j->w = i->w; j->x = &(i->x); i->x = 0;
  }

  /* find break item_2 */
  partsort(fitem_2, litem_2, 0);
  fsort = s.f; lsort = s.l;

  /* solve problem */
  z = heuristic(fitem_2, litem_2);
  heur = z + psb;
  
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  
  short out;
  out = elebranch(0, wsb-c, br-1, br, &start);
  if (out < 0) {
	  return -1;
  }

  /* define solution */
  definesolution();

  pfree_exp(ihead1);
  pfree_exp(ihead2);
  pfree_exp(ehead);
  pfree_exp(fitem_2);
  sorts = lsort - fsort + 1;
  return z + psb;
}
