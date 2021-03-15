#define N 1024
#define M 194
#define P 15
//#define REGULAR
#define MAXFILENAME 20
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MT.h"

int hnode[N];//host nodes
int snode[M][P] = {};//switch nodes
int degree[M] = {};//degree of each switch
int hdegree[M] = {};//number of connected host nodes
int hnode_bak[N], snode_bak[M][P], hdegree_bak[M];
/* host nodes: #0-#N-1, switch nodes: #N-#N+M-1 */

double ASPL;
#include "bfs.c"

/* Functions */
void init (void);
void init_regular (void);
void init_near_regular (void);
void print (void);
void remove_loop (int, int*);
void hostid_order (void);

int main ()
{
#ifdef REGULAR
  init_regular();
#else
  init_near_regular();
#endif
  //print();

/*   init_queue(); */
/* #ifdef REGULAR */
/*   sa_main(ASPL); */
/* #else */
/*   sa_2neighbor(ASPL); */
/* #endif */
/*   free_queue(); */

/*   init_queue(); */
/*   hostid_order_bfs2(); */
/*   free_queue(); */
/*   print(); */
  //generate_for_simgrid();
  return EXIT_SUCCESS;
}

void init (void)//fully random
{
  int i, j, k, l;
  int u, v;
  int loop[M] = {};

  if ((M * P - N) % 2 != 0) {
	fprintf(stderr, "Number of free ports must be even.\n");
	exit(EXIT_FAILURE);
  }
  init_genrand((unsigned int)time(NULL));
  /* Generate random regular host-switch graph */
  /* STEP1: randomly add edges */
  // connect host-switch
  for (i = 0; i < N; i++) {
	u = genrand_int31() % M;
	if (degree[u] >= P) {
	  i--;
	  continue;
	}
	hnode[i] = u;
	snode[u][degree[u]] = i;
	degree[u]++;
	hdegree[u]++;
  }
  // connect switch-switch
  for (i = 0; i < (M * P - N) / 2; i++) {
	u = genrand_int31() % M;
	v = genrand_int31() % M;
	if (degree[u] == P || degree[v] == P || ((u == v) && (degree[u] > P - 2))) {
		i--;
		continue;
	}
	if (u == v) loop[u]++;
	snode[u][degree[u]] = v + N;
	degree[u]++;
	snode[v][degree[v]] = u + N;
	degree[v]++;
  }
  //print();
  /* STEP2: remove loops */
  for (i = 0; i < M; i++) {	
  	while (loop[i] > 0) {
	  printf("remove_loop %d\n", i);
  	  remove_loop(i, loop);
  	}
  }
  //print();
  ASPL = aspl_host();
  printf("aspl: %f\n", ASPL);
}

void remove_loop (int u, int* loop)
{
  int v1, v2;
  int i, j;

  if (loop[u] <= 0) {
	fprintf(stderr, "Error in remove_loop\n");
	exit(EXIT_FAILURE);
  }
  for (;;) {
	v1 = genrand_int31() % M;
	//if (hdegree[v1] > 0) continue; //nn????
	v2 = snode[v1][genrand_int31() % P] - N;
	if (v1 != u && v2 >= 0 && v2 != u) break;
  }

  for (i = 0; i < P; i++) {
	if (snode[v1][i] == v2 + N) {
	  snode[v1][i] = u + N;
	  break;
	}
  }
  for (i = 0; i < P; i++) {
	if (snode[v2][i] == v1 + N) {
	  snode[v2][i] = u + N;
	  break;
	}
  }
  for (i = 0; i < P; i++) {
	if (snode[u][i] == u + N) {
	  snode[u][i] = v1 + N;
	  break;
	}
  }
  for (i = 0; i < P; i++) {
	if (snode[u][i] == u + N) {
	  snode[u][i] = v2 + N;
	  break;
	}
  }
  printf("u %d v1 %d v2 %d\n", u, v1, v2);
  loop[u] -= 2;
  if (v1 == v2) loop[v1] -= 2;
}

