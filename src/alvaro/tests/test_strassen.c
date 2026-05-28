/* test_strassen.c
 *
 * Unity regression tests for Issue #12: pervasive memory leaks and unchecked
 * malloc in src/alvaro/wasm/main.c (Strassen matrix multiplication).
 *
 * Bugs:
 *   - multiplyStrassens allocates 8 sub-matrices (a11..a22, b11..b22) and
 *     8 intermediate product matrices (via recursive calls + add) but frees
 *     none of them.  The compound expressions like
 *       c11 = add(multiplyStrassens(...), multiplyStrassens(...), half);
 *     discard the inner return values — those matrices are permanently leaked.
 *   - matrixMultiplication allocates m1, m2, m3 and returns without freeing
 *     any of them.
 *   - initializeMatrix / initializeEmptyMatrix do not check malloc returns.
 *
 * Malloc/free injection technique
 * ---------------------------------
 * count_malloc() and count_free() are defined BEFORE the #define directives so
 * their bodies still call the real stdlib allocators.  freeMatrix() — a new
 * helper added by the fix — is defined AFTER #define free count_free so that
 * its free() calls are counted.  All allocations inside the subsequently-
 * included main.c are then transparently tracked.
 *
 * Test ordering:
 *   test 1 – multiplyStrassens correctness baseline (passes before and after)
 *   test 2 – matrixMultiplication must free every allocation it makes
 *             (FAIL before fix: alloc_count != free_count; PASS after fix)
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "unity.h"

/* ---- allocation counters ------------------------------------------ */

static int alloc_count = 0;
static int free_count  = 0;

/* Defined BEFORE #define malloc/free so their own bodies use real allocators */
static void *count_malloc(size_t size)
{
    alloc_count++;
    return malloc(size); /* real stdlib malloc — #define not yet active */
}

static void count_free(void *ptr)
{
    if (ptr != NULL)
        free_count++;
    free(ptr); /* real stdlib free — #define not yet active */
}

/* ---- malloc/free injection ----------------------------------------- */

#define malloc count_malloc
#define free   count_free

/* freeMatrix is defined HERE (after #define free) so its free() calls are
 * counted.  The #define HAVE_FREE_MATRIX guard prevents main.c from
 * re-defining the same function when it is included below.             */
static void freeMatrix(int **m, int size)
{
    if (m == NULL)
        return;
    for (int i = 0; i < size; i++)
        free(m[i]); /* counted */
    free(m);        /* counted */
}
#define HAVE_FREE_MATRIX 1

#include "../wasm/main.c"

#undef malloc
#undef free

/* ---- Unity lifecycle ---------------------------------------------- */

void setUp(void)    { alloc_count = 0; free_count = 0; }
void tearDown(void) {}

/* ================================================================
 * Test 1 – multiplyStrassens correctness baseline
 *
 * For an NxN matrix of all-2s multiplied by an NxN matrix of all-3s,
 * every result element must equal N * (2*3) = 6N.
 * Uses the base-case multiply() path (N=4 < 256 threshold).
 * Passes before and after the fix.
 * ================================================================ */
void test_multiplyStrassens_produces_correct_result(void)
{
    int N = 4;
    int **m1     = initializeMatrix(N, 2);
    int **m2     = initializeMatrix(N, 3);
    int **result = multiplyStrassens(m1, m2, N);

    TEST_ASSERT_NOT_NULL_MESSAGE(result, "multiplyStrassens must return non-NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(N * 6, result[0][0],
        "result[0][0] must equal N*2*3 = 24");
    TEST_ASSERT_EQUAL_INT_MESSAGE(N * 6, result[N - 1][N - 1],
        "result[N-1][N-1] must equal N*2*3 = 24");

    freeMatrix(m1,     N);
    freeMatrix(m2,     N);
    freeMatrix(result, N);
}

/* ================================================================
 * Test 2 – matrixMultiplication must not leak memory (Issue #12)
 *
 * Bug:  matrixMultiplication allocates m1, m2, m3 and returns without
 *       freeing any of them.  multiplyStrassens similarly leaks all 8
 *       sub-matrix allocations and all 8 intermediate product matrices.
 * Fix:  add freeMatrix() helper; free every intermediate and result
 *       matrix before returning.
 *
 * Before fix: alloc_count > 0, free_count == 0 → TEST FAILS
 * After  fix: alloc_count == free_count         → TEST PASSES
 *
 * NOTE: this test is placed last so test-1 results are visible even
 * if an unfixed crash occurs (though here the bug is a silent leak,
 * not a crash).
 * ================================================================ */
void test_matrixMultiplication_frees_all_matrices(void)
{
    /* alloc_count and free_count are 0 from setUp() */
    matrixMultiplication(4); /* small size — uses base-case multiply() */

    TEST_ASSERT_GREATER_THAN_INT_MESSAGE(0, alloc_count,
        "sanity: matrixMultiplication must allocate at least one matrix");
    TEST_ASSERT_EQUAL_INT_MESSAGE(alloc_count, free_count,
        "Issue #12: matrixMultiplication leaks memory "
        "(expected alloc_count == free_count; before fix free_count == 0)");
}

/* ================================================================ */
int main(void)
{
    setbuf(stdout, NULL); /* unbuffered so results survive any crash */
    UNITY_BEGIN();
    RUN_TEST(test_multiplyStrassens_produces_correct_result);
    RUN_TEST(test_matrixMultiplication_frees_all_matrices);
    return UNITY_END();
}
