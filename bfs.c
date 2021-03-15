#include <stdlib.h>
#include <float.h>
typedef struct queue
{
  int item;
  struct queue *next;
} QUEUE;

QUEUE *head, *tail;

void print (void);

void init_queue (void);
void enqueue (int);
int dequeue (void);
void free_queue (void);
double aspl_host (void);
double aspl_host_fast (void);
double aspl_host2 (void);
int calc_n_switch_with_host (void);
void hostid_order_bfs (void);
void hostid_order_bfs2 (void);

void init_queue (void)
{
  head = tail = (QUEUE *)malloc(sizeof(QUEUE));
  if (head == NULL) exit(EXIT_FAILURE);  
}

void enqueue (int x)
{
  tail->item = x;
  tail->next = (QUEUE *)malloc(sizeof(QUEUE));
  if (tail->next == NULL) exit(EXIT_FAILURE);
  tail = tail->next;
}

int dequeue (void)
{
  int x;
  QUEUE *p;
  
  p = head;
  head = p->next;
  x = p->item;
  //free((char *)p);
  free(p);
  return x;
}

void free_queue (void)
{
  free(head);
}

int Diameter;

double aspl_host (void)
{
  int i, j, node;
  int start;
  int distance[M];
  unsigned long int sum = 0;
  int num_host_combination = 0;
  int diameter = 0;
  int weight_source, weight_dist;

  init_queue();
  for (start = 0; start < M; start++) {
	if (hdegree[start] == 0) continue;
	//init_queue();
	weight_source = hdegree[start];
	num_host_combination += weight_source * (weight_source - 1) / 2;
	sum += weight_source * (weight_source - 1);
	for (i = 0; i < M; i++) {
	  distance[i] = -1;
	}
	distance[start] = 0;

	enqueue(start);
	do {
	  i = dequeue();
	  for (j = 0; j < P; j++) {
		node = snode[i][j] - N;
		if (node < 0) continue;
		if (node != -1 && distance[node] < 0) {
		  enqueue(node);
		  distance[node] = distance[i] + 1;
 		} else if (node != -1) {
		  continue;
		} else {
		  break;
		}
	  }	  
	} while (head != tail);

	for (i = 0; i < M; i++) {
	  if (hdegree[i] == 0 || i == start || i < start) continue;
	  weight_dist = hdegree[i];
	  sum += (distance[i] + 2) * weight_source * weight_dist;
	  if (distance[i] != -1 && start < i) num_host_combination += weight_source * weight_dist;
	  if (distance[i] > diameter && weight_dist > 0) diameter = distance[i] + 2;
	}
  }

  //printf("sum %u\n", sum);
  Diameter = diameter;
  if (num_host_combination != (N * N - N) / 2)
	return DBL_MAX;
  else
	return (double)(2 * sum) / (double)(N * N - N);
}

double aspl_host_fast (void)
{
  int i, j, node;
  int start;
  int distance[M];
  unsigned long int sum = 0;
  int num_host_combination = 0;
  int diameter = 0;
  int weight_source, weight_dist;
  int n_enqueue;

  for (start = 0; start < M; start++) {
	if (hdegree[start] == 0) continue;
	weight_source = hdegree[start];
	num_host_combination += weight_source * (weight_source - 1) / 2;
	sum += weight_source * (weight_source - 1);
	//init_queue();
	for (i = 0; i < M; i++) {
	  distance[i] = -1;
	}
	distance[start] = 0;

	enqueue(start);
	n_enqueue = 1;
	for (;;) {
	  i = dequeue();
	  for (j = 0; j < P; j++) {
		node = snode[i][j] - N;
		if (node < 0) continue;
		if (node != -1 && distance[node] < 0) {
		  enqueue(node);
		  n_enqueue++;
		  distance[node] = distance[i] + 1;
		  if (n_enqueue == M) {
			while (head != tail) {
			  i = dequeue();
			}
			break;
		  }
 		} else if (node != -1) {
		  continue;
		} else {
		  break;
		}
	  }
	  if (n_enqueue == M || head == tail) break;
	}

	for (i = 0; i < M; i++) {
	  if (hdegree[i] == 0 || i == start || i < start) continue;
	  weight_dist = hdegree[i];
	  sum += (distance[i] + 2) * weight_source * weight_dist;
	  if (distance[i] != -1 && start < i) num_host_combination += weight_source * weight_dist;
	  if (distance[i] > diameter && weight_dist > 0) diameter = distance[i] + 2;
	}
  }

  Diameter = diameter;
  if (num_host_combination != (N * N - N) / 2)
	return DBL_MAX;
  else
	return (double)(2 * sum) / (double)(N * N - N);
}

int calc_n_switch_with_host (void)
{
  int i, n_switch_with_host;
  n_switch_with_host = 0;
  for (i = 0; i < M; i++) {
	if (hdegree[i] > 0) n_switch_with_host++;
  }
  return n_switch_with_host;
}

