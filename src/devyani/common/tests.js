// export function runTests(kruskal) {
export function runTests(init, insertadjver, kruskal, printResults) {
    init();
    // const nodes = [0, 1, 2, 3, 4, 5, 6];
    const edges = [
        [0, 1, 7],
        [0, 3, 5],
        [1, 2, 8],
        [1, 3, 9],
        [1, 4, 7],
        [2, 4, 5],
        [3, 4, 15],
        [3, 5, 6],
        [4, 5, 8],
        [4, 6, 9],
        [5, 6, 11]
    ];
    edges.forEach(([source, destination, weight]) =>
        insertadjver(source, destination, weight)
    )
    kruskal();
    printResults();

    // console.log(kruskal(nodes, edges));
}