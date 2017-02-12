#include "limit.h"
#include "graph.h"
#include "staticgreedy.h"
#include "general_cascade.h"
#include "staticgreedy_directed_new.h"
#include "staticgreedy_basic.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include <ios>
#include <unistd.h>
#include <string.h>

using namespace std;

double toSimulateOnce(int *seeds, int set_size, double (*Run)(int num_iter, int size, int set[]))
{
  int set[MAX_NODE];
  int t;

  for (t = 0; t < set_size; t++)
  {
    set[t] = seeds[t];
  }

  return Run(10000, t, set);
}


int main(int argc, char * argv[])
{
  time_t t;
  srand((unsigned) time(&t));

  //input parameters
  if (argc < 4) {
    printf("Incomplete parameters, four papameters is needed, including:\n");
    printf("argv[1]: full file_path\n");
    printf("argv[2]: seed set size\n");
    printf("argv[3]: R for staticgreedy\n");
    printf("argv[4]: kind of algorithm, bsg for basic staticgreedy, sgd for staticgreedy with dynamic update on directed graph\n");
    printf("example: ./main hep.txt 50 100 bsg\n");
    return 0;
  }

  char file_path[50] = "";
  strcat(file_path, argv[1]);
  int set_size = atoi(argv[2]);
  int R = atoi(argv[3]);
  string algo = argv[4];

  //build graph
  Graph::BuildFromFile2GIC(file_path);
  GeneralCascade::Build();

  char outfile[100];
  long time;
  double influence;
  int *seeds;

  string s; 
  s="bsg";//basic static greedy

  if(!s.compare(algo)){
    sprintf(outfile, "BasicStaticGreedy_R%d_k%d.txt", R, set_size);

    int start_time = clock();
    seeds = BasicStaticGreedy::GetSeeds(R, set_size);
    int end_time = clock();
    time = end_time - start_time;

    influence = toSimulateOnce(seeds, set_size, GeneralCascade::Run);
  }

  s="sgd";//static greedy with dynamic update on directed graph
  if(!s.compare(algo)){
    sprintf(outfile, "StaticGreedyUD_SGD_R%d_k%d.txt", R, set_size);
    
    int start_time = clock();
    seeds = StaticGreedyDirectedNew::GetSeedsForDirectedG(R, set_size);
    int end_time = clock();
    time = end_time - start_time;

    influence = toSimulateOnce(seeds, set_size, GeneralCascade::Run);
  }

  //output result
  FILE* out = fopen(outfile, "w");
  fprintf(out,"BasicStaticGreedy config: R=%d\n", R);
  fprintf(out,"Time-consuming: T=%f sec\n", (double)(time)/CLOCKS_PER_SEC);
  fprintf(out,"Influence-spread: I=%f\n",influence);
  fprintf(out,"Seeds:\n");
  for (int i = 0; i < set_size; i++) {
      fprintf(out, "%d\n", seeds[i]);
  }
  fclose(out);

  return 1;
}