double aspl_host2 (void)//calculate the number of used switch nodes.
{
  int i, j, k, node;
  int start;
  int distance[M][M] = {};//distance[source][dist]
  unsigned long int sum = 0;
  int num_host_combination = 0;
  int diameter = 0;
  int weight_source, weight_dist;
  int n_enqueue;

  int n_switch_with_host = calc_n_switch_with_host();
  int used[M] = {};
  int n_used = 0;

  for (start = 0; start < M; start++) {
	if (hdegree[start] == 0) continue;
	if (hdegree[start] == P) return DBL_MAX;
	used[start] = 1;
	init_queue();
	weight_source = hdegree[start];
	num_host_combination += weight_source * (weight_source - 1) / 2;
	sum += weight_source * (weight_source - 1);
	for (i = 0; i < M; i++) {
	  distance[start][i] = -1;
	}
	distance[start][start] = 0;

	enqueue(start);
	n_enqueue = 1;
	do {
	  i = dequeue();
	  for (j = 0; j < P; j++) {
		node = snode[i][j] - N;
		if (node < 0) continue;
		if (node != -1 && distance[start][node] < 0) {
		  enqueue(node);
		  n_enqueue++;
		  distance[start][node] = distance[start][i] + 1;
 		} else if (node != -1) {
		  continue;
		} else {
		  break;
		}
	  }	 
	  if (n_enqueue == M) init_queue();
	} while (head != tail);
	  //} while (n_enqueue < M);

	for (i = 0; i < M; i++) {
	  if (hdegree[i] == 0 || i == start || i < start) continue;
	  weight_dist = hdegree[i];
	  sum += (distance[start][i] + 2) * weight_source * weight_dist;
	  if (distance[start][i] != -1 && start < i) num_host_combination += weight_source * weight_dist;
	  if (distance[start][i] > diameter && weight_dist > 0) diameter = distance[start][i] + 2;
	}
  }

  for (k = 0; k < M; k++) {
	for (i = 0; i < M; i++) {
	  if (used[k] == 1) break;
	  if (hdegree[i] == 0) continue;
	  for (j = 0; j < M; j++) {
		if (hdegree[j] == 0) continue;
		if (distance[i][j] == distance[i][k] + distance[j][k]) {
		  used[k] = 1;
		  break;
		}
	  }
	}
  }

  for (i = 0; i < M; i++) {
	n_used += used[i];
  }

  //printf("sum %u\n", sum);
  fprintf(stderr, "num_used %d\n", n_used);
  fprintf(stderr, "num_switch_with_host %d\n", n_switch_with_host);
  Diameter = diameter;
  if (num_host_combination != (N * N - N) / 2)
	return DBL_MAX;
  else
	return (double)(2 * sum) / (double)(N * N - N);
}

void hostid_order_bfs (void)
{
  int i, j, node;
  int start;
  int distance[M];
  int n_enqueue;

  int id = 0;
  int distance_now = 1;

  start = 0;
  while (hdegree[start] == 0) {
	start++;
  }
  
  //init_queue();
  for (i = 0; i < M; i++) {
	distance[i] = -1;
  }
  distance[start] = 0;

  enqueue(start);
  n_enqueue = 1;
  for (;;) {
	i = dequeue();
	for (j = 0; j < P; j++) {
	  node = snode[i][j] - N;
	  if (node < 0) continue;
	  if (node != -1 && distance[node] < 0) {
		enqueue(node);
		n_enqueue++;
		distance[node] = distance[i] + 1;
		if (n_enqueue == M) {
		  while (head != tail) {
			i = dequeue();
		  }
		  break;
		}
	  } else if (node != -1) {
		continue;
	  } else {
		break;
	  }
	}
	if (n_enqueue == M || head == tail) break;
  }

  for (i = 0; i < M; i++) {
  	printf("distance[%d] = %d\n", i, distance[i]);
  }

  for (i = 0; i < P; i++) {
	if (snode[start][i] < N) {
	  hnode[id] = start;
	  snode[start][i] = id;
	  id++;
	}
  }
  for (;;) {
	for (i = 0; i < M; i++) {
	  if (distance[i] == distance_now) {
		for (j = 0; j < P; j++) {
		  if (snode[i][j] < N) {
			hnode[id] = i;
			snode[i][j] = id;
			id++;
		  }
		}		
	  }
	}
	if (id >= N) break;
	distance_now++;
  }
  
  return;
}

void hostid_order_bfs2 (void)
{
  int i, j, k, node;
  int start;
  int distance[M];
  int n_enqueue;

  int id = 0;
  int distance_now = 1;

  start = 0;
  while (hdegree[start] == 0) {
	start++;
  }
  
  //init_queue();
  for (i = 0; i < M; i++) {
	distance[i] = -1;
  }
  distance[start] = 0;

  for (k = 0; k < P; k++) {
	if (snode[start][k] < N) {
	  hnode[id] = start;
	  snode[start][k] = id;
	  id++;
	}
  }

  enqueue(start);
  n_enqueue = 1;
  for (;;) {
	i = dequeue();
	for (j = 0; j < P; j++) {
	  node = snode[i][j] - N;
	  if (node < 0) continue;
	  if (node != -1 && distance[node] < 0) {
		enqueue(node);
		n_enqueue++;
		distance[node] = distance[i] + 1;
		for (k = 0; k < P; k++) {
		  if (snode[node][k] < N) {
			hnode[id] = node;
			snode[node][k] = id;
			id++;
		  }
		}
		if (n_enqueue == M) {
		  while (head != tail) {
			i = dequeue();
		  }
		  break;
		}
	  } else if (node != -1) {
		continue;
	  } else {
		break;
	  }
	}
	if (n_enqueue == M || head == tail) break;
  }

  for (i = 0; i < M; i++) {
  	printf("distance[%d] = %d\n", i, distance[i]);
  }

  for (i = 0; i < M; i++) {
	if (distance[i] == distance_now) {
	}
  }
  
  return;
}
