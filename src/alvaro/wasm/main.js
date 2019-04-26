import { runTests } from "../common/tests.js"


async function main() {

    const matrixMultiplication = window.Module.cwrap("matrixMultiplication", null, ["number"])
    runTests(matrixMultiplication)
}
Module['onRuntimeInitialized'] = onRuntimeInitialized;

function onRuntimeInitialized() {
    main();
}