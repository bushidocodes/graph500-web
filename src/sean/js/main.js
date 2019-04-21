import { runTests } from "../common/tests.js"
import { createGraph, insertEdge, runBFS, getParent, compressData } from "./graph.js"

async function main() {
    runTests(createGraph, insertEdge, runBFS, getParent, compressData);
}
main()