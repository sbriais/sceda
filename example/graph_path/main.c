#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_path.h>

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

SCEDA_COST_TYPE get_distance(SCEDA_Edge *e, void *ctxt) {
  return SCEDA_edge_get_data(Integer *,e)->value;
}

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by Integer and edges are not labelled
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_Integer);
 
  SCEDA_Vertex *vA = SCEDA_graph_add_vertex(g, strdup("A"));
  SCEDA_Vertex *vB = SCEDA_graph_add_vertex(g, strdup("B"));
  SCEDA_Vertex *vC = SCEDA_graph_add_vertex(g, strdup("C"));
  SCEDA_Vertex *vD = SCEDA_graph_add_vertex(g, strdup("D"));
  SCEDA_Vertex *vE = SCEDA_graph_add_vertex(g, strdup("E"));

  SCEDA_graph_add_edge(g, vA, vB, new_Integer(5));
  SCEDA_graph_add_edge(g, vB, vC, new_Integer(-1));
  SCEDA_graph_add_edge(g, vB, vD, new_Integer(2));
  SCEDA_graph_add_edge(g, vC, vD, new_Integer(3));
  SCEDA_graph_add_edge(g, vC, vA, new_Integer(-2));

  SCEDA_Vertex *origin = vA;
  
  SCEDA_Vertex *cycle = NULL;

  SCEDA_HashMap *shortest_paths = SCEDA_graph_shortest_path_bellman_ford(g, origin, get_distance, NULL, &cycle);

  if(cycle == NULL) {
    fprintf(stdout,"g has no cycle of negative weight.\n");
    
    SCEDA_HashMapIterator paths;
    SCEDA_hashmap_iterator_init(shortest_paths, &paths);
    while(SCEDA_hashmap_iterator_has_next(&paths)) {
      SCEDA_Vertex *u;
      SCEDA_PathInfo *pu = SCEDA_hashmap_iterator_next(&paths, &u);
      if(pu->in_edge != NULL) {
	fprintf(stdout,"distance to %s = %g.\n", SCEDA_vertex_get_data(char *, u), pu->distance);
      } else if(pu->self == origin) {
      } else {
	fprintf(stdout,"%s is not reachable.\n", SCEDA_vertex_get_data(char *, u));
      }
    }
    SCEDA_hashmap_iterator_cleanup(&paths);

  } else {
    fprintf(stdout,"a cycle of negative weight has been detected!\n");
    fprintf(stdout,"%s\n",SCEDA_vertex_get_data(char *, cycle));
  }

  SCEDA_hashmap_delete(shortest_paths);

  SCEDA_graph_delete(g);

  return 0;
}
