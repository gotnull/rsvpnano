/*
 * Decompiled with CFR 0.152.
 */
package b.f;

import b.f.b;

public class a {
    private int[] a;
    private int[] b;
    private int c;
    private int d;
    private int e;
    private String f;
    private boolean g;
    private int h;
    private int i = 3;
    private float j = 2.2f;

    public void a(int n2, int n3, int[] nArray, int[] nArray2) {
        boolean bl = b.f.b.I;
        this.f = b.f.a.a("fDLf\u001bEF\tk\u0000[");
        this.a = nArray;
        this.b = nArray2;
        this.h = 1;
        this.a(n2, n3);
        this.g = false;
        if (b.e.a.m != 0) {
            b.f.b.I = !bl;
        }
    }

    /*
     * Unable to fully structure code
     */
    private void a(int var1_1, int var2_2) {
        block3: {
            var4_3 = b.f.b.I;
            if (var2_2 - var1_1 < 1) break block3;
            this.d = this.a[var1_1];
            var3_4 = var1_1;
            this.e = var1_1 + 1;
            if (!var4_3) ** GOTO lbl21
            block0: while (true) {
                if (this.a[this.e] < this.d) {
                    this.c = this.a[++var3_4];
                    this.a[var3_4] = this.a[this.e];
                    this.a[this.e] = this.c;
                    this.c = this.b[var3_4];
                    this.b[var3_4] = this.b[this.e];
                    this.b[this.e] = this.c;
                }
                v0 = this;
                v1 = v0;
                v2 = v0.e;
                v3 = 1;
                do {
                    v1.e = v2 + v3;
lbl21:
                    // 2 sources

                    if (this.e < var2_2 + 1) continue block0;
                    this.c = this.a[var3_4];
                    this.a[var3_4] = this.a[var1_1];
                    this.a[var1_1] = this.c;
                    this.c = this.b[var3_4];
                    this.b[var3_4] = this.b[var1_1];
                    this.b[var1_1] = this.c;
                    this.a(var1_1, var3_4 - 1);
                    v1 = this;
                    v2 = var3_4 + 1;
                    v3 = var2_2;
                } while (var4_3);
                break;
            }
            v1.a(v2, v3);
        }
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
                        n5 = 40;
                        break;
                    }
                    case 1: {
                        n5 = 43;
                        break;
                    }
                    case 2: {
                        n5 = 108;
                        break;
                    }
                    case 3: {
                        n5 = 5;
                        break;
                    }
                    default: {
                        n5 = 116;
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

