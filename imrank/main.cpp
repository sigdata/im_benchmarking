#include "limit.h"
#include "graph.h"
#include "IMRank.h"
#include "general_cascade.h"
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

double toSimulateOnce(int (*GetNode)(int i), int set_size, double (*Run)(int num_iter, int size, int set[]))
{
  int set[MAX_NODE];
  int t;

  for (t = 0; t < set_size; t++)
  {
    set[t] = GetNode(t);
  }

  return Run(10000, t, set);
}

int main(int argc, char * argv[])
{
  time_t t;
  srand((unsigned) time(&t));

  //input parameters
  if (argc < 5) {
    printf("Incomplete parameters, four papameters is needed, including:\n");
    printf("argv[1]: full file_path\n");
    printf("argv[2]: seed set size\n");
    printf("argv[3]: l for IMRank\n");
    printf("argv[4]: MAX_LOOP for IMRank\n");
    printf("argv[5]: inital_ranking for IMRank, candidates are Degree,PageRank,WeightedDegree,Random,DegreeInverse\n");
    printf("example: ./main hep.txt 50 1 10 Degree\n");
    return 0;
  }

  char file_path[50] = "";
  strcat(file_path, argv[1]);
  int set_size = atoi(argv[2]);
  int l = atoi(argv[3]);
  int max_loop = atoi(argv[4]);
  char initial_ranking[50] = "";
  strcat(initial_ranking, argv[5]);

  //build graph
  Graph::BuildFromFile2GIC(file_path);
  GeneralCascade::Build();

  //run imrank
  char outfile[100];
  sprintf(outfile, "IMRank_k%d_l%d_LOOP%d_ir%s.txt", set_size, l, max_loop, initial_ranking);
  long time;
  double influence;
  int *seeds;

  int start_time = clock();
  imrank::Build(initial_ranking, l, max_loop, set_size);
  int end_time = clock();
  time = end_time - start_time;
  influence = toSimulateOnce(imrank::GetNode, set_size, GeneralCascade::Run);

  //output result
  FILE* out = fopen(outfile, "w");
  fprintf(out,"IMRank configs: l=%d MAX_LOOP=%d inital_ranking=%s\n", l, max_loop, initial_ranking);
  fprintf(out,"Time-consuming: T=%f sec\n", (double)(time)/CLOCKS_PER_SEC);
  fprintf(out,"Influence-spread: I=%f\n",influence);
  fprintf(out,"Seeds:\n");
  for (int i = 0; i < set_size; i++) {
      fprintf(out, "%d\n", imrank::GetNode(i));
  }
  fclose(out);

  return 1;
}
