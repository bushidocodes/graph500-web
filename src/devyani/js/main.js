import { runTests } from "../common/tests.js"

function kruskal(nodes, edges) {
    const mst = [];
    let forest = nodes.map(node => [node])
    edges.sort((a, b) => a[2] - b[2])
    while (forest.length > 1) {
        const edge = edges.pop();
        const [n1, n2, weight] = edge;
        const t1 = forest.filter(tree => tree.includes(n1))[0];
        const t2 = forest.filter(tree => tree.includes(n2))[0];

        if (t1 != t2) {
            forest = forest.filter(t => t[0] != t1[0] && t[0] != t2[0]);
            const union = [...t1, ...t2];
            forest.push(union);
            mst.push(edge);
        }
    }
    return mst;
}

async function main() {
    runTests(kruskal);
}
main()