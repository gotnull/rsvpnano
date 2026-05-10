/*
 * Decompiled with CFR 0.152.
 */
package b.f;

import b.e.a;

public class b {
    private String a;
    private boolean b;
    private int c;
    private int d = 2;
    private float e = 1.1f;
    private float f;
    private float g;
    private float h;
    private float i;
    private float j;
    private float k;
    private float l;
    private float m;
    private float n;
    private float o;
    private float p;
    private float q;
    private float r;
    private float s;
    private float t;
    private float u;
    private float v;
    private float w;
    private float x;
    private float y;
    private float z;
    private float A;
    private int B;
    private float C;
    private float D;
    private float E;
    private int F;
    private int G;
    private int H;
    public static boolean I;

    /*
     * Unable to fully structure code
     */
    public void a(a var1_1, int var2_2, float var3_3, float var4_4, float var5_5, float var6_6, float var7_7, float var8_8, int[] var9_9, int[] var10_10, int[] var11_11, float[] var12_12, int var13_13, int var14_14) {
        var15_15 = b.f.b.I;
        this.f = (float)Math.cos(var3_3);
        this.g = (float)Math.sin(var3_3);
        this.h = (float)Math.cos(var4_4);
        this.i = (float)Math.sin(var4_4);
        this.j = (float)Math.cos(var5_5);
        this.k = (float)Math.sin(var5_5);
        this.c = 1;
        this.s = this.f * this.h;
        this.t = -(this.f * this.i * this.k + this.g * this.j);
        this.u = -(this.f * this.i * this.j - this.g * this.k);
        this.v = this.g * this.h;
        this.a = b.f.b.a("Y<\n4\u0002H~WmBHyJw_\u001b0YmIHrWqB");
        this.w = -(this.g * this.i * this.k - this.f * this.j);
        this.x = -(this.g * this.i * this.j + this.f * this.k);
        this.y = this.i;
        this.z = this.h * this.k;
        this.A = this.h * this.j;
        this.B = var1_1.b;
        this.a = b.f.b.a("\\<\r4\u0007HsM}X\u0004|Qj\u0011\fuK8R\rbQkT\u001b>\u00166");
        this.G = var2_2;
        this.F = 0;
        this.H = this.B;
        if (!var15_15) ** GOTO lbl41
        var16_16 = b.e.a.m;
        b.e.a.m = ++var16_16;
        block0: while (true) {
            this.c = 2;
            this.p = var1_1.g[this.F];
            this.q = var1_1.h[this.F];
            this.r = var1_1.f[this.F++];
            this.m = this.s * this.p + this.t * this.q + this.u * this.r + var6_6;
            this.o = this.v * this.p + this.w * this.q + this.x * this.r + var7_7;
            this.n = this.y * this.p + this.z * this.q + this.A * this.r + var8_8;
            this.l = 400.0f / (400.0f - this.n);
            var9_9[this.G] = (int)(this.m * this.l + (float)(var13_13 / 2));
            var10_10[this.G] = (int)(-this.o * this.l + (float)(var14_14 / 2));
            var11_11[this.G++] = (int)this.n;
            do {
                --this.H;
lbl41:
                // 2 sources

                if (this.H > 0) continue block0;
                this.H = this.B;
                this.F = 0;
            } while (var15_15);
            break;
        }
        if (var1_1.e == 0) {
            this.H = var1_1.c;
        }
        if (!var15_15) ** GOTO lbl55
        do {
            this.C = var1_1.j[this.F];
            this.D = var1_1.k[this.F];
            this.E = var1_1.l[this.F];
            this.n = this.y * this.C + this.z * this.D + this.A * this.E;
            var12_12[this.F++] = Math.min(1.0f, Math.max(0.0f, this.n));
            --this.H;
lbl55:
            // 2 sources

        } while (this.H > 0);
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
                        n5 = 104;
                        break;
                    }
                    case 1: {
                        n5 = 16;
                        break;
                    }
                    case 2: {
                        n5 = 56;
                        break;
                    }
                    case 3: {
                        n5 = 24;
                        break;
                    }
                    default: {
                        n5 = 49;
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

