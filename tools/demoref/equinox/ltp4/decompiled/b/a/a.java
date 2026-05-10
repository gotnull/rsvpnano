/*
 * Decompiled with CFR 0.152.
 */
package b.a;

public class a {
    private static final int a = 1000;
    private int b;
    private int c;
    private int d;
    private int e;
    private int f;
    private int g;
    private int h;
    private int i;
    private int j;
    private int k;
    private int l;
    private int m;
    private int n;
    private int o;
    private int p;
    private int q;
    private int r;
    private int s;
    private int t;
    private int u;
    private int v;
    private int w;
    private int x;
    private int y;
    private int z;
    private int A;
    private int B;
    private int C;
    private float D;
    private String E;
    private boolean F;
    private int G;
    private int H;
    private int I;
    private byte J;
    private float K;
    public static int L;

    /*
     * Unable to fully structure code
     */
    public int a(b.e.a var1_1, int var2_2, int[] var3_3, int[] var4_4, int[] var5_5, int[] var6_6, int[] var7_7, int[] var8_8, int var9_9, float[] var10_10, float[] var11_11, int[] var12_12) {
        block22: {
            var13_13 = b.a.a.L;
            this.E = b.a.a.a("_7\u000fV\u0004");
            if (var1_1.e != 1 && var1_1.e != 2) break block22;
            this.A = var12_12[0];
            this.F = false;
            this.c = var2_2;
            this.d = 0;
            this.e = var1_1.c;
            this.E = b.a.a.a("<<@_U\u007f4\u0005J\u0005m!@u@`(\u0013\u001e\u0003/\u000b\u0014_Wc1\u000fP");
            this.f = 0;
            if (var13_13 == 0) ** GOTO lbl82
            var14_14 = b.e.a.m;
            b.e.a.m = ++var14_14;
            block0: while (true) {
                this.b = var9_9;
                this.F = false;
                v0 = --this.H;
                do {
                    block24: {
                        block23: {
                            if (v0 > 0) {
                                this.F = true;
                            }
                            this.g = var1_1.i[this.d++];
                            this.z = this.d;
                            this.i = this.c + var1_1.i[this.d++];
                            this.l = var3_3[this.i];
                            this.m = var4_4[this.i];
                            this.j = this.c + var1_1.i[this.d++];
                            this.o = var3_3[this.j];
                            this.p = var4_4[this.j];
                            this.k = this.c + var1_1.i[this.d++];
                            this.r = var3_3[this.k];
                            this.s = var4_4[this.k];
                            this.t = var5_5[this.k];
                            this.n = var5_5[this.i];
                            this.q = var5_5[this.j];
                            this.G = 2;
                            this.w = -((this.o - this.l) * (this.s - this.m) - (this.r - this.l) * (this.p - this.m));
                            if (this.w <= 0) break block23;
                            this.x = this.n + this.q + this.t;
                            this.y = this.g - 3 - 1;
                            if (var13_13 == 0) ** GOTO lbl45
                            block2: while (true) {
                                this.x += var5_5[this.c + var1_1.i[this.d + this.y]];
                                do {
                                    --this.y;
lbl45:
                                    // 2 sources

                                    if (this.y >= 0) continue block2;
                                    this.x /= this.g;
                                    var8_8[var9_9++] = var1_1.e;
                                    var8_8[var9_9++] = this.g;
                                    var8_8[var9_9++] = this.i;
                                    var8_8[var9_9++] = this.j;
                                    var8_8[var9_9++] = this.k;
                                    this.y = this.g - 3;
                                } while (var13_13 != 0);
                                break;
                            }
                            if (var13_13 == 0) ** GOTO lbl59
                            block4: while (true) {
                                var8_8[var9_9++] = this.c + var1_1.i[this.d++];
                                do {
                                    --this.y;
lbl59:
                                    // 2 sources

                                    if (this.y > 0) continue block4;
                                    this.J = (byte)-128;
                                    this.B = var1_1.i[this.d++];
                                    this.C = var1_1.i[this.d++];
                                    this.y = 0;
                                } while (var13_13 != 0);
                                break;
                            }
                            if (var13_13 == 0) ** GOTO lbl71
                            block6: while (true) {
                                this.D = var11_11[var1_1.i[this.z + this.y]];
                                var8_8[var9_9++] = (int)((float)this.B + (float)(this.C - this.B) * var10_10[(int)(1000.0f * this.D)]);
                                do {
                                    ++this.y;
lbl71:
                                    // 2 sources

                                    if (this.y < this.g) continue block6;
                                    var6_6[this.A] = this.x;
                                    var7_7[this.A] = this.b;
                                    ++this.A;
                                } while (var13_13 != 0);
                                break;
                            }
                            if (var13_13 == 0) break block24;
                        }
                        this.d += 2 + this.g - 3;
                        this.F = true;
                    }
                    ++this.f;
lbl82:
                    // 2 sources

                    if (this.f < this.e) continue block0;
                    var12_12[0] = this.A;
                    v0 = var9_9;
                } while (var13_13 != 0);
                break;
            }
            return v0;
        }
        this.E = b.a.a.a("_7\u000fV\u0004");
        this.A = var12_12[0];
        this.F = false;
        this.c = var2_2;
        this.d = 0;
        this.e = var1_1.c;
        this.E = b.a.a.a("<<@_U\u007f4\u0005J\u0005m!@u@`(\u0013\u001e\u0003/\u000b\u0014_Wc1\u000fP");
        this.f = 0;
        if (var13_13 == 0) ** GOTO lbl161
        block8: while (true) {
            this.b = var9_9;
            this.E = "?";
            v1 = --this.H;
            do {
                block26: {
                    block25: {
                        if (v1 > 0) {
                            this.F = true;
                        }
                        this.g = var1_1.i[this.d++];
                        this.z = this.d;
                        this.i = this.c + var1_1.i[this.d++];
                        this.l = var3_3[this.i];
                        this.m = var4_4[this.i];
                        this.j = this.c + var1_1.i[this.d++];
                        this.o = var3_3[this.j];
                        this.p = var4_4[this.j];
                        this.k = this.c + var1_1.i[this.d++];
                        this.G = 2;
                        this.r = var3_3[this.k];
                        this.s = var4_4[this.k];
                        this.t = var5_5[this.k];
                        this.n = var5_5[this.i];
                        this.q = var5_5[this.j];
                        this.G = 2;
                        this.w = -((this.o - this.l) * (this.s - this.m) - (this.r - this.l) * (this.p - this.m));
                        if (this.w <= 0) break block25;
                        this.x = this.n + this.q + this.t;
                        this.y = this.g - 3 - 1;
                        if (var13_13 == 0) ** GOTO lbl129
                        block10: while (true) {
                            this.x += var5_5[this.c + var1_1.i[this.d + this.y]];
                            do {
                                --this.y;
lbl129:
                                // 2 sources

                                if (this.y >= 0) continue block10;
                                this.x /= this.g;
                                this.G = 2;
                                var8_8[var9_9++] = 0;
                                var8_8[var9_9++] = this.g;
                                var8_8[var9_9++] = this.i;
                                var8_8[var9_9++] = this.j;
                                var8_8[var9_9++] = this.k;
                                this.J = 0;
                                this.y = this.g - 3;
                            } while (var13_13 != 0);
                            break;
                        }
                        if (var13_13 == 0) ** GOTO lbl145
                        block12: while (true) {
                            var8_8[var9_9++] = this.c + var1_1.i[this.d++];
                            do {
                                --this.y;
lbl145:
                                // 2 sources

                                if (this.y > 0) continue block12;
                                this.B = var1_1.i[this.d++];
                                this.C = var1_1.i[this.d++];
                                this.D = var11_11[this.f];
                                var8_8[var9_9++] = (int)((float)this.B + (float)(this.C - this.B) * var10_10[(int)(1000.0f * this.D)]);
                                var6_6[this.A] = this.x;
                                var7_7[this.A] = this.b;
                                ++this.A;
                                this.G = 2;
                            } while (var13_13 != 0);
                            break;
                        }
                        if (var13_13 == 0) break block26;
                    }
                    this.d += 2 + this.g - 3;
                    this.F = true;
                }
                ++this.f;
lbl161:
                // 2 sources

                if (this.f < this.e) continue block8;
                var12_12[0] = this.A;
                v1 = var9_9;
            } while (var13_13 != 0);
            break;
        }
        return v1;
    }

    /*
     * Handled impossible loop by adding 'first' condition
     * Enabled aggressive block sorting
     */
    private static String a(String string) {
        char[] cArray = string.toCharArray();
        int n2 = cArray.length;
        int n3 = 0;
        boolean bl = true;
        block6: do {
            if (bl && !(bl = false) && n2 > 1) continue;
            char[] cArray2 = cArray;
            int n4 = n3;
            while (true) {
                int n5;
                char c2 = cArray2[n4];
                switch (n3 % 5) {
                    case 0: {
                        n5 = 15;
                        break;
                    }
                    case 1: {
                        n5 = 88;
                        break;
                    }
                    case 2: {
                        n5 = 96;
                        break;
                    }
                    case 3: {
                        n5 = 62;
                        break;
                    }
                    default: {
                        n5 = 37;
                    }
                }
                cArray2[n4] = (char)(c2 ^ n5);
                ++n3;
                if (n2 != 0) continue block6;
                cArray2 = cArray;
                n4 = n2;
            }
        } while (n3 < n2);
        return new String(cArray);
    }
}

