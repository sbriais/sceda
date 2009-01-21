#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_mrc.h>
#include <SCEDA/graph_path.h>

#include <assert.h>

typedef struct {
  int cost;
  int time;
} CostTime;

CostTime *new_CostTime(int c, int t) {
  CostTime *x = malloc(sizeof(CostTime));
  x->cost = c;
  x->time = t;
  return x;
}

void delete_CostTime(CostTime *x) {
  free(x);
}

void delete_string(char *s) {
  free(s);
}

int get_cost(SCEDA_Edge *e, void *ctxt) {
  return -SCEDA_edge_get_data(CostTime *,e)->cost;
}

int get_time(SCEDA_Edge *e, void *ctxt) {
  return SCEDA_edge_get_data(CostTime *,e)->time;
}

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by Integer and edges are not labelled
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_CostTime);
 
  SCEDA_Vertex *vA = SCEDA_graph_add_vertex(g, strdup("A"));
  SCEDA_Vertex *vB = SCEDA_graph_add_vertex(g, strdup("B"));
  SCEDA_Vertex *vC = SCEDA_graph_add_vertex(g, strdup("C"));
  SCEDA_Vertex *vD = SCEDA_graph_add_vertex(g, strdup("D"));
  SCEDA_Vertex *vE = SCEDA_graph_add_vertex(g, strdup("E"));

  SCEDA_graph_add_edge(g, vA, vB, new_CostTime(3,1));
  SCEDA_graph_add_edge(g, vB, vC, new_CostTime(10,4));
  SCEDA_graph_add_edge(g, vC, vD, new_CostTime(5,2));
  SCEDA_graph_add_edge(g, vD, vA, new_CostTime(1,3));
  SCEDA_graph_add_edge(g, vB, vE, new_CostTime(3,2));
  SCEDA_graph_add_edge(g, vE, vC, new_CostTime(1,1));
  SCEDA_graph_add_edge(g, vA, vC, new_CostTime(9,2));
  SCEDA_graph_add_edge(g, vC, vA, new_CostTime(0,1));
  SCEDA_graph_add_edge(g, vC, vB, new_CostTime(-5,1));

  int p, q;
  SCEDA_graph_mrc(g, get_cost, NULL, get_time, NULL, &p, &q);
  fprintf(stdout,"%d / %d\n", p, q);

  SCEDA_graph_delete(g);

  return 0;
}
