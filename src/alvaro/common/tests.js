export function runTests(fibonacci) {
    [19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30].forEach(desiredSolution => {
        console.time(`fibonacci(${desiredSolution})`);
        console.log(fibonacci(desiredSolution))
        console.timeEnd(`fibonacci(${desiredSolution})`);
    })
}