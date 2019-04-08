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
        return this.data.getColumn(source);
    }

    hasSourceVertex(source) {
        return this.data.hasColumnIndex(source);
    }

    getSourceVertices() {
        return this.data.getColumnIndices();
    }

    printGraph() {
        for (let sourceVertex of this.getSourceVertices()) {
            for (let [destinationVertex] of this.getNeighbors(sourceVertex)) {
                console.log(`${sourceVertex} -> ${destinationVertex}`)
            }
        }
    }

    bfs(start) {
        if (this.hasSourceVertex(start) == false) {
            console.log(`${start} not found`);
            return;
        }
        const queue = [];
        const isDiscovered = {}
        const hasParent = {}
        queue.push(start);
        isDiscovered[start] = true;
        hasParent[start] = start;

        // Find a connected component...
        while (queue.length > 0) {
            let vertex = queue.shift();
            for (let [neighbor] of this.getNeighbors(vertex)) {
                if (!isDiscovered[neighbor]) {
                    queue.push(neighbor);
                    isDiscovered[neighbor] = true;
                    hasParent[neighbor] = vertex;
                }
            }
        }
        return hasParent;
        // for (let [child, parent] of Object.entries(hasParent)) {
        //     console.log(`${child} has parent ${parent}`)
        // }
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

export function runBFS(root) {
    hasParent = myGraph.bfs(root);
}

export function getParent(vertex) {
    return hasParent[vertex];
}
