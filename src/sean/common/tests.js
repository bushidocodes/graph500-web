const vertices = new Set();
let edgeCount = 0;
const TEST_RUN_COUNT = 3;

function getRandomVertex() {
    const verticesAsArr = [...vertices];
    const randomIdx = Math.floor(Math.random() * verticesAsArr.length);
    return verticesAsArr[randomIdx]
}
export async function runTests(createGraph, insertEdge, runBFS, getParent, compressData) {
    console.log("Creating Graph")
    createGraph();
    console.log("Downloading Edgelist")
    const response = await fetch("../../16x16_edgelist.txt");
    const edgelist = await response.text();
    console.log("Adding Edges to Graph")
    const edges = edgelist.split("\n");
    try {
        edges.forEach(string => {
            const [source, destination] = string.split(" ");
            const sourceAsInt = parseInt(source, 10);
            const destinationAsInt = parseInt(destination, 10);
            vertices.add(sourceAsInt);
            vertices.add(destinationAsInt);
            insertEdge(sourceAsInt, destinationAsInt);
            edgeCount++;
        });
    } catch (error) {
        console.error(`Failed to insertEdge. Successfully allocated ${edgeCount} / ${edges.length}. This is likely due to malloc failing.`);
    }
    for (let i = 0; i < TEST_RUN_COUNT; i++) {
        const start = getRandomVertex();
        console.log(`Starting BFS with Vertex #${start} `);
        console.time(`BFS with Vertex #${start} `);
        runBFS(start);
        console.timeEnd(`BFS with Vertex #${start} `);
    }
    compressData();
    for (let i = 0; i < TEST_RUN_COUNT; i++) {
        const start = getRandomVertex();
        console.log(`Starting BFS with Vertex #${start} `);
        console.time(`CSR BFS with Vertex #${start} `);
        runBFS(start);
        console.timeEnd(`CSR BFS with Vertex #${start} `);
    }
    // vertices.forEach(vertex => console.log(`${vertex} has parent ${getParent(vertex)} `));
}