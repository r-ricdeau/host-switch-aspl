#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

int *hnode;
int **snode;
int *degree;
int *hdegree;

int N, M, R;
int Diameter;
double ASPL;
#include "bfs.c"

int main (int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Usage:\n\t./a.out txtfile\n");
    exit(EXIT_FAILURE);
  }

  std::ifstream ifs(argv[1]);
  std::string s;
  int line = 0, word = 0, id;

  
  if (!ifs) {
    fprintf(stderr, "File not exist.\n");
    exit(EXIT_FAILURE);
  }

  int node[2];
  while (ifs && getline(ifs, s)) {
    std::istringstream iss(s);
    std::string tmp;
    word = 0;
    while (getline(iss, tmp, ' ')) {
      sscanf(tmp.c_str(), "%d", &id);      
      if (line == 0) {
	if (word == 0) {
	  N = id;
	} else if (word == 1) {
	  M = id;
	} else if (word == 2) {
	  R = id;
	}	
      } else {
	assert(word == 0 || word == 1);
	node[word] = id;
      }
      word++;
    }    

    if (line > 0) {
      assert((node[0] < N + M) && (node[1] < N + M));
      if (node[0] > node[1]) {
	id = node[0];
	node[0] = node[1];
	node[1] = id;
      }
      assert(node[1] >= N); 
      if (node[0] < N) {
	hnode[node[0]] = node[1];
	snode[node[1] - N][degree[node[1] - N]] = node[0];
	degree[node[1] - N]++;
	hdegree[node[1] - N]++;
	//printf("host-switch connect h%d s%d (%d, %d)\n", node[0], node[1] - N, hdegree[node[1] - N], degree[node[1] - N]);
      } else {
	snode[node[0] - N][degree[node[0] - N]] = node[1];
	snode[node[1] - N][degree[node[1] - N]] = node[0];
	degree[node[0] - N]++;
	degree[node[1] - N]++;
	//printf("switch-switch connect %d (%d) %d (%d)\n", node[0] - N, degree[node[0] - N], node[1] - N, degree[node[1] - N]);
      }
    } else {
      hnode = new int[N];
      snode = new int*[M];
      for (int i = 0; i < M; i++) {
	snode[i] = new int[R];
      }
      degree = new int[M];
      hdegree = new int[M];	      
    }
    line++;
  }
  ifs.close();
  
  init_queue();
  printf("%lf\n", aspl_host_fast());
  printf("%d\n", Diameter);
  free_queue();

  delete[] hnode;
  for (int i = 0; i < M; i++) {
    delete[] snode[i];
  }
  delete[] degree;
  delete[] hdegree;

  return EXIT_SUCCESS;
}
