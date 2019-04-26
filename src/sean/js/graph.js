import { SparseMatrix } from "./SparseMatrix.js";

export class Graph {
    constructor(isDirected) {
        this.data = new SparseMatrix();
        this.isDirected = !!isDirected;
    }

    insertEdge(source, destination, isDirected = this.isDirected) {
        this.data.setCell(source, destination);
        if (!isDirected) {
            this.insertEdge(destination, source, true);
        }
    }

    getNeighbors(source) {
        return Array.from(this.data.getColumn(source).entries()).filter(([k, v]) => v).map(([k, v]) => k);
    }

    hasSourceVertex(source) {
        return this.data.hasColumn(source);
    }

    bfs(start) {
        if (this.hasSourceVertex(start) == false) {
            console.log(`${start} not found`);
            return;
        }
        const queue = [];
        const isDiscovered = new Set();
        const hasParent = {}
        queue.push(start);
        isDiscovered[start] = true;
        hasParent[start] = start;

        // Find a connected component... but the edgeset is fully connected
        while (queue.length > 0) {
            let vertex = queue.shift();
            this.getNeighbors(vertex).forEach((neighbor => {
                if (isDiscovered.has(neighbor) === false) {
                    queue.push(neighbor);
                    isDiscovered.add(neighbor);
                    hasParent[neighbor] = vertex;
                }
            }));
        }
        return hasParent;
    }
}

// Public API to keep API consistency with WASM Bridge

let myGraph;
let hasParent;

export function createGraph(isDirected) {
    myGraph = new Graph(false);
}

export function insertEdge(source, destination) {
    myGraph.insertEdge(source, destination);
}

export function compressData() {
    myGraph.data.generateCSR();
}

export function runBFS(root) {
    hasParent = myGraph.bfs(root);
}

export function getParent(vertex) {
    return hasParent[vertex];
}
