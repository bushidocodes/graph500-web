import { runTests } from "../common/tests.js"

const edges = []
const nodes = new Set();
const mst = [];

function init() { };

function insertajdver(source, destination, weight) {
    edges.push([source, destination, weight]);
    nodes.add(source);
    nodes.add(destination);
}

function kruskal() {

    let forest = Array.from(nodes).map(node => [node])
    edges.sort((a, b) => b[2] - a[2])
    while (forest.length > 1) {
        const edge = edges.pop();
        const [n1, n2, _] = edge;
        const t1 = f tree => tree.includes(n1))[0];
        const t2 = forest.filter(tree => tree.includes(n2))[0];

        if (t1 != t2) {
            forest = forest.filter(t => t[0] != t1[0] && t[0] != t2[0]);
            const union = [...t1, ...t2];
            forest.push(union);
            mst.push(edge);
        }
    }
}

function printResults() {
    console.log(mst);
}

async function main() {
    runTests(init, insertajdver, kruskal, printResults);
}
main()