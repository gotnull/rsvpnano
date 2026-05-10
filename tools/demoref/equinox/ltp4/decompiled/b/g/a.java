/*
 * Decompiled with CFR 0.152.
 */
package b.g;

public class a {
    private int a;
    public static boolean b;

    /*
     * Unable to fully structure code
     */
    public void a(b.e.a var1_1, int var2_2, int var3_3, int var4_4, int var5_5, int var6_6, int var7_7) {
        var10_8 = b.g.a.b;
        this.a = 0;
        var1_1.g[this.a] = 0.0f;
        var1_1.h[this.a] = var4_4;
        var1_1.f[this.a++] = 0.0f;
        var8_9 = 1;
        if (!var10_8) ** GOTO lbl21
        var11_10 = b.e.a.m;
        b.e.a.m = ++var11_10;
        block0: while (true) {
            v0 = var9_11 = 0;
            do {
                if (!var10_8) ** GOTO lbl19
                do {
                    var1_1.g[this.a] = (float)((double)var4_4 * Math.sin(3.141592653589793 * (double)var8_9 / (double)var3_3) * Math.sin(6.283185307179586 * ((double)var9_11 + (double)var7_7 * 0.5 * (double)(var8_9 % 2)) / (double)var2_2));
                    var1_1.h[this.a] = (float)((double)var4_4 * Math.cos(3.141592653589793 * (double)var8_9 / (double)var3_3));
                    var1_1.f[this.a++] = (float)((double)var4_4 * Math.sin(3.141592653589793 * (double)var8_9 / (double)var3_3) * Math.cos(6.283185307179586 * ((double)var9_11 + (double)var7_7 * 0.5 * (double)(var8_9 % 2)) / (double)var2_2));
                    ++var9_11;
lbl19:
                    // 2 sources

                } while (var9_11 < var2_2);
                ++var8_9;
lbl21:
                // 2 sources

                if (var8_9 < var3_3) continue block0;
                var1_1.g[this.a] = 0.0f;
                var1_1.h[this.a] = -var4_4;
                var1_1.f[this.a++] = 0.0f;
                this.a = 0;
                v0 = 0;
            } while (var10_8);
            break;
        }
        var8_9 = v0;
        if (!var10_8) ** GOTO lbl40
        block3: while (true) {
            if (var8_9 % 2 == 0) {
                var1_1.i[this.a++] = 3;
                var1_1.i[this.a++] = 0;
                var1_1.i[this.a++] = var8_9 + 1;
                var1_1.i[this.a++] = (var8_9 + 1) % var2_2 + 1;
                var1_1.i[this.a++] = var5_5;
                var1_1.i[this.a++] = var6_6;
            }
            do {
                ++var8_9;
lbl40:
                // 2 sources

                if (var8_9 < var2_2) continue block3;
                var8_9 = 0;
            } while (var10_8);
            break;
        }
        if (!var10_8) ** GOTO lbl60
        block5: while (true) {
            v1 = var9_11 = 0;
            do {
                if (!var10_8) ** GOTO lbl58
                do {
                    if ((var9_11 + var8_9) % 2 == 1) {
                        var1_1.i[this.a++] = 4;
                        var1_1.i[this.a++] = var9_11 + var8_9 * var2_2 + 1;
                        var1_1.i[this.a++] = var9_11 + (var8_9 + 1) % var3_3 * var2_2 + 1;
                        var1_1.i[this.a++] = (var9_11 + 1) % var2_2 + (var8_9 + 1) % var3_3 * var2_2 + 1;
                        var1_1.i[this.a++] = (var9_11 + 1) % var2_2 + var8_9 * var2_2 + 1;
                        var1_1.i[this.a++] = var5_5;
                        var1_1.i[this.a++] = var6_6;
                    }
                    ++var9_11;
lbl58:
                    // 2 sources

                } while (var9_11 < var2_2);
                ++var8_9;
lbl60:
                // 2 sources

                if (var8_9 < var3_3 - 2) continue block5;
                v1 = 0;
            } while (var10_8);
            break;
        }
        var8_9 = v1;
        if (!var10_8) ** GOTO lbl74
        do {
            if (var8_9 % 2 == 1) {
                var1_1.i[this.a++] = 3;
                var1_1.i[this.a++] = (var8_9 + 1) % var2_2 + 1 + var2_2 * (var3_3 - 2);
                var1_1.i[this.a++] = var8_9 + 1 + var2_2 * (var3_3 - 2);
                var1_1.i[this.a++] = var2_2 * (var3_3 - 1) + 2 - 1;
                var1_1.i[this.a++] = var5_5;
                var1_1.i[this.a++] = var6_6;
            }
            ++var8_9;
lbl74:
            // 2 sources

        } while (var8_9 < var2_2);
    }
}

