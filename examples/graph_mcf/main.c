#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_flow.h>

#include <assert.h>

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

int get_edge_value(SCEDA_Edge *e, SCEDA_HashMap *map) {
  Integer *x = SCEDA_hashmap_get(map, e);
  return x->value;
}

int get_vertex_value(SCEDA_Vertex *v, SCEDA_HashMap *map) {
  Integer *x = SCEDA_hashmap_get(map, v);
  return x->value;
}

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by strings and edges are unlabelled
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, NULL);

  SCEDA_Vertex *vA = SCEDA_graph_add_vertex(g, strdup("A"));
  SCEDA_Vertex *vB = SCEDA_graph_add_vertex(g, strdup("B"));
  SCEDA_Vertex *vC = SCEDA_graph_add_vertex(g, strdup("C"));
  SCEDA_Vertex *vD = SCEDA_graph_add_vertex(g, strdup("D"));

  SCEDA_Edge *e0 = SCEDA_graph_add_edge(g, vA, vB, NULL);
  SCEDA_Edge *e1 = SCEDA_graph_add_edge(g, vB, vC, NULL);
  SCEDA_Edge *e2 = SCEDA_graph_add_edge(g, vC, vA, NULL);
  SCEDA_Edge *e3 = SCEDA_graph_add_edge(g, vC, vD, NULL);
  SCEDA_Edge *e4 = SCEDA_graph_add_edge(g, vA, vD, NULL);

  SCEDA_HashMap *supply = SCEDA_vertex_map_create((SCEDA_delete_fun)delete_Integer);
  SCEDA_hashmap_put(supply, vA, new_Integer(4), NULL);
  SCEDA_hashmap_put(supply, vB, new_Integer(3), NULL);
  SCEDA_hashmap_put(supply, vC, new_Integer(-2), NULL);
  SCEDA_hashmap_put(supply, vD, new_Integer(-5), NULL);
  
  SCEDA_HashMap *lcap = SCEDA_edge_map_create((SCEDA_delete_fun)delete_Integer);
  SCEDA_hashmap_put(lcap, e0, new_Integer(0), NULL);
  SCEDA_hashmap_put(lcap, e1, new_Integer(2), NULL);
  SCEDA_hashmap_put(lcap, e2, new_Integer(0), NULL);
  SCEDA_hashmap_put(lcap, e3, new_Integer(1), NULL);
  SCEDA_hashmap_put(lcap, e4, new_Integer(4), NULL);

  SCEDA_HashMap *ucap = SCEDA_edge_map_create((SCEDA_delete_fun)delete_Integer);
  SCEDA_hashmap_put(ucap, e0, new_Integer(1), NULL);
  SCEDA_hashmap_put(ucap, e1, new_Integer(4), NULL);
  SCEDA_hashmap_put(ucap, e2, new_Integer(5), NULL);
  SCEDA_hashmap_put(ucap, e3, new_Integer(2), NULL);
  SCEDA_hashmap_put(ucap, e4, new_Integer(9), NULL);

  SCEDA_HashMap *cost = SCEDA_edge_map_create((SCEDA_delete_fun)delete_Integer);
  SCEDA_hashmap_put(cost, e0, new_Integer(1), NULL);
  SCEDA_hashmap_put(cost, e1, new_Integer(2), NULL);
  SCEDA_hashmap_put(cost, e2, new_Integer(1), NULL);
  SCEDA_hashmap_put(cost, e3, new_Integer(3), NULL);
  SCEDA_hashmap_put(cost, e4, new_Integer(2), NULL);

  SCEDA_HashMap *flow = SCEDA_graph_min_cost_flow(g, get_edge_value, lcap, get_edge_value, ucap, NULL, supply, NULL, cost);

  if(flow != NULL) {
    int flowcost = 0;

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      int *f_e = SCEDA_hashmap_get(flow, e);
      fprintf(stdout,"%s -> %s : %d\n", SCEDA_vertex_get_data(char *, SCEDA_edge_source(e)), SCEDA_vertex_get_data(char *, SCEDA_edge_target(e)), *f_e);
      flowcost += *f_e * get_edge_value(e, cost);
    }      
    SCEDA_edges_iterator_cleanup(&edges);

    fprintf(stdout,"total cost = %d\n",flowcost);

    SCEDA_hashmap_delete(flow);
  } else {
    fprintf(stdout,"no solution was found\n");
  }

  SCEDA_hashmap_delete(cost);
  SCEDA_hashmap_delete(ucap);
  SCEDA_hashmap_delete(lcap);
  SCEDA_hashmap_delete(supply);

  SCEDA_graph_delete(g);

  return 0;
}
