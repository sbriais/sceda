#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_flow.h>

#include <assert.h>

typedef struct {
  int flow;
  int cost;
} FlowCost;

FlowCost *new_FlowCost(int f, int c) {
  FlowCost *x = malloc(sizeof(FlowCost));
  x->flow = f;
  x->cost = c;
  return x;
}

void delete_FlowCost(FlowCost *x) {
  free(x);
}

void delete_string(char *s) {
  free(s);
}

int get_cost(SCEDA_Edge *e, void *ctxt) {
  return SCEDA_edge_get_data(FlowCost *,e)->cost;
}

int get_flow(SCEDA_Edge *e, void *ctxt) {
  return SCEDA_edge_get_data(FlowCost *,e)->flow;
}

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by Integer and edges are not labelled
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_FlowCost);

  SCEDA_Vertex *vS = SCEDA_graph_add_vertex(g, strdup("S"));
  SCEDA_Vertex *v1 = SCEDA_graph_add_vertex(g, strdup("V1"));
  SCEDA_Vertex *v2 = SCEDA_graph_add_vertex(g, strdup("V2"));
  SCEDA_Vertex *v3 = SCEDA_graph_add_vertex(g, strdup("V3"));
  SCEDA_Vertex *v4 = SCEDA_graph_add_vertex(g, strdup("V4"));
  SCEDA_Vertex *vT = SCEDA_graph_add_vertex(g, strdup("T"));

  // CLRS p 640
  SCEDA_graph_add_edge(g, vS, v1, new_FlowCost(16,0));
  SCEDA_graph_add_edge(g, vS, v2, new_FlowCost(13,0));

  SCEDA_graph_add_edge(g, v1, v2, new_FlowCost(10,1));
  SCEDA_graph_add_edge(g, v1, v3, new_FlowCost(12,0));

  SCEDA_graph_add_edge(g, v2, v1, new_FlowCost(4,1));
  SCEDA_graph_add_edge(g, v2, v4, new_FlowCost(14,0));

  SCEDA_graph_add_edge(g, v3, v2, new_FlowCost(9,0));
  SCEDA_graph_add_edge(g, v3, vT, new_FlowCost(20,0));

  SCEDA_graph_add_edge(g, v4, v3, new_FlowCost(7,0));
  SCEDA_graph_add_edge(g, v4, vT, new_FlowCost(4,0));
 
  SCEDA_HashMap *flow = SCEDA_graph_min_cost_max_flow(g, vS, vT, get_flow, NULL, get_cost, NULL);

  {
    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      int *f_e = SCEDA_hashmap_get(flow, e);
      fprintf(stdout,"%s -> %s : %d\n", SCEDA_vertex_get_data(char *, SCEDA_edge_source(e)), SCEDA_vertex_get_data(char *, SCEDA_edge_target(e)), *f_e);
    }      
    SCEDA_edges_iterator_cleanup(&edges);
  }   

  SCEDA_hashmap_delete(flow);

  SCEDA_graph_delete(g);

  return 0;
}
