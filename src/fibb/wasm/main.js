import { runTests } from "../common/tests.js"


async function main() {

    const fibonacci = window.Module.cwrap("fibonacci", "int", ["int"]);
    runTests(fibonacci);
}
Module['onRuntimeInitialized'] = onRuntimeInitialized;

function onRuntimeInitialized() {
    main();
}