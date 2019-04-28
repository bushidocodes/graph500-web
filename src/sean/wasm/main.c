#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "bfs.c"

/* Globals and Public API to enable WASM Bridge */

graph *myGraph = NULL;
bool is_discovered[MAXV + 1];
int32_t has_parent[MAXV + 1];

void createGraph()
{
    printf("Start Create Graph\n");
    myGraph = malloc(sizeof(graph));
    initialize_graph(myGraph, false);
    printf("Graph Created!\n");
}

void insertEdge(int32_t source, int32_t destination)
{
    insert_edge(myGraph, source, destination, false);
}

void runBFS(int32_t root)
{
    for (int i = 0; i < MAXV + 1; i++)
    {
        is_discovered[i] = 0;
        has_parent[i] = -1;
    }
    bfs(myGraph, root, is_discovered, has_parent);
}

int32_t getParent(int32_t vertex)
{
    return has_parent[vertex];
}
