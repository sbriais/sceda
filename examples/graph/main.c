#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/graph.h>

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

#define ORDER 4

int main(int argc, char *argv[]) {
  // create a graph whose nodes are labelled by Integer and edges are not labelled
  SCEDA_Graph *g = SCEDA_graph_create((SCEDA_delete_fun)delete_Integer, NULL);

  // build the graph
  {
    SCEDA_Vertex *v[1<<ORDER];

    // create vertices
    int i;
    for(i = 0; i < 1<<ORDER; i++) {
      v[i] = SCEDA_graph_add_vertex(g, new_Integer(i));
    }

    // connect them
    // i -> j iff j is a superset of i with exactly one more element
    // (when we interpret the binary representation as a subset of [0;ORDER[)
    for(i = 0; i < 1<<ORDER; i++) {
      int k;
      for(k = 0; k < ORDER; k++) {
	int j = i | (1 << k);
	if(i != j) {
	  SCEDA_graph_add_edge(g, v[i], v[j], NULL);
	}
      }
    }
  }

  // print the graph
  {
    // iterate over all vertices
    SCEDA_VerticesIterator vert;
    SCEDA_vertices_iterator_init(g, &vert);
    while(SCEDA_vertices_iterator_has_next(&vert)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vert);
      Integer *vlab = SCEDA_vertex_get_data(Integer *, v);
      fprintf(stdout,"Vertex labelled by %d.\nSuccessors: ", vlab->value);

      // iterate over the successors of the current vertex
      SCEDA_VertexSuccIterator succ;
      SCEDA_vertex_succ_iterator_init(v, &succ);
      while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
	SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&succ);
	Integer *wlab = SCEDA_vertex_get_data(Integer *, w);
	fprintf(stdout,"%d ", wlab->value);
      }
      SCEDA_vertex_succ_iterator_cleanup(&succ);
      fprintf(stdout,"\n");
    }
    SCEDA_vertices_iterator_cleanup(&vert);
  }

  // delete the graph
  SCEDA_graph_delete(g);

  return 0;
}
