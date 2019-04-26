import { runTests } from "../common/tests.js"

// Constructor Function
class Matrix {
    constructor(width, height, value = 0) {
        this.W = width;
        this.H = height;
        this.element = [];

        for (let y = 0; y < height; y++) {
            this.element[y] = []
            for (let x = 0; x < width; x++) {
                this.element[y][x] = value;
            }
        }
    }

    print = function (string = "") {
        if (!string == "") console.log(string);
        console.log(this.element.join('\n'));
    }

    static add(m1, m2) {
        let m3 = new Matrix(m1.W, m1.H);
        for (let y = 0; y < m1.H; y++) {
            for (let x = 0; x < m2.W; x++) {
                m3.element[y][x] = m1.element[y][x] + m2.element[y][x];
            }
        }
        return m3;
    }

    static multiply(m1, m2) {
        let m3 = new Matrix(m1.W, m1.H);

        for (let y = 0; y < m1.H; y++) {
            for (let x = 0; x < m1.W; x++) {
                let sum = 0;
                for (let z = 0; z < m1.W; z++) {
                    sum += m1.element[x][z] * m2.element[z][y];
                }
                m3.element[x][y] = sum;
            }
        }
        return m3;
    }


    getSubMatrix(xSection, ySection) {
        let fromX, fromY;
        if (xSection == 1) fromX = 0;
        else fromX = this.W / 2;

        if (ySection == 1) fromY = 0;
        else fromY = this.W / 2;

        let m = new Matrix(this.W / 2, this.H / 2);

        for (var y = 0; y < m.H; y++) {
            for (var x = 0; x < m.W; x++) {
                m.element[y][x] = this.element[y + fromY][x + fromX];
            }
        }
        return m;
    }

    static getSuperMatrix(C11, C12, C21, C22) {
        let C = new Matrix(C11.W * 2, C11.H * 2);
        let size = C11.W;
        for (var y = 0; y < size; y++) {
            for (var x = 0; x < size; x++) {
                C.element[x][y] = C11.element[x][y];
                C.element[x][y + size] = C12.element[x][y];
                C.element[x + size][y] = C21.element[x][y];
                C.element[x + size][y + size] = C22.element[x][y];
            }
        }

        return C;
    }

    static multipyStrassens(m1, m2) {

        if (m1.H < 256) return Matrix.multiply(m1, m2);

        let A11 = m1.getSubMatrix(1, 1);
        let A12 = m1.getSubMatrix(1, 2);
        let A21 = m1.getSubMatrix(2, 1);
        let A22 = m1.getSubMatrix(2, 2);

        let B11 = m2.getSubMatrix(1, 1);
        let B12 = m2.getSubMatrix(1, 2);
        let B21 = m2.getSubMatrix(2, 1);
        let B22 = m2.getSubMatrix(2, 2);

        let C11 = Matrix.add(Matrix.multipyStrassens(A11, B11), Matrix.multipyStrassens(A12, B21));
        let C12 = Matrix.add(Matrix.multipyStrassens(A11, B12), Matrix.multipyStrassens(A12, B22));
        let C21 = Matrix.add(Matrix.multipyStrassens(A21, B11), Matrix.multipyStrassens(A22, B21));
        let C22 = Matrix.add(Matrix.multipyStrassens(A21, B12), Matrix.multipyStrassens(A22, B22));

        let C = Matrix.getSuperMatrix(C11, C12, C21, C22);
        return C;
    }

}

function matrixMultiplication(matrixSize) {
    let m1 = new Matrix(matrixSize, matrixSize, 2);
    let m2 = new Matrix(matrixSize, matrixSize, 3);

    console.log("Multiplication started")
    let start = new Date().getTime();
    let m6 = Matrix.multipyStrassens(m1, m2);
    let end = new Date().getTime();
    console.log("Strassens process time for " + matrixSize + "x" + matrixSize + " matrix multiplication: " + (end - start) + " ms");
}


async function main() {
    runTests(matrixMultiplication);
}
main()