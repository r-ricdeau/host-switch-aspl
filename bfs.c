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
double aspl_host_fast (void);

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

double aspl_host_fast (void)
{
  int i, j, node;
  int start;
  int distance[M];
  unsigned long long int sum = 0;
  unsigned long long int num_host_combination = 0;
  int diameter = 0;
  int weight_source, weight_dist;
  int n_enqueue;

  for (start = 0; start < M; start++) {
	if (hdegree[start] == 0) continue;
	weight_source = hdegree[start];
	num_host_combination += weight_source * (weight_source - 1) / 2;
	sum += weight_source * (weight_source - 1);
	for (i = 0; i < M; i++) {
	  distance[i] = -1;
	}
	distance[start] = 0;

	enqueue(start);
	n_enqueue = 1;
	for (;;) {
	  i = dequeue();
	  for (j = 0; j < R; j++) {
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
	  if ((distance[i] + 2 > diameter) && (weight_dist > 0)) diameter = distance[i] + 2;
	}
  }

  Diameter = diameter;
  if (num_host_combination != (unsigned long long int)(N * (N - 1)) / 2)
    return DBL_MAX;
  else
    return (double)(sum) / (double)(num_host_combination);
}
