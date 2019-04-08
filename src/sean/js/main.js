import { runTests } from "../common/tests.js"
import { createGraph, insertEdge, runBFS, getParent } from "./graph.js"

async function main() {
    runTests(createGraph, insertEdge, runBFS, getParent);
}
main()