#include <stdio.h>
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

int V; //no. of vertices
int E; //no. of edges
int start;
int parent[200];
int rank[200];

//FUNTION DECLARATION
void init(void);
void readfile(void);
void insertadjver(int, int, int);
heap *Buildheap(int);
void swap(heapnode **, heapnode **);
heapnode *extractMin(heap *);
void minHeapify(heap *, int);
void makeset(int);
int find(int);
int Union(int, int);
void kruskal(void);
edgenode *newedgenode(int, int, int);

//MAIN FUNCTION
// int main()
// {
//     int i;
//     init();     //INITIALISATION
//     readfile(); //READ FILE FROM TERMINAL
//     kruskal();  //Kruskal FUNCTION CALLED

//     printf("\n\nMST IS: \n"); //PRINTING MST
//     int weightsum = 0;
//     for (i = 0; i < V - 1; i++)
//     {
//         weightsum = weightsum + mst[i]->weight;
//         printf("edge = (%d,%d), edge weight = %d \n", mst[i]->src, mst[i]->des, mst[i]->weight);
//     }
//     printf("\nAND WEIGHT OF MST IS : %d\n**************************************\n", weightsum);
//     return 0;
// }

void printResults()
{
    int i;
    // init();     //INITIALISATION
    // readfile(); //READ FILE FROM TERMINAL
    // kruskal();  //Kruskal FUNCTION CALLED

    printf("\n\nMST IS: \n"); //PRINTING MST
    printf("V is %d\n", V);
    int weightsum = 0;
    for (i = 0; i < V - 1; i++)
    {
        printf("i is %d\n", V);
        weightsum = weightsum + mst[i]->weight;
        printf("edge = (%d,%d), edge weight = %d \n", mst[i]->src, mst[i]->des, mst[i]->weight);
    }
    printf("\nAND WEIGHT OF MST IS : %d\n**************************************\n", weightsum);
    // return 0;
}

//Kruskal FUNCTION
void kruskal()
{
    printf("Starting Kruskal\n");
    int i;
    heap *h = Buildheap(E);
    for (i = 0; i <= V; i++) //makeset for all vertices
        makeset(i);
    int e = 1;
    while (e <= V)
    {
        heapnode *p = extractMin(h);

        if (Union(p->src, p->des) == 1)
        {
            mst[e - 1] = newedgenode(p->src, p->des, p->weight); //e-1, since e starting from 1
            e++;
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
    for (i = 0; i < E; i++)
    {
        h->edgeheaparray[i] = newheapnode(edgearray[i]);
        h->currentsize++;
    }

    for (i = (E - 1) / 2; i >= 0; i--)
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
        {
            child++;
        }

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

int find(int u)
{
    if (parent[u] != u)
    {
        u = parent[u];
        parent[u] = find(u);
    }
    return (u);
}
int Union(int u, int v)
{
    int x, y;
    x = find(u);
    y = find(v);

    if (x == y)
    {
        return (0);
    }
    if (rank[x] > rank[y])
    {
        parent[y] = x;
    }
    else if (rank[x] < rank[y])
    {
        parent[x] = y;
    }
    else
    {
        parent[y] = x;
        rank[x]++;
    }
    return (1);
}

int edgearrayLength;

//FUNTION FOR INITIALISATION
void init()
{
    int i;
    for (i = 0; i < V; i++)
    {
        edgearray[i] = NULL;
    }
    edgearrayLength = 0;
    V = 0;
}

// void readfile()
// {
//     int u, v, weight;
//     int i = 0;
//     printf("\n**************************************\nGRAPH IS :\n");
//     printf("\nenter no. of vertex");
//     scanf("%d", &V); //reading no. vertex
//     printf("\nvetices %d\n", V);
//     printf("\nenter edges\n");
//     scanf("%d", &E); //reading no. edge
//     printf("edges are\n");
//     printf("%d\n", E);

//     for (i = 0; i < E; i++)
//     {
//         printf("Source\n");
//         scanf("%d", &u);

//         printf("Destination\n");
//         scanf("%d", &v);
//         printf("Enter Weight\n");
//         scanf("%d", &weight);
//         printf("Edge and weight to be Inserted: %d %d %d\n", u, v, weight);
//         insertadjver(i, u, v, weight);
//     }
// }
// void insertadjver(int i, int src, int des, int weight)

void insertadjver(int src, int des, int weight) //src is source vertex and des is destination vertex
{

    int max = src > des ? src : des;
    if (max > V)
        V = max;

    printf("Inserting %d -> %d of weight %d at index %d\n", src, des, weight, edgearrayLength);
    edgearray[edgearrayLength] = newedgenode(src, des, weight); // (i)th edge
    edgearrayLength++;
}

edgenode *newedgenode(int src, int des, int weight)
{
    edgenode *p;
    p = (edgenode *)malloc(sizeof(edgenode));
    p->src = src;
    p->des = des;
    p->weight = weight;
    // p->next = NULL;
    return (p);
}

heapnode *newheapnode(edgenode *q)
{
    heapnode *p;
    p = (heapnode *)malloc(sizeof(heapnode));
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
    h->currentsize--; //reducing the currentsize

    minHeapify(h, 0);
    return (minHeapnode);
}
