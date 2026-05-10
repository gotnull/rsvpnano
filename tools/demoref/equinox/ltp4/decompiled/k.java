/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  k
 */
import b.e.a;

class k {
    void a(float[] fArray, float[] fArray2, float[] fArray3, float[] fArray4, float[] fArray5, float[] fArray6, int[] nArray, int n2, int n3) {
        boolean bl = Troisd.gq;
        int n4 = nArray[n2 * 4 + 1];
        nArray[n2 * 4 + 1] = nArray[n3 * 4 + 1];
        nArray[n3 * 4 + 1] = n4;
        n4 = nArray[n2 * 4 + 2];
        nArray[n2 * 4 + 2] = nArray[n3 * 4 + 2];
        nArray[n3 * 4 + 2] = n4;
        n4 = nArray[n2 * 4 + 3];
        nArray[n2 * 4 + 3] = nArray[n3 * 4 + 3];
        nArray[n3 * 4 + 3] = n4;
        float f2 = fArray[n2];
        fArray[n2] = fArray[n3];
        fArray[n3] = f2;
        f2 = fArray2[n2];
        fArray2[n2] = fArray2[n3];
        fArray2[n3] = f2;
        f2 = fArray3[n2];
        fArray3[n2] = fArray3[n3];
        fArray3[n3] = f2;
        f2 = fArray4[n2];
        fArray4[n2] = fArray4[n3];
        fArray4[n3] = f2;
        f2 = fArray5[n2];
        fArray5[n2] = fArray5[n3];
        fArray5[n3] = f2;
        f2 = fArray6[n2];
        fArray6[n2] = fArray6[n3];
        fArray6[n3] = f2;
        if (a.m != 0) {
            Troisd.gq = !bl;
        }
    }

    k() {
    }
}

