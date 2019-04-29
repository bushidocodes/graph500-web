#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

typedef struct edgenode //structure for edgenode
{
    int src;
    int des;
    int weight;
} edgenode;

edgenode *edgearray[200]; //to store input graph
edgenode *mst[200];       //to store mst

typedef struct heapnode //structure for heapnode
{
    int src;
    int des;
    int weight;
} heapnode;

heapnode *newheapnode(edgenode *);

typedef struct heap //heap structure
{
    int currentsize;                 //current size of heap
    int maxsize;                     // maximum size of heap of min heap
    struct heapnode **edgeheaparray; //ptr to the array of address(ptr to node) of heapnode in the array
} heap;

int vertexCount; //no. of vertices
int edgeCount;
int start;
int parent[200];
int rank[200];

void init(void);
void readfile(void);
void insertadjver(int, int, int);
heap *Buildheap(int);
void swap(heapnode **, heapnode **);
heapnode *extractMin(heap *);
void minHeapify(heap *, int);
void makeset(int);
int findRoot(int);
bool mergeUnionSets(int, int);
void kruskal(void);
edgenode *newedgenode(int, int, int);

void printResults()
{
    printf("\n\nMST IS: \n"); //PRINTING MST
    int weightsum = 0;
    for (int i = 0; i < vertexCount - 1; i++)
    {
        weightsum += mst[i]->weight;
        printf("edge = (%d,%d), edge weight = %d \n", mst[i]->src, mst[i]->des, mst[i]->weight);
    }
    printf("\nAND WEIGHT OF MST IS : %d\n**************************************\n", weightsum);
}

//Kruskal FUNCTION
void kruskal()
{
    printf("Starting Kruskal\n");
    heap *h = Buildheap(edgeCount);
    for (int i = 0; i <= vertexCount; i++) //makeset for all vertices
        makeset(i);
    int mstEdgeIdx = 0;
    int mstLength = vertexCount - 1; // MST always has edgeCount of vertexCount - 1
    while (mstEdgeIdx < mstLength)
    {
        heapnode *minWeightEdge = extractMin(h);
        // Check to see if this edge connects nodes already connected via MST
        bool didMergeUnionSets = mergeUnionSets(minWeightEdge->src, minWeightEdge->des);

        if (didMergeUnionSets)
        {
            mst[mstEdgeIdx] = newedgenode(minWeightEdge->src, minWeightEdge->des, minWeightEdge->weight);
            mstEdgeIdx++;
        }
    }
    printf("Kruskal Complete\n");
}

heap *Buildheap(int maxsize)
{
    int i;
    heap *h = (heap *)malloc(sizeof(heap));
    h->currentsize = 0;
    h->maxsize = maxsize;
    h->edgeheaparray = (heapnode **)malloc(maxsize * sizeof(heapnode *));
    for (i = 0; i < edgeCount; i++)
    {
        h->edgeheaparray[i] = newheapnode(edgearray[i]);
        h->currentsize++;
    }

    for (i = (edgeCount - 1) / 2; i >= 0; i--)
    {
        minHeapify(h, i); //minHeapify at all node having child staring form bottom
    }

    return (h);
}

void minHeapify(heap *h, int now)
{
    int child; //child node position

    while ((now * 2 + 2) <= (h->currentsize)) //while right child index is less or equal to heapsize
    {
        child = now * 2 + 1;
        if (child != h->currentsize && (h->edgeheaparray[child + 1])->weight < (h->edgeheaparray[child])->weight) //gives the minimum child of now
            child++;

        if ((h->edgeheaparray[now])->weight > (h->edgeheaparray[child])->weight) //if heap[now] is greater(i.e unfit) than minimum child
        {
            swap(&h->edgeheaparray[now], &h->edgeheaparray[child]);
        }
        else
        {
            return;
        }
        now = child;
    }
}

void makeset(int u)
{
    parent[u] = u;
    rank[u] = 0;
}

// Traverses to find the root ancestor in Union Set
int findRoot(int u)
{
    if (parent[u] != u)
    {
        u = parent[u];
        parent[u] = findRoot(u);
    }
    return (u);
}

// Returns boolean indicating if union of two disparate sets occured
bool mergeUnionSets(int u, int v)
{
    int rootOfU = findRoot(u);
    int rootOfV = findRoot(v);

    if (rootOfU == rootOfV)
    {
        return false;
    }
    else
    {
        if (rank[rootOfU] > rank[rootOfV])
        {
            parent[rootOfV] = rootOfU;
        }
        else if (rank[rootOfU] < rank[rootOfV])
        {
            parent[rootOfU] = rootOfV;
        }
        else
        {
            parent[rootOfV] = rootOfU;
            rank[rootOfU]++;
        }
        return true;
    }
}

void init()
{
    for (int i = 0; i < vertexCount; i++)
        edgearray[i] = NULL;
    edgeCount = 0;
    vertexCount = 0;
}

void insertadjver(int src, int des, int weight) //src is source vertex and des is destination vertex
{
    edgearray[edgeCount] = newedgenode(src, des, weight); // (i)th edge

    // Update number of vertices and edges. This assumes vertices are identified by numerical indices starting at 0
    int max = src > des ? src : des;
    if (max > (vertexCount - 1))
        vertexCount = (max + 1);

    edgeCount++;
}

edgenode *newedgenode(int src, int des, int weight)
{
    edgenode *p = (edgenode *)malloc(sizeof(edgenode));
    p->src = src;
    p->des = des;
    p->weight = weight;
    return (p);
}

heapnode *newheapnode(edgenode *q)
{
    heapnode *p = (heapnode *)malloc(sizeof(heapnode));
    p->src = q->src;
    p->des = q->des;
    p->weight = q->weight;
    return (p);
}

void swap(heapnode **p, heapnode **q) // swap function for swaping ptr to heapnode
{
    heapnode *temp = *p;
    *p = *q;
    *q = temp;
}

heapnode *extractMin(heap *h)
{
    heapnode *minHeapnode, *lastHeapnode;
    minHeapnode = h->edgeheaparray[0]; //topmost node is the minimum
    lastHeapnode = h->edgeheaparray[h->currentsize - 1];
    h->edgeheaparray[0] = lastHeapnode;
    h->currentsize--;
    minHeapify(h, 0);
    return (minHeapnode);
}
