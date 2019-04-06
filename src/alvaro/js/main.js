import { runTests } from "../common/tests.js"

function fibonacci(iterations) {
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

async function main() {
    runTests(fibonacci);
}
main()