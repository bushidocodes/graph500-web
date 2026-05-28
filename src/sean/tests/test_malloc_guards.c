/* test_malloc_guards.c
 *
 * Unity regression tests for Issue #11: malloc returns unchecked in the
 * BFS (sean) module.
 *
 * Most critically, insert_edge in src/sean/wasm/graph.c checks for a NULL
 * return from malloc but then falls through and dereferences it anyway:
 *
 *   new_edge = (edge *)malloc(sizeof(edge));
 *   if (new_edge == NULL) { printf("Malloc failed!"); }  // no return
 *   new_edge->destination = destination;                 // NULL deref -> SIGSEGV
 *
 * Malloc injection technique
 * --------------------------
 * test_mock_malloc() is defined BEFORE the `#define malloc test_mock_malloc`
 * directive so its own body still calls the real stdlib malloc.  All malloc()
 * calls inside the subsequently-included graph.c are transparently redirected
 * through the injector, enabling controlled single-call failure without any
 * linker tricks or platform-specific hooks.
 *
 * Test ordering:
 *   test 1 – insert_edge baseline correctness (passes before and after fix)
 *   test 2 – insert_edge with malloc failure: must return cleanly, not crash
 *             (process SIGSEGV before fix; PASS after fix — TDD red→green)
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "unity.h"

/* ---- malloc injection -------------------------------------------------- */

static int g_malloc_fail_countdown = -1; /* -1 = never fail, 0 = fail next */

/* IMPORTANT: defined BEFORE #define malloc test_mock_malloc so that the
 * `malloc(size)` call in this body still refers to the real stdlib malloc. */
static void *test_mock_malloc(size_t size)
{
    if (g_malloc_fail_countdown == 0)
        return NULL;
    if (g_malloc_fail_countdown > 0)
        g_malloc_fail_countdown--;
    return malloc(size); /* real stdlib malloc — #define not yet active */
}

/* Redirect all malloc() calls in included .c files through the injector. */
#define malloc test_mock_malloc
#include "../wasm/graph.c"
#undef malloc /* restore for test helper code below */

/* ---- helpers ----------------------------------------------------------- */

static void free_graph_edges(graph *g)
{
    for (int32_t v = 0; v <= MAXV; v++)
    {
        edge *e = g->edges[v];
        while (e != NULL)
        {
            edge *next = e->next;
            free(e);
            e = next;
        }
    }
}

/* ---- Unity lifecycle --------------------------------------------------- */

void setUp(void)    { g_malloc_fail_countdown = -1; }
void tearDown(void) {}

/* ================================================================
 * Test 1 – insert_edge baseline correctness
 *
 * Confirms the normal (malloc succeeds) path works correctly.
 * Passes before and after the fix.
 * ================================================================ */
void test_insert_edge_succeeds_normally(void)
{
    graph *g = malloc(sizeof(graph)); /* real malloc — #define was undef'd */
    initialize_graph(g, true);

    insert_edge(g, 1, 2, true);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, g->number_edges,
        "insert_edge must increment number_edges on success");
    TEST_ASSERT_NOT_NULL_MESSAGE(g->edges[1],
        "insert_edge must populate edges[source] on success");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, g->edges[1]->destination,
        "inserted edge must record correct destination");

    free_graph_edges(g);
    free(g);
}

/* ================================================================
 * Test 2 – insert_edge must return cleanly on malloc failure (Issue #11)
 *
 * Bug:  after the NULL check there is no `return`, so the code falls
 *       through to `new_edge->destination = destination` — a guaranteed
 *       NULL dereference that terminates the process with SIGSEGV.
 * Fix:  add `return;` immediately after the fprintf(stderr, ...) call.
 *
 * Before fix: process crashes (SIGSEGV) — confirming the bug.
 * After fix:  insert_edge returns early; graph state is unchanged.
 *
 * NOTE: this test is placed last so that test 1 results are reported
 * even when the unfixed code kills the process here.
 * ================================================================ */
void test_insert_edge_malloc_failure_is_safe(void)
{
    graph *g = malloc(sizeof(graph)); /* real malloc, countdown=-1 */
    initialize_graph(g, true);        /* no malloc inside */

    g_malloc_fail_countdown = 0;      /* next malloc() call → NULL */
    insert_edge(g, 1, 2, true);       /* internally calls test_mock_malloc → NULL */

    /* If we reach these assertions the fix is in place. */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, g->number_edges,
        "Issue #11: insert_edge must not modify graph when malloc fails");
    TEST_ASSERT_NULL_MESSAGE(g->edges[1],
        "Issue #11: insert_edge must not dereference the NULL new_edge pointer");

    free(g); /* no edges allocated — simple free is sufficient */
}

/* ================================================================ */
int main(void)
{
    setbuf(stdout, NULL); /* unbuffer stdout so test-1 results survive a crash in test-2 */
    UNITY_BEGIN();
    RUN_TEST(test_insert_edge_succeeds_normally);
    RUN_TEST(test_insert_edge_malloc_failure_is_safe); /* last: crashes before fix */
    return UNITY_END();
}
