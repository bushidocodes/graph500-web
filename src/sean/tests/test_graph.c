/* test_graph.c
 *
 * Unity regression tests for src/sean/wasm/graph.c.
 *
 * Validates bugs reported in:
 *   Issue #8 – insert_edge leaves new_edge->next uninitialized for the first
 *               edge inserted at each vertex.
 *   Issue #7 – build_csr has three bugs:
 *     (7a) inner loop `currentEdge->next != NULL` skips the last edge of
 *          every adjacency list
 *     (7b) outer loop `current_vertex < g->number_vertices` skips the
 *          highest-numbered vertex
 *     (7c) inner loop dereferences NULL when a vertex has no edges
 *   Issue #34 – insert_edge writes edges[source]/degree[source] out of bounds
 *               when a vertex index falls outside the valid [1, MAXV] range.
 *
 * Test ordering is deliberate: tests 1-4 fail *cleanly* with the buggy code
 * (no crash), test 5 crashes the process with the buggy code (demonstrating
 * the NULL-deref of bug 7c).  After both fixes all five tests pass.
 */

#include "unity.h"
#include "../wasm/graph.c"

/* ---- helpers ---- */

static edge *make_edge(int32_t dest, edge *next_edge)
{
    edge *e = malloc(sizeof(edge));
    e->destination = dest;
    e->next = next_edge;
    return e;
}

static void free_graph(graph *g)
{
    if (g->is_CSR) {
        free(g->IA);
        free(g->JA);
    }
    free(g);
}

void setUp(void)  {}
void tearDown(void) {}

/* ================================================================
 * Issue #8 – insert_edge uninitialized next pointer
 *
 * Detection: poison one sizeof(edge) block with 0xFF before the call so the
 * allocator hands the same block to insert_edge's malloc.  With the bug
 * new_edge->next keeps the 0xFF garbage value; with the fix it is set to NULL.
 * ================================================================ */

void test_insert_edge_first_edge_next_is_null(void)
{
    /* Poison the heap so the next sizeof(edge) malloc likely returns non-zero bytes */
    void *poison = malloc(sizeof(edge));
    memset(poison, 0xFF, sizeof(edge));
    free(poison);

    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    insert_edge(g, 1, 2, true); /* directed: first (and only) edge for vertex 1 */

    TEST_ASSERT_NOT_NULL_MESSAGE(g->edges[1],
        "vertex 1 must have an edge after insert");
    TEST_ASSERT_NULL_MESSAGE(g->edges[1]->next,
        "Issue #8: first edge next must be NULL "
        "(bug: only set when list non-empty, else left uninitialized)");

    free_graph(g);
}

void test_insert_edge_chain_is_null_terminated(void)
{
    /* Poison before the first insert so the first edge node has 0xFF bytes */
    void *poison = malloc(sizeof(edge));
    memset(poison, 0xFF, sizeof(edge));
    free(poison);

    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    insert_edge(g, 1, 2, true); /* first edge for vertex 1 (list: 2 -> ?) */
    insert_edge(g, 1, 3, true); /* prepended     (list: 3 -> 2 -> ?) */

    TEST_ASSERT_NOT_NULL(g->edges[1]);
    TEST_ASSERT_EQUAL_INT(3, g->edges[1]->destination);
    TEST_ASSERT_NOT_NULL(g->edges[1]->next);
    TEST_ASSERT_EQUAL_INT(2, g->edges[1]->next->destination);
    TEST_ASSERT_NULL_MESSAGE(g->edges[1]->next->next,
        "Issue #8: tail of chained list must be NULL "
        "(bug: first insert leaves next uninitialized so tail is garbage)");

    free_graph(g);
}

/* ================================================================
 * Issue #7 – build_csr correctness
 *
 * These tests use manually-constructed, properly NULL-terminated adjacency
 * lists to isolate build_csr bugs from any insert_edge behaviour.
 * ================================================================ */

/*
 * 7a – inner loop off-by-one
 *
 * Graph: vertex 1 has exactly one edge (to vertex 2), NULL-terminated.
 * Bug:   inner guard `currentEdge->next != NULL` is false on entry; loop
 *        body never runs; getDegree(g,1) returns 0 instead of 1.
 * Fix:   guard `currentEdge != NULL` enters the body for every non-NULL node.
 *
 * No isolated vertices are in the outer-loop range (number_vertices == 2,
 * loop < 2 covers only vertex 1), so this test fails *cleanly* with the bug.
 */
void test_build_csr_includes_last_edge(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    g->edges[1]        = make_edge(2, NULL);
    g->degree[1]       = 1;
    g->number_vertices = 2;
    g->number_edges    = 1;

    build_csr(g);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, getDegree(g, 1),
        "Issue #7a: vertex 1 must have degree 1 after CSR build "
        "(bug: last edge of every adjacency list is skipped)");

    free(g->edges[1]);
    free_graph(g);
}

/*
 * 7b – outer loop off-by-one
 *
 * Graph: vertices 1 and 2 each have one edge (so the buggy inner loop does
 * not crash); vertex 3 (== number_vertices) has one edge to vertex 1.
 * Bug:   outer loop `< number_vertices` skips vertex 3; IA[4] is never
 *        written so getDegree(g,3) returns indeterminate garbage.
 * Fix:   outer loop `<= number_vertices` processes all vertices.
 *
 * Vertices 1 and 2 each have exactly one edge (next==NULL) so the buggy
 * inner loop halts without dereferencing NULL, keeping this test clean.
 */
void test_build_csr_includes_highest_vertex(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    g->edges[1]        = make_edge(3, NULL);
    g->degree[1]       = 1;
    g->edges[2]        = make_edge(3, NULL);
    g->degree[2]       = 1;
    g->edges[3]        = make_edge(1, NULL);
    g->degree[3]       = 1;
    g->number_vertices = 3;
    g->number_edges    = 3;

    build_csr(g);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, getDegree(g, 3),
        "Issue #7b: highest-indexed vertex must appear in CSR "
        "(bug: outer loop `< number_vertices` skips it; IA[n+1] stays uninitialized)");

    free(g->edges[1]);
    free(g->edges[2]);
    free(g->edges[3]);
    free_graph(g);
}

/*
 * 7c – NULL dereference on isolated vertex
 *
 * Graph: vertex 2 has no edges; vertices 1 and 3 each have one edge.
 * Bug:   when the outer loop reaches vertex 2, `currentEdge` is NULL;
 *        evaluating `currentEdge->next` crashes the process.
 * Fix:   guard `currentEdge != NULL` short-circuits before the dereference.
 *
 * NOTE: this test is placed last because the bug causes a hard crash that
 * terminates the process, preventing subsequent tests from running.
 */
void test_build_csr_isolated_vertex_no_crash(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    g->edges[1]        = make_edge(3, NULL);
    g->degree[1]       = 1;
    /* vertex 2 intentionally left with no edges (g->edges[2] == NULL) */
    g->edges[3]        = make_edge(1, NULL);
    g->degree[3]       = 1;
    g->number_vertices = 3;
    g->number_edges    = 2;

    build_csr(g); /* must not crash */

    TEST_ASSERT_TRUE_MESSAGE(g->is_CSR,
        "Issue #7c: build_csr must complete without crashing on isolated vertex");
    TEST_ASSERT_EQUAL_INT(1, getDegree(g, 1));
    TEST_ASSERT_EQUAL_INT(0, getDegree(g, 2));
    TEST_ASSERT_EQUAL_INT(1, getDegree(g, 3));

    free(g->edges[1]);
    free(g->edges[3]);
    free_graph(g);
}

/* ================================================================
 * Issue #34 – insert_edge out-of-bounds write
 *
 * edges[]/degree[] are fixed-size (MAXV + 1 elements, valid 1-indexed range
 * [1, MAXV]). insert_edge accessed g->edges[source]/g->degree[source] without
 * any bounds check, so a vertex index outside that range writes past the end
 * of the arrays (corrupting adjacent heap/struct state in WASM linear memory).
 * It also set number_vertices = max unconditionally, which would later drive
 * build_csr to allocate/iterate out of bounds.
 *
 * Detection: a rejected insert must leave the graph untouched — number_vertices
 * and number_edges stay 0 and edges[]/degree[] are never written. With the bug
 * number_vertices becomes `max`, failing these assertions (red); the guard
 * makes them pass (green).
 * ================================================================ */

void test_insert_edge_rejects_oversized_vertex(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    /* source beyond the array — would write edges[MAXV + 1] out of bounds */
    insert_edge(g, MAXV + 1, 1, true);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, g->number_vertices,
        "Issue #34: insert with source > MAXV must be rejected "
        "(bug: number_vertices = max and edges[source] written out of bounds)");

    /* destination beyond the array — undirected recursion would write edges[dest] OOB */
    insert_edge(g, 1, MAXV + 1, false);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, g->number_vertices,
        "Issue #34: insert with destination > MAXV must be rejected");

    TEST_ASSERT_EQUAL_INT(0, g->number_edges);

    free_graph(g);
}

void test_insert_edge_rejects_nonpositive_vertex(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true);

    /* vertex 0 is the reserved/poison slot; the graph is 1-indexed */
    insert_edge(g, 0, 1, true);
    insert_edge(g, 1, -5, true);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, g->number_vertices,
        "Issue #34: insert with a non-positive vertex must be rejected "
        "(bug: 1-indexed graph treats 0/negative as valid indices)");
    TEST_ASSERT_EQUAL_INT(0, g->number_edges);

    free_graph(g);
}

/* ================================================================ */
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_insert_edge_first_edge_next_is_null);
    RUN_TEST(test_insert_edge_chain_is_null_terminated);
    RUN_TEST(test_build_csr_includes_last_edge);
    RUN_TEST(test_build_csr_includes_highest_vertex);
    RUN_TEST(test_build_csr_isolated_vertex_no_crash);
    RUN_TEST(test_insert_edge_rejects_oversized_vertex);
    RUN_TEST(test_insert_edge_rejects_nonpositive_vertex);
    return UNITY_END();
}
