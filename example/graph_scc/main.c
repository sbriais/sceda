#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_scc.h>

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

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by Integer and edges are not labelled
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_Integer, NULL);
 
  SCEDA_Vertex *v0 = SCEDA_graph_add_vertex(g, new_Integer(0));
  SCEDA_Vertex *v1 = SCEDA_graph_add_vertex(g, new_Integer(1));
  SCEDA_Vertex *v2 = SCEDA_graph_add_vertex(g, new_Integer(2));
  SCEDA_Vertex *v3 = SCEDA_graph_add_vertex(g, new_Integer(3));
  SCEDA_Vertex *v4 = SCEDA_graph_add_vertex(g, new_Integer(4));
  SCEDA_Vertex *v5 = SCEDA_graph_add_vertex(g, new_Integer(5));

  SCEDA_graph_add_edge(g, v0, v1, NULL);
  SCEDA_graph_add_edge(g, v1, v2, NULL);
  SCEDA_graph_add_edge(g, v2, v0, NULL);
  SCEDA_graph_add_edge(g, v4, v5, NULL);
  SCEDA_graph_add_edge(g, v5, v4, NULL);

  SCEDA_List *scc = SCEDA_graph_strongly_connected_components(g);

  fprintf(stdout,"g has %d strongly connected components:\n", SCEDA_list_size(scc));

  SCEDA_ListIterator comp;
  SCEDA_list_iterator_init(scc, &comp);
  while(SCEDA_list_iterator_has_next(&comp)) {
    SCEDA_HashSet *cc = SCEDA_list_iterator_next(&comp);
    SCEDA_HashSetIterator ccvert;
    SCEDA_hashset_iterator_init(cc, &ccvert);
    while(SCEDA_hashset_iterator_has_next(&ccvert)) {
      SCEDA_Vertex *u = SCEDA_hashset_iterator_next(&ccvert);
      fprintf(stdout,"%d ",SCEDA_vertex_get_data(Integer *, u)->value);
    }
    SCEDA_hashset_iterator_cleanup(&ccvert);
    fprintf(stdout,"\n");
  }
  SCEDA_list_iterator_cleanup(&comp);

  SCEDA_list_delete(scc);

  SCEDA_graph_delete(g);

  return 0;
}
