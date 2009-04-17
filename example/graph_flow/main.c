#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_flow.h>

typedef struct {
  int value;
} Integer;

Integer *new_Integer(int n) {
  Integer *x = malloc(sizeof(Integer));
  x->value = n;
  return x;
}

void delete_Integer(Integer *x) {
  free(x);
}

void delete_string(char *s) {
  free(s);
}

int get_capacity(SCEDA_Edge *e, void *ctxt) {
  return SCEDA_edge_get_data(Integer *,e)->value;
}

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by strings and edges are labelled by Integer
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_Integer);
 
  SCEDA_Vertex *vS = SCEDA_graph_add_vertex(g, strdup("S"));
  SCEDA_Vertex *v1 = SCEDA_graph_add_vertex(g, strdup("V1"));
  SCEDA_Vertex *v2 = SCEDA_graph_add_vertex(g, strdup("V2"));
  SCEDA_Vertex *v3 = SCEDA_graph_add_vertex(g, strdup("V3"));
  SCEDA_Vertex *v4 = SCEDA_graph_add_vertex(g, strdup("V4"));
  SCEDA_Vertex *vT = SCEDA_graph_add_vertex(g, strdup("T"));

  // CLRS p 640
  SCEDA_graph_add_edge(g, vS, v1, new_Integer(16));
  SCEDA_graph_add_edge(g, vS, v2, new_Integer(13));

  SCEDA_graph_add_edge(g, v1, v2, new_Integer(10));
  SCEDA_graph_add_edge(g, v1, v3, new_Integer(12));

  SCEDA_graph_add_edge(g, v2, v1, new_Integer(4));
  SCEDA_graph_add_edge(g, v2, v4, new_Integer(14));

  SCEDA_graph_add_edge(g, v3, v2, new_Integer(9));
  SCEDA_graph_add_edge(g, v3, vT, new_Integer(20));

  SCEDA_graph_add_edge(g, v4, v3, new_Integer(7));
  SCEDA_graph_add_edge(g, v4, vT, new_Integer(4));

  //

  SCEDA_HashMap *flow = SCEDA_graph_max_flow(g, vS, vT, get_capacity, NULL);

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