void print (void)
{
  int i, j;
  for (i = 0; i < M; i++) {
	printf("\ns%d(d%d h%d): ", i, degree[i], hdegree[i]);
	for (j = 0; j < P; j++) {
	  if (snode[i][j] < N) {
		printf("h%d ", snode[i][j]);
	  } else {
		printf("s%d ", snode[i][j] - N);
	  }
	}
  }
  printf("\nASPL: %f\n", aspl_host());
}

void init_regular (void)//regular
{
  int i, j, k, l;
  int u, v;
  int loop[M] = {};
  int hosts_per_switch;

  if ((M * P - N) % 2 != 0) {
	fprintf(stderr, "Number of free ports must be even.\n");
	exit(EXIT_FAILURE);
  }
  if (N % M != 0) {
	fprintf(stderr, "N / M must be int.\n");
	exit(EXIT_FAILURE);
  } else {
	hosts_per_switch = N / M;
  }
  init_genrand((unsigned int)time(NULL));
  /* Generate random host-switch graph */
  /* STEP1: randomly add edges */
  // connect host-switch
  for (i = 0; i < N; i++) {
	snode[i / hosts_per_switch][i % hosts_per_switch] = i;
	hnode[i] = i / hosts_per_switch;
	degree[i / hosts_per_switch]++;
	hdegree[i / hosts_per_switch]++;
  }
	  
  // connect switch-switch
  for (i = 0; i < (M * P - N) / 2; i++) {
	u = genrand_int31() % M;
	v = genrand_int31() % M;
	if (degree[u] == P || degree[v] == P || ((u == v) && (degree[u] > P - 2))) {
		i--;
		continue;
	}
	if (u == v) loop[u]++;
	snode[u][degree[u]] = v + N;
	degree[u]++;
	snode[v][degree[v]] = u + N;
	degree[v]++;
  }
  //print();
  /* STEP2: remove loops */
  for (i = 0; i < M; i++) {
  	while (loop[i] > 0) {
  	  remove_loop(i, loop);
  	}
  }
  //print();
  ASPL = aspl_host();
  printf("aspl: %f\n", ASPL);
}

void init_near_regular (void)
{
  int i, j, k, l;
  int u, v;
  int loop[M] = {};
  int hosts_per_switch;
  int rest = N;

  if ((M * P - N) % 2 != 0) {
	fprintf(stderr, "Number of free ports must be even.\n");
	exit(EXIT_FAILURE);
  }
  hosts_per_switch = N / M;
  
  init_genrand((unsigned int)time(NULL));
  /* Generate random host-switch graph */
  /* STEP1: randomly add edges */
  // connect host-switch
  for (i = 0; i < hosts_per_switch * M; i++) {
	snode[i / hosts_per_switch][i % hosts_per_switch] = i;
	hnode[i] = i / hosts_per_switch;
	degree[i / hosts_per_switch]++;
	hdegree[i / hosts_per_switch]++;
	rest--;
  }
  //printf("rest %d i %d\n", rest, i);
  for (j = 0; j < rest; j++) {
	//printf("j %d i %d hosts_per_switch %d\n", j, i, hosts_per_switch);
	snode[j][hosts_per_switch] = i;
	hnode[i++] = j;
	degree[j]++;
	hdegree[j]++;
  }

  // connect switch-switch
  for (i = 0; i < (M * P - N) / 2; i++) {
	u = genrand_int31() % M;
	v = genrand_int31() % M;
	if (degree[u] == P || degree[v] == P || ((u == v) && (degree[u] > P - 2))) {
		i--;
		continue;
	}
	if (u == v) loop[u]++;
	snode[u][degree[u]] = v + N;
	degree[u]++;
	snode[v][degree[v]] = u + N;
	degree[v]++;
  }
  //print();
  /* STEP2: remove loops */
  for (i = 0; i < M; i++) {
  	while (loop[i] > 0) {
  	  remove_loop(i, loop);
  	}
  }
  //print();
  ASPL = aspl_host();
  printf("aspl: %f\n", ASPL);
}

void hostid_order ()
{
  int i, j;
  int id = 0;

  for (i = 0; i < M; i++) {
	for (j = 0; j < P; j++) {
	  if (snode[i][j] < N) {
		hnode[id] = i;
		snode[i][j] = id;
		id++;
	  }
	}
  }
}
