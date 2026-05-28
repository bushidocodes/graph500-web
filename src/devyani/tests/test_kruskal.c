/* test_kruskal.c
 *
 * Unity regression tests for src/devyani/wasm/main.c.
 *
 * Validates Issue #9: insertadjver writes to fixed-size 200-element global
 * arrays (edgearray, mst, parent, rank) without bounds checking.  Any caller
 * that supplies > MAX_EDGES edges, or any vertex id >= MAX_VERTICES, produces
 * an out-of-bounds write.
 *
 * The expected limits are declared here so the tests compile and go *red*
 * against the unfixed code (which performs no bounds checks).  The fix adds
 * the same constants to main.c (via #ifndef guards) and enforces them in
 * insertadjver.
 *
 * Test ordering:
 *   test 1 – algorithm correctness baseline (passes before and after fix)
 *   test 2 – TDD red→green: insertadjver must reject the (MAX_EDGES+1)th edge
 *   test 3 – TDD red→green: insertadjver must reject vertex id >= MAX_VERTICES
 */

/* Declare the expected capacity limits BEFORE including main.c so the tests
 * compile even when main.c does not yet define them. */
#define MAX_EDGES    200
#define MAX_VERTICES 200

#include "unity.h"
#include "../wasm/main.c"

void setUp(void)    { init(); }
void tearDown(void) {}

/* ================================================================
 * Test 1 – Kruskal algorithm correctness (baseline)
 *
 * Triangle graph: edges (0,1,1), (0,2,5), (1,2,2).
 * MST must include the two cheapest edges: weights 1 and 2, total = 3.
 * This test passes before and after the fix (the algorithm itself is correct).
 * ================================================================ */
void test_kruskal_produces_correct_mst(void)
{
    insertadjver(0, 1, 1);
    insertadjver(0, 2, 5);
    insertadjver(1, 2, 2);
    TEST_ASSERT_EQUAL_INT(3, edgeCount);
    TEST_ASSERT_EQUAL_INT(3, vertexCount); /* vertices 0, 1, 2 */

    kruskal();

    /* MST of this triangle has exactly 2 edges with combined weight 3 */
    int mst_weight = mst[0]->weight + mst[1]->weight;
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, mst_weight,
        "MST of triangle(1,2,5) must have total weight 3");
}

/* ================================================================
 * Test 2 – insertadjver must stop accepting edges once full (Issue #9)
 *
 * Bug:  `edgearray[edgeCount]` is written without checking edgeCount < 200;
 *       on the 201st insert `edgearray[200]` is an out-of-bounds write
 *       (overlapping the `mst` global) and edgeCount becomes 201.
 * Fix:  guard `if (edgeCount >= MAX_EDGES) return;` in insertadjver.
 * ================================================================ */
void test_insertadjver_rejects_edge_at_capacity(void)
{
    /* fill to exactly the limit */
    for (int i = 0; i < MAX_EDGES; i++)
        insertadjver(0, 1, i);
    TEST_ASSERT_EQUAL_INT(MAX_EDGES, edgeCount);

    /* one over — must be silently rejected */
    insertadjver(0, 1, 9999);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MAX_EDGES, edgeCount,
        "Issue #9: edgeCount must not exceed MAX_EDGES "
        "(bug: insertadjver writes edgearray[200] out of bounds)");
}

/* ================================================================
 * Test 3 – insertadjver must reject vertex ids >= MAX_VERTICES (Issue #9)
 *
 * Bug:  insertadjver updates vertexCount = max(src,des)+1 without a cap;
 *       kruskal then calls makeset(i) for i up to vertexCount, writing
 *       parent[vertexCount] and rank[vertexCount] out of bounds when
 *       vertexCount >= 200.
 * Fix:  guard `if (max >= MAX_VERTICES) return;` in insertadjver.
 * ================================================================ */
void test_insertadjver_rejects_oversized_vertex_id(void)
{
    /* insert one valid edge so we can detect a silent reject */
    insertadjver(0, 1, 1);
    TEST_ASSERT_EQUAL_INT(1, edgeCount);

    /* vertex id MAX_VERTICES (== 200) would push vertexCount to 201,
     * causing makeset / kruskal to write parent[201] out of bounds */
    insertadjver(0, MAX_VERTICES, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, edgeCount,
        "Issue #9: insert with vertex id >= MAX_VERTICES must be rejected "
        "(bug: vertexCount = max+1 = 201 causes parent/rank OOB in kruskal)");
}

/* ================================================================ */
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_kruskal_produces_correct_mst);
    RUN_TEST(test_insertadjver_rejects_edge_at_capacity);
    RUN_TEST(test_insertadjver_rejects_oversized_vertex_id);
    return UNITY_END();
}
