#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_meancycle.h>

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

int get_distance(SCEDA_Edge *e, void *ctxt) {
  return SCEDA_edge_get_data(Integer *,e)->value;
}

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by strings and edges are labelled by Integers
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_Integer);
  
  SCEDA_Vertex *vA = SCEDA_graph_add_vertex(g, strdup("A"));
  SCEDA_Vertex *vB = SCEDA_graph_add_vertex(g, strdup("B"));
  SCEDA_Vertex *vC = SCEDA_graph_add_vertex(g, strdup("C"));
  SCEDA_Vertex *vD = SCEDA_graph_add_vertex(g, strdup("D"));
  SCEDA_Vertex *vE = SCEDA_graph_add_vertex(g, strdup("E"));

  SCEDA_graph_add_edge(g, vA, vA, new_Integer(3));

  //  SCEDA_graph_add_edge(g, vA, vB, new_Integer(1));
  SCEDA_graph_add_edge(g, vB, vC, new_Integer(2));
  //  SCEDA_graph_add_edge(g, vB, vC, new_Integer(-4));
  SCEDA_graph_add_edge(g, vC, vD, new_Integer(3));
  SCEDA_graph_add_edge(g, vD, vE, new_Integer(4));
  SCEDA_graph_add_edge(g, vE, vB, new_Integer(5));
  
  SCEDA_List *mean_cycle;
  
  if(SCEDA_graph_minimum_mean_cycle(g, get_distance, NULL, &mean_cycle) == 0) {
    fprintf(stdout,"Minimum Mean Cycle found.\n");

    int cost = 0;

    SCEDA_ListIterator edges;
    SCEDA_list_iterator_init(mean_cycle, &edges);
    while(SCEDA_list_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_list_iterator_next(&edges);
      cost += get_distance(e, NULL);
      fprintf(stdout,"%s -> %s; ",SCEDA_vertex_get_data(char *,SCEDA_edge_source(e)),SCEDA_vertex_get_data(char *,SCEDA_edge_target(e)));
    }
    SCEDA_list_iterator_cleanup(&edges);

    fprintf(stdout," of mean cost = %d/%d\n", cost, SCEDA_list_size(mean_cycle));

    SCEDA_list_delete(mean_cycle);
  }

  SCEDA_graph_delete(g);

  return 0;
}
