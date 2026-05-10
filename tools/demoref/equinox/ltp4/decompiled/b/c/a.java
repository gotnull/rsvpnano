/*
 * Decompiled with CFR 0.152.
 */
package b.c;

import b.c.b;

public class a {
    private int a;
    private String b;

    /*
     * Unable to fully structure code
     */
    public void a(byte[] var1_1, byte[] var2_2, byte[] var3_3, int var4_4, int var5_5, int var6_6, int var7_7, int var8_8, int var9_9, int var10_10, int var11_11, int var12_12, int var13_13, int var14_14, int var15_15) {
        var17_16 = b.c.b.b;
        this.b = b.c.a.a("Qx\u0007T\u0013{qRO\bxl\u0001\u001c");
        var16_17 = var4_4;
        if (!var17_16) ** GOTO lbl14
        var18_18 = b.e.a.m;
        b.e.a.m = ++var18_18;
        block0: while (true) {
            var1_1[var16_17] = (byte)Math.min((double)((float)var6_6 + (float)var9_9 * this.a((double)(var16_17 - var4_4) * 3.141592653589793 / 2.0 / (double)(var5_5 - var4_4 + 1))) + (double)var12_12 * Math.pow(this.a((double)(var16_17 - var4_4) * 3.141592653589793 / 2.0 / (double)(var5_5 - var4_4 + 1)), var15_15), 255.0);
            var2_2[var16_17] = (byte)Math.min((double)((float)var7_7 + (float)var10_10 * this.a((double)(var16_17 - var4_4) * 3.141592653589793 / 2.0 / (double)(var5_5 - var4_4 + 1))) + (double)var13_13 * Math.pow(this.a((double)(var16_17 - var4_4) * 3.141592653589793 / 2.0 / (double)(var5_5 - var4_4 + 1)), var15_15), 255.0);
            var3_3[var16_17] = (byte)Math.min((double)((float)var8_8 + (float)var11_11 * this.a((double)(var16_17 - var4_4) * 3.141592653589793 / 2.0 / (double)(var5_5 - var4_4 + 1))) + (double)var14_14 * Math.pow(this.a((double)(var16_17 - var4_4) * 3.141592653589793 / 2.0 / (double)(var5_5 - var4_4 + 1)), var15_15), 255.0);
            this.b = b.c.a.a("Z\\>q");
            do {
                ++var16_17;
lbl14:
                // 2 sources

                if (var16_17 < var5_5 + 1) continue block0;
                this.a = 1;
            } while (var17_16);
            break;
        }
    }

    public float a(double d2) {
        this.a = 1;
        return (float)Math.sin(d2);
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
                        n5 = 20;
                        break;
                    }
                    case 1: {
                        n5 = 9;
                        break;
                    }
                    case 2: {
                        n5 = 114;
                        break;
                    }
                    case 3: {
                        n5 = 61;
                        break;
                    }
                    default: {
                        n5 = 125;
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

