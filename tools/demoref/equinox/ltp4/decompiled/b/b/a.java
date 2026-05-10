/*
 * Decompiled with CFR 0.152.
 */
package b.b;

import java.applet.Applet;

public class a
extends Applet {
    private byte[] a = new byte[78400];
    private byte[] b = new byte[78400];
    private byte[] c = new byte[78400];
    private float[] d = new float[256];
    private int e;
    public static boolean f;

    /*
     * Unable to fully structure code
     */
    public void a(int[] var1_1, int var2_2) {
        var3_3 = b.b.a.f;
        this.e = 0;
        if (!var3_3) ** GOTO lbl10
        var4_4 = b.e.a.m;
        b.e.a.m = ++var4_4;
        block0: while (true) {
            this.d[this.e] = ((float)this.e + 0.0f) / (float)var2_2;
            do {
                ++this.e;
lbl10:
                // 2 sources

                if (this.e < 256) continue block0;
                this.e = 0;
            } while (var3_3);
            break;
        }
        if (!var3_3) ** GOTO lbl19
        do {
            this.a[this.e] = (byte)(var1_1[this.e] >> 16 & 255);
            this.b[this.e] = (byte)(var1_1[this.e] >> 8 & 255);
            this.c[this.e] = (byte)(var1_1[this.e] & 255);
            ++this.e;
lbl19:
            // 2 sources

        } while (this.e < 78400);
    }

    public void b(int[] nArray, int n2) {
        this.e = 0;
        while (this.e < 78400) {
            nArray[this.e] = ((int)(this.d[this.a[this.e] & 0xFF] * (float)n2) & 0xFF) << 16 | ((int)(this.d[this.b[this.e] & 0xFF] * (float)n2) & 0xFF) << 8 | (int)(this.d[this.c[this.e] & 0xFF] * (float)n2) & 0xFF;
            ++this.e;
        }
    }
}

