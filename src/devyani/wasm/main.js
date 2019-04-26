import { runTests } from "../common/tests.js"


async function main() {

    const init = window.Module.cwrap("init", null);
    const insertadjver = window.Module.cwrap("insertadjver", null, ["number", "number", "number"]);
    const kruskal = window.Module.cwrap("kruskal", null);
    const printResults = window.Module.cwrap("printResults", null);
    runTests(init, insertadjver, kruskal, printResults)
}
Module['onRuntimeInitialized'] = onRuntimeInitialized;

function onRuntimeInitialized() {
    main();
}