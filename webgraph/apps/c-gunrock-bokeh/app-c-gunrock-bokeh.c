#include "thunderdome/td.h"
#include "etl/c_import.h"
#include "cluster/gunrock/gunrock_clusters.h"


void _print_usage()
{
  char* usage = "app <num_top_nodes>  <graph_type> <graph_name>\n\
    \n\
    Use c etl, clusters with gunrock top-k, and produces an \n\
    bokeh visualization. \n\
\n\
Description\n\
\n\
    num_top_nodes: Maximum number of top nodes to produce\n\
    graph_format: one of WDC or SNAP\n\
    graph_names: filename to graph (or index_file arc_file for WDC graphs)\n\
\n\
Examples:\n\
    test_c_import 3 SNAP ../data/web-NotreDame.txt\n\
\n\
    test_c_import 10  WDC ../data/pld-arc-sample ../data/pld-index-sample\n\
\n\
Author(s):\n\
    Andy R. Terrel <andy.terrel@gmail.com>\n";
  printf("%s\n", usage);
}


int main(int argc, char** argv)
{
  if (argc > 5 || argc < 4) {
    _print_usage();
    return 1;
  }

  graph_format_t format = graph_format_from_str(argv[2]);
  char* arc_file = argv[3];
  char* index_file = (argc == 5) ? argv[4] : NULL;
  int top_nodes = atoi(argv[1]);
  int *node_ids          = (int*)malloc(sizeof(int) * top_nodes);
  int *centrality_values = (int*)malloc(sizeof(int) * top_nodes);

  graph_t td_graph;

  load_graph(format, arc_file, index_file, &td_graph);
  gunrock_topk(&td_graph, top_nodes, node_ids, centrality_values);

  // print results for check correctness
  /* int i; */
  /* for (i = 0; i < top_nodes; ++i) */
  /* { */
  /*   printf("Node ID [%d] : CV [%d] \n", node_ids[i], centrality_values[i]); */
  /* } */
  /* printf("\n"); */



  td_val_t out_py;
  td_env_t *py = td_env_python(TD_DIR, TD_PYTHON_EXE);

  td_array_t td_nodes = { .data=node_ids, .length=top_nodes, .eltype=TD_INT32, .ndims=1 };
  td_val_t arg_py = { .tag = TD_ARRAY, .object = &td_nodes };
  td_val_t arg_py_1 = { .tag = TD_INT32, .int32_val = 2 };

  py->invoke1(&out_py, "bokeh_wrap.visualize", &arg_py_1);
  printf("bokeh_wrap.visualize(2) = %d\n", td_int32(&out_py));
    
  py->invoke1(&out_py, "bokeh_wrap.visualize", &arg_py);
  printf("bokeh_wrap(nodes_ids) = %d\n", td_int32(&out_py));
  
  if (centrality_values) free(centrality_values);
  if (node_ids)          free(node_ids);
  

  return 0;

}
