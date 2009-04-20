#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/list.h>
#include <SCEDA/graph.h>
#include <SCEDA/graph_dag.h>
#include <SCEDA/graph_antichain.h>
#include <SCEDA/graph_traversal.h>

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

  {
    // iterate over all vertices
    SCEDA_VerticesIterator vert;
    SCEDA_vertices_iterator_init(g, &vert);
    while(SCEDA_vertices_iterator_has_next(&vert)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vert);
      Integer *vlab = SCEDA_vertex_get_data(Integer *, v);
      fprintf(stdout,"Vertex labelled by %d.\n", vlab->value);
      
      {
	fprintf(stdout,"BFS: ");
	// make a bfs from the current vertex
	SCEDA_BFSIterator desc;
	SCEDA_bfs_iterator_init(v, &desc);
	while(SCEDA_bfs_iterator_has_next(&desc)) {
	  SCEDA_Vertex *w = SCEDA_bfs_iterator_next(&desc);
	  Integer *wlab = SCEDA_vertex_get_data(Integer *, w);
	  fprintf(stdout,"%d ", wlab->value);
	}
	SCEDA_bfs_iterator_cleanup(&desc);
	fprintf(stdout,"\n");
      }

      {
	fprintf(stdout,"DFS: ");
	// make a dfs from the current vertex
	SCEDA_DFSIterator desc;
	SCEDA_dfs_iterator_init(v, &desc);
	while(SCEDA_dfs_iterator_has_next(&desc)) {
	  SCEDA_Vertex *w = SCEDA_dfs_iterator_next(&desc);
	  Integer *wlab = SCEDA_vertex_get_data(Integer *, w);
	  fprintf(stdout,"%d ", wlab->value);
	}
	SCEDA_dfs_iterator_cleanup(&desc);
	fprintf(stdout,"\n");
      }
    }
    SCEDA_vertices_iterator_cleanup(&vert);
  }

  
  if(!SCEDA_graph_is_acyclic(g)) {
    fprintf(stderr,"error: the graph is not acyclic\n");
    exit(-1);
  }

  // transitive closure
  {
    SCEDA_Graph *gf = SCEDA_graph_transitive_closure(g);

    fprintf(stdout,"== Transitive closure ==\n");
    // print the graph
    {
      // iterate over all vertices
      SCEDA_VerticesIterator vert;
      SCEDA_vertices_iterator_init(gf, &vert);
      while(SCEDA_vertices_iterator_has_next(&vert)) {
	// next vertex of gf
	SCEDA_Vertex *vf = SCEDA_vertices_iterator_next(&vert);
	// corresponding vertex in g
	SCEDA_Vertex *v = SCEDA_vertex_get_data(SCEDA_Vertex *, vf); 
	// label of the vertex
	Integer *vlab = SCEDA_vertex_get_data(Integer *, v);
	fprintf(stdout,"Vertex labelled by %d.\nSuccessors: ", vlab->value);
	
	// iterate over the successors of the current vertex
	SCEDA_VertexSuccIterator succ;
	SCEDA_vertex_succ_iterator_init(vf, &succ);
	while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
	  SCEDA_Vertex *wf = SCEDA_vertex_succ_iterator_next(&succ);
	  SCEDA_Vertex *w = SCEDA_vertex_get_data(SCEDA_Vertex *, wf); 
	  Integer *wlab = SCEDA_vertex_get_data(Integer *, w);
	  fprintf(stdout,"%d ", wlab->value);
	}
	SCEDA_vertex_succ_iterator_cleanup(&succ);
	fprintf(stdout,"\n");
      }
      SCEDA_vertices_iterator_cleanup(&vert);
    }

    SCEDA_graph_delete(gf);
  }

  // maximum antichain
  {
    SCEDA_List *l = SCEDA_graph_maximum_antichain(g);

    fprintf(stdout,"== Maximum antichain ==\n");
    SCEDA_ListIterator elts;
    SCEDA_list_iterator_init(l, &elts);
    while(SCEDA_list_iterator_has_next(&elts)) {
      SCEDA_Vertex *v = SCEDA_list_iterator_next(&elts);
      Integer *vlab = SCEDA_vertex_get_data(Integer *, v);
      fprintf(stdout,"%d ", vlab->value);
    }
    SCEDA_list_iterator_cleanup(&elts);
    fprintf(stdout,"\n");

    SCEDA_list_delete(l);
  }

  // delete the graph
  SCEDA_graph_delete(g);
  
  return 0;
}
