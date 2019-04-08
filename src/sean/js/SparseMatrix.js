
// JS doesn't guarantee memory locality, so use nested Maps to create a sparse matrix...
// This is basically an adjacency list
export class SparseMatrix {
    constructor(undefinedValue = false) {
        this._data = new Map();
        this.undefinedValue = undefinedValue;
    }
    setCell(x, y, value = true) {
        if (!this._data.has(x)) {
            this._data.set(x, new Map());
        }
        this._data.get(x).set(y, value);
    }
    getCell(x, y) {
        return this._data.has(x) && this._data.get(x).get ? this._data.get(x).get(y) : this.undefinedValue;
    }
    getColumn(x) {
        return this._data.has(x) && this._data.get(x).entries ? this._data.get(x).entries() : undefined;
    }
    getColumnIndices() {
        return this._data.keys();
    }
    hasColumnIndex(source) {
        return this._data.has(source);
    }
    getUnionOfRowAndCellIndices() {
        const values = new Set();
        for (let [x, row] of this._data.entries()) {
            values.add(x);
            for (let y of row.keys()) {
                values.add(y);
            }
        }
        return values;
    }
}