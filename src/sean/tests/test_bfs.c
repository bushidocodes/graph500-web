/* test_bfs.c
 *
 * Unity regression tests for the CSR BFS in src/sean/wasm/bfs.c.
 *
 * Validates bugs reported in Issue #10:
 *   The `+200` malloc padding was a workaround for the now-fixed build_csr
 *   size mismatch (issues #7/#8).  Now that getDegree and getNeighbors are
 *   consistent, the padding is dead code and should be removed.
 *
 *   While auditing bfs.c, a second bug was found: the inner scan loop uses
 *   `current_vertex < g->number_vertices`, which is the same off-by-one as
 *   the build_csr outer loop (also fixed in that PR).  The highest-numbered
 *   vertex is never processed as a BFS *source*, so any vertex reachable only
 *   through it is silently dropped from the BFS tree.
 *
 * Test ordering:
 *   test 1 – basic correctness baseline (passes before and after fix)
 *   test 2 – fails before fix, passes after (TDD red/green for the bug)
 */

#include "unity.h"
#include "../wasm/bfs.c"

/* ---- helpers ---- */

/* Allocate and zero-initialise the caller-supplied BFS arrays. */
static bool     *alloc_discovered(void) { return calloc(MAXV + 1, sizeof(bool));    }
static int32_t  *alloc_parents(void)
{
    int32_t *p = malloc((MAXV + 1) * sizeof(int32_t));
    memset(p, 0xFF, (MAXV + 1) * sizeof(int32_t)); /* sentinel: -1 means "not found" */
    return p;
}

static void free_graph(graph *g)
{
    if (g->is_CSR) { free(g->IA); free(g->JA); }
    free(g);
}

void setUp(void)  {}
void tearDown(void) {}

/* ================================================================
 * Test 1 – CSR BFS basic correctness (baseline)
 *
 * Graph: undirected path  1 – 2 – 3
 * BFS from vertex 1 must assign correct parents.
 *
 * This test passes even with the buggy < guard because the highest vertex (3)
 * is only a *destination*, not a required source; it serves as a baseline to
 * confirm the CSR BFS produces correct results at all.
 * ================================================================ */
void test_bfs_csr_finds_all_reachable_vertices(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, false); /* undirected */
    insert_edge(g, 1, 2, false);
    insert_edge(g, 2, 3, false);
    build_csr(g);

    bool    *is_discovered = alloc_discovered();
    int32_t *has_parent    = alloc_parents();

    bfs(g, 1, is_discovered, has_parent);

    TEST_ASSERT_EQUAL_INT(1, has_parent[1]);  /* start node is its own parent */
    TEST_ASSERT_EQUAL_INT(1, has_parent[2]);
    TEST_ASSERT_EQUAL_INT(2, has_parent[3]);

    free(is_discovered);
    free(has_parent);
    free_graph(g);
}

/* ================================================================
 * Test 2 – CSR BFS outer loop off-by-one (TDD red→green test)
 *
 * Graph: directed  1 → 4 → 2   (vertex 4 == number_vertices)
 * BFS from vertex 1 must traverse through vertex 4 to discover vertex 2.
 *
 * Bug: the scan loop `for (current_vertex = 1; current_vertex < number_vertices; ...)`
 * never processes vertex 4 as a BFS source; vertex 2 is therefore never
 * discovered, and has_parent[2] stays at the sentinel value -1.
 *
 * Fix: change `<` to `<=`.
 * ================================================================ */
void test_bfs_csr_highest_vertex_explored_as_source(void)
{
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, true); /* directed */
    insert_edge(g, 1, 4, true); /* 1 → 4 */
    insert_edge(g, 4, 2, true); /* 4 → 2  (vertex 4 = number_vertices) */
    build_csr(g);

    bool    *is_discovered = alloc_discovered();
    int32_t *has_parent    = alloc_parents();

    bfs(g, 1, is_discovered, has_parent);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, has_parent[4],
        "vertex 4 must be reachable from vertex 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, has_parent[2],
        "Issue #10 (BFS outer loop): vertex 2 must be discovered through "
        "vertex 4; bug: `current_vertex < number_vertices` skips vertex 4 "
        "as a BFS source so vertex 2 is never found");

    free(is_discovered);
    free(has_parent);
    free_graph(g);
}

/* ================================================================ */
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_bfs_csr_finds_all_reachable_vertices);
    RUN_TEST(test_bfs_csr_highest_vertex_explored_as_source);
    return UNITY_END();
}
