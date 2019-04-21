import { allocateTypedArray } from "./TypedArrays.js"

export class SparseMatrix {
    constructor(valueType = "boolean", undefinedValue = false) {
        this.adjacencyList = new Map();
        this.valueType = valueType; // any or boolean or number
        this.undefinedValue = undefinedValue; // Returned when cell not contained in sparse representation
        this.elementCount = 0;
        this.highestXIndex = 0;
        this.highestYIndex = 0;
        this.highestValue = 0;
        this.compatibleWithCSR = true;
        this.isCSR = false;
    }
    setCell(x, y, value = true) {
        if (this.isCSR)
            throw new Error("Set to CSR, so data is now immutable");

        if (this.valueType !== "any" && typeof value !== this.valueType)
            throw new Error(`Type Mismatch. Expected ${this.valueType}. Received ${typeof value}`);

        this.validateCSRCompatibility(value);

        if (!this.adjacencyList.has(x)) this.adjacencyList.set(x, new Map());

        this.updateCounters(x, y, value);

        this.adjacencyList.get(x).set(y, value);
    }

    updateCounters(x, y) {
        if (x > this.highestXIndex) {
            this.highestXIndex = x;
        }
        if (y > this.highestYIndex) {
            this.highestYIndex = y;
        }
        // If not updating an existing cell, increment elementCount
        if (this.adjacencyList.get(x).has(y) == false) this.elementCount++;
    }

    validateCSRCompatibility(value) {
        if (
            (this.valueType === "boolean" && typeof value !== "boolean") ||
            (this.valueType === "number" && (typeof value !== "number" || !Number.isSafeInteger(value))))
            this.compatibleWithCSR = false;
    }

    getCell(x, y) {
        if (this.isCSR) {
            if (this.columnIsEmpty(x) || !this.columnContainsYIndex(x, y)) {
                return this.undefinedValue
            } else if (this.isBooleanMatrix()) {
                return true;
            } else {
                // We need to maintain index parity between a and ja, so we only slice the trailing data
                const cell = this.a[this.ja.slice(0, this.ia[x + 1]).indexOf(y, this.ia[0])];
                return this.processCSRValue(cell);
            }
        }
        return this.adjacencyList.has(x) && this.adjacencyList.get(x).get ? this.adjacencyList.get(x).get(y) : this.undefinedValue;
    }

    columnIsEmpty(x) {
        return this.ia[x + 1] == this.ia[x];
    }

    columnContainsYIndex(x, y) {
        return this.ja.slice(this.ia[x], this.ia[x + 1]).includes(y) == true;
    }

    processCSRValue(val) {
        return this.isCSR && this.valueType == "number" ? parseInt(val, 10) : val;
    }

    getColumn(x) {
        if (this.isCSR) {
            let result = new Map();
            for (let i = this.ia[x]; i < this.ia[x + 1]; i++) {
                result.set(this.ja[i], this.isBooleanMatrix() ? true : this.processCSRValue(this.a[i]));
            }
            return result;
        }
        return this.adjacencyList.has(x) ? this.adjacencyList.get(x) : new Map();
    }

    getColumnIndices() {
        return this.adjacencyList.keys();
    }

    // The column could "exist" logically, but be filled with only sparse values
    hasColumn(x) {
        if (this.isCSR) {
            // If column is out of bounds
            if (x > this.highestXIndex) {
                console.log("Error: out of bounds");
                return false
            };
            // If column is empty
            if (this.ia[x + 1] == this.ia[x]) {
                console.log("Column is empty");
                return false
            };
            return true;
        }

        if (x < 0 || x > this.highestXIndex) return false;
        return this.adjacencyList.has(x);
    }

    // Column Major Order CSR. Assumes value is boolean
    generateCSR() {
        if (this.compatibleWithCSR == false) {
            console.log("Not compatible with CSR");
            return
        };
        // I only need this.a if not a boolean matrix
        if (!this.isBooleanMatrix()) {
            let highestValue = 0;
            for (let column of this.adjacencyList.values()) {
                for (let value of column.values()) {
                    const valueAsInt = parseInt(value);
                    if (valueAsInt > highestValue) highestValue = valueAsInt;
                }
            }
            this.a = allocateTypedArray(highestValue, this.elementCount);
        }

        this.ia = allocateTypedArray(this.elementCount, this.highestXIndex + 1);
        this.ja = allocateTypedArray(this.highestYIndex, this.elementCount);

        this.ia[0] = 0;
        let xIndices = Array.from(this.adjacencyList.keys())
        xIndices = xIndices.map(a => parseInt(a, 10)).sort((a, b) => a - b);
        for (let x = 0; x <= this.highestXIndex; x++) {
            // Fill in any empty columns;
            if (this.hasColumn(x) == false) {
                this.ia[x + 1] = this.ia[x];
            } else {
                let column = this.getColumn(x);
                let tuples = Array.from(column.entries()).map(([k, v]) => [parseInt(k, 10), v]).sort(([a,], [b,]) => a - b);
                for (let i = 0; i < tuples.length; i++) {
                    const [k, v] = tuples[i];
                    this.ja[this.ia[x] + i] = k;
                    if (!this.isBooleanMatrix()) {
                        this.a[this.ia[x] + i] = parseInt(v, 10);
                    }
                }
                this.ia[x + 1] = this.ia[x] + tuples.length;
            }
        }
        this.isCSR = true;
        this.adjacencyList.clear();
    }

    isBooleanMatrix() {
        return this.valueType == "boolean";
    }
}