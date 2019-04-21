export function allocateTypedArray(maxElementSize, length) {
    let typedArray;
    if (maxElementSize <= 255) {
        // Use Uint8
        typedArray = new Uint8Array(new ArrayBuffer(length * 1));
    } else if (maxElementSize <= 65535) {
        // Use Uint16
        typedArray = new Uint16Array(new ArrayBuffer(length * 2));
    } else {
        // Use Uint32
        typedArray = new Uint32Array(new ArrayBuffer(length * 4));
    }
    return typedArray;
}
