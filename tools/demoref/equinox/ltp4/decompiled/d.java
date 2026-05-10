/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  d
 */
class d {
    boolean a;

    boolean a(int[] nArray, int n2, int n3) {
        this.a = false;
        if (nArray[n2 * 4 + 1] == nArray[n3 * 4 + 1]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 1] == nArray[n3 * 4 + 2]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 1] == nArray[n3 * 4 + 3]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 2] == nArray[n3 * 4 + 1]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 2] == nArray[n3 * 4 + 2]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 2] == nArray[n3 * 4 + 3]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 3] == nArray[n3 * 4 + 1]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 3] == nArray[n3 * 4 + 2]) {
            this.a = true;
        }
        if (nArray[n2 * 4 + 3] == nArray[n3 * 4 + 3]) {
            this.a = true;
        }
        return this.a;
    }

    d() {
    }
}

