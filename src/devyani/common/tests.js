export function runTests(init, insertadjver, kruskal, printResults) {
    init();
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
    console.time(`Kruskal`);
    kruskal();
    console.timeEnd(`Kruskal`);
    printResults();
}