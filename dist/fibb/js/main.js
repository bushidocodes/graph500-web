function fibb(iterations) {
    let val = 1;
    let last = 0;

    if (iterations == 0) {
        return 0;
    }
    for (let i = 1; i < iterations; i++) {
        let seq;

        seq = val + last;
        last = val;
        val = seq;
    }
    return val;
}

[19, 20, 21, 22, 23].forEach(desiredSolution => {
    console.time(`fibb(${desiredSolution})`);
    console.log(fibb(desiredSolution))
    console.timeEnd(`fibb(${desiredSolution})`);
})