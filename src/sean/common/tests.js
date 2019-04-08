const vertices = new Set();
let edgeCount = 0;
const TEST_RUN_COUNT = 3;

function getRandomVertex() {
    const verticesAsArr = [...vertices];
    const randomIdx = Math.floor(Math.random() * verticesAsArr.length);
    return verticesAsArr[randomIdx]
}
export async function runTests(createGraph, insertEdge, runBFS, getParent) {
    console.log("Creating Graph")
    createGraph();
    console.log("Downloading Edgelist")
    const response = await fetch("../../16x16_edgelist.txt");
    const edgelist = await response.text();
    console.log("Adding Edges to Graph")
    edgelist.split("\n").forEach(string => {
        edgeCount++;
        const [source, destination] = string.split(" ");
        vertices.add(source);
        vertices.add(destination);
        insertEdge(source, destination);
    });
    console.log(`Added ${edgeCount} Edges to Graph`)
    for (let i = 0; i < TEST_RUN_COUNT; i++) {
        const start = getRandomVertex();
        console.log(`Starting BFS with Vertex #${start}`);
        console.time(`BFS with Vertex #${start}`);
        runBFS(start);
        console.timeEnd(`BFS with Vertex #${start}`);
    }
    // vertices.forEach(vertex => console.log(`${vertex} has parent ${getParent(vertex)}`));
}