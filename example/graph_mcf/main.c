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
  // create a graph whose nodes are labelled by strings and edges are labelled by FlowCost structure
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_FlowCost);

  SCEDA_Vertex *vS = SCEDA_graph_add_vertex(g, strdup("S"));

  SCEDA_Vertex *vA1 = SCEDA_graph_add_vertex(g, strdup("A1"));
  SCEDA_Vertex *vA2 = SCEDA_graph_add_vertex(g, strdup("A2"));
  SCEDA_Vertex *vA3 = SCEDA_graph_add_vertex(g, strdup("A3"));
  SCEDA_Vertex *vA4 = SCEDA_graph_add_vertex(g, strdup("A4"));
  SCEDA_Vertex *vA5 = SCEDA_graph_add_vertex(g, strdup("A5"));

  SCEDA_Vertex *vB1 = SCEDA_graph_add_vertex(g, strdup("B1"));
  SCEDA_Vertex *vB2 = SCEDA_graph_add_vertex(g, strdup("B2"));
  SCEDA_Vertex *vB3 = SCEDA_graph_add_vertex(g, strdup("B3"));
  SCEDA_Vertex *vB4 = SCEDA_graph_add_vertex(g, strdup("B4"));
  SCEDA_Vertex *vB5 = SCEDA_graph_add_vertex(g, strdup("B5"));

  SCEDA_Vertex *vT = SCEDA_graph_add_vertex(g, strdup("T"));

  // We encode the following assignment problem:
  //     B1 B2 B3 B4 B5
  //  A1  7  8  7 15  4
  //  A2  7  9 17 14 10
  //  A3  9  6 12  6  7
  //  A4  7  6 14  6 10
  //  A5  9  6 12 10  6

  SCEDA_graph_add_edge(g, vS, vA1, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vS, vA2, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vS, vA3, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vS, vA4, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vS, vA5, new_FlowCost(1,0));

  SCEDA_graph_add_edge(g, vB1, vT, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vB2, vT, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vB3, vT, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vB4, vT, new_FlowCost(1,0));
  SCEDA_graph_add_edge(g, vB5, vT, new_FlowCost(1,0));

  SCEDA_graph_add_edge(g, vA1, vB1, new_FlowCost(1,7));
  SCEDA_graph_add_edge(g, vA1, vB2, new_FlowCost(1,8));
  SCEDA_graph_add_edge(g, vA1, vB3, new_FlowCost(1,7));
  SCEDA_graph_add_edge(g, vA1, vB4, new_FlowCost(1,15));
  SCEDA_graph_add_edge(g, vA1, vB5, new_FlowCost(1,4));

  SCEDA_graph_add_edge(g, vA2, vB1, new_FlowCost(1,7));
  SCEDA_graph_add_edge(g, vA2, vB2, new_FlowCost(1,9));
  SCEDA_graph_add_edge(g, vA2, vB3, new_FlowCost(1,17));
  SCEDA_graph_add_edge(g, vA2, vB4, new_FlowCost(1,14));
  SCEDA_graph_add_edge(g, vA2, vB5, new_FlowCost(1,10));

  SCEDA_graph_add_edge(g, vA3, vB1, new_FlowCost(1,9));
  SCEDA_graph_add_edge(g, vA3, vB2, new_FlowCost(1,6));
  SCEDA_graph_add_edge(g, vA3, vB3, new_FlowCost(1,12));
  SCEDA_graph_add_edge(g, vA3, vB4, new_FlowCost(1,6));
  SCEDA_graph_add_edge(g, vA3, vB5, new_FlowCost(1,7));

  SCEDA_graph_add_edge(g, vA4, vB1, new_FlowCost(1,7));
  SCEDA_graph_add_edge(g, vA4, vB2, new_FlowCost(1,6));
  SCEDA_graph_add_edge(g, vA4, vB3, new_FlowCost(1,14));
  SCEDA_graph_add_edge(g, vA4, vB4, new_FlowCost(1,6));
  SCEDA_graph_add_edge(g, vA4, vB5, new_FlowCost(1,10));

  SCEDA_graph_add_edge(g, vA5, vB1, new_FlowCost(1,9));
  SCEDA_graph_add_edge(g, vA5, vB2, new_FlowCost(1,6));
  SCEDA_graph_add_edge(g, vA5, vB3, new_FlowCost(1,12));
  SCEDA_graph_add_edge(g, vA5, vB4, new_FlowCost(1,10));
  SCEDA_graph_add_edge(g, vA5, vB5, new_FlowCost(1,6));

  SCEDA_HashMap *flow = SCEDA_graph_min_cost_max_flow(g, vS, vT, get_flow, NULL, get_cost, NULL);

  {
    int cost = 0;

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      int *f_e = SCEDA_hashmap_get(flow, e);
      fprintf(stdout,"%s -> %s : %d\n", SCEDA_vertex_get_data(char *, SCEDA_edge_source(e)), SCEDA_vertex_get_data(char *, SCEDA_edge_target(e)), *f_e);
      cost += *f_e * get_cost(e, NULL);
    }      
    SCEDA_edges_iterator_cleanup(&edges);

    fprintf(stdout,"total cost = %d\n",cost);
  }   

  SCEDA_hashmap_delete(flow);

  SCEDA_graph_delete(g);

  return 0;
}
