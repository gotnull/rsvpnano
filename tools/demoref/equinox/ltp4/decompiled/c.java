/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  c
 */
class c {
    boolean a(float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, int n2, int[] nArray, float[] fArray, float[] fArray2, float[] fArray3, float f11) {
        boolean bl = false;
        boolean bl2 = false;
        boolean bl3 = false;
        boolean bl4 = false;
        boolean bl5 = false;
        boolean bl6 = false;
        if (f2 == 0.0f && f3 == 0.0f && f4 == 0.0f) {
            return false;
        }
        float f12 = f2 * (fArray[nArray[n2 * 4 + 1]] - f8) + f3 * (fArray2[nArray[n2 * 4 + 1]] - f9) + f4 * (fArray3[nArray[n2 * 4 + 1]] - f10);
        float f13 = f2 * (fArray[nArray[n2 * 4 + 2]] - f8) + f3 * (fArray2[nArray[n2 * 4 + 2]] - f9) + f4 * (fArray3[nArray[n2 * 4 + 2]] - f10);
        float f14 = f2 * (fArray[nArray[n2 * 4 + 3]] - f8) + f3 * (fArray2[nArray[n2 * 4 + 3]] - f9) + f4 * (fArray3[nArray[n2 * 4 + 3]] - f10);
        if (Math.abs(f12) == 0.0f) {
            bl = true;
        }
        if (Math.abs(f13) == 0.0f) {
            bl2 = true;
        }
        if (Math.abs(f14) == 0.0f) {
            bl3 = true;
        }
        f2 = this.a(f2);
        f3 = this.a(f3);
        f4 = this.a(f4);
        f5 = this.a(f5);
        f6 = this.a(f6);
        f7 = this.a(f7);
        if (f2 == f5) {
            bl4 = true;
        }
        if (f3 == f6) {
            bl5 = true;
        }
        if (f4 == f7) {
            bl6 = true;
        }
        return bl && bl2 && bl3 && bl4 && bl5 && bl6;
    }

    int a(float f2) {
        if (f2 > 0.0f) {
            return 1;
        }
        if (f2 < 0.0f) {
            return -1;
        }
        return 0;
    }

    c() {
    }
}

