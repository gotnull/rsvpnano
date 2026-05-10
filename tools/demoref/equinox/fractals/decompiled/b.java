/*
 * Decompiled with CFR 0.152.
 */
public class b {
    public int a1 = 0;
    public float[] a0;
    public float[] a_;
    public float[] az;
    public float[] ay;
    public float[] ax;
    public float[] aw;
    public float av;
    public float au;
    public float at;
    public float as;
    public float ar;
    public float aq;
    public float ap;
    public float ao;
    public float an;
    public float am;
    public float al;
    public int ak;
    public int aj;
    public int ai;
    public byte[] ah;
    public int ag = 0;
    public int af;
    public int ae;
    public int ad = 1;
    public int ac;
    public boolean ab;
    public static boolean aa;

    public final void u(int n) {
        this.ad = n;
    }

    public final void u(int n, int n2) {
        this.ak = n;
        this.aj = n2;
    }

    public final void u(float f, float f2, float f3, float f4, float f5, int n) {
        this.av = f;
        this.au = f2;
        this.at = f3;
        this.as = f4;
        this.ar = f5;
        this.ai = n;
    }

    public final void u(float f, float f2, float f3, float f4, float f5, float f6) {
        this.aq = f;
        this.ap = f2;
        this.ao = f3;
        this.an = f4;
        this.am = f5;
        this.al = f6;
    }

    public final void t(int n) {
        if (n > this.af) {
            n = this.af;
        }
        if (n < 50) {
            n = 50;
        }
        this.ae = n;
        this.ac = 0;
    }

    /*
     * Unable to fully structure code
     */
    public final void u(byte var1_1, byte var2_2, byte var3_3, byte var4_4, byte var5_5, byte var6_6) {
        var9_7 = a.a2;
        this.a_ = null;
        this.az = null;
        this.a0 = null;
        this.ay = null;
        this.ax = null;
        this.aw = null;
        System.gc();
        this.ag = 0;
        this.a1 = 0;
        this.ae = 0;
        this.ac = 0;
        this.ab = false;
        this.a_ = new float[0];
        this.az = new float[0];
        this.a0 = new float[0];
        this.ay = new float[0];
        this.ax = new float[0];
        this.aw = new float[0];
        var7_8 = 0;
        if (var9_7 == 0) ** GOTO lbl40
        do {
            var8_9 = 0;
            if (var9_7 == 0) ** GOTO lbl37
            block1: while (true) {
                block4: {
                    if (var7_8 % 2 != 0) break block4;
                    this.ah[var7_8 * this.af * 3 + var8_9 * 3] = var1_1;
                    this.ah[var7_8 * this.af * 3 + var8_9 * 3 + 1] = var2_2;
                    this.ah[var7_8 * this.af * 3 + var8_9 * 3 + 2] = var3_3;
                    if (var9_7 == 0) ** GOTO lbl36
                }
                this.ah[var7_8 * this.af * 3 + var8_9 * 3] = var4_4;
                this.ah[var7_8 * this.af * 3 + var8_9 * 3 + 1] = var5_5;
                do {
                    this.ah[var7_8 * this.af * 3 + var8_9 * 3 + 2] = var6_6;
lbl36:
                    // 2 sources

                    ++var8_9;
lbl37:
                    // 2 sources

                    if (var8_9 < this.af) continue block1;
                } while (var9_7 != 0);
                break;
            }
            ++var7_8;
lbl40:
            // 2 sources

        } while (var7_8 < this.af);
        if (b.aa) {
            a.a2 = ++var9_7;
        }
    }

    public final void t(float f, float f2, float f3, float f4, float f5, float f6) {
        int n = a.a2;
        float f7 = (float)Math.sqrt(f * f + f2 * f2 + f3 * f3);
        float[] fArray = new float[this.a1];
        System.arraycopy(this.a_, 0, fArray, 0, this.a1);
        this.a_ = null;
        this.a_ = new float[this.a1 + 1];
        System.arraycopy(fArray, 0, this.a_, 0, this.a1);
        this.a_[this.a1] = f / f7;
        System.arraycopy(this.az, 0, fArray, 0, this.a1);
        this.az = null;
        this.az = new float[this.a1 + 1];
        System.arraycopy(fArray, 0, this.az, 0, this.a1);
        this.az[this.a1] = f2 / f7;
        System.arraycopy(this.a0, 0, fArray, 0, this.a1);
        this.a0 = null;
        this.a0 = new float[this.a1 + 1];
        System.arraycopy(fArray, 0, this.a0, 0, this.a1);
        this.a0[this.a1] = f3 / f7;
        System.arraycopy(this.ay, 0, fArray, 0, this.a1);
        this.ay = null;
        this.ay = new float[this.a1 + 1];
        System.arraycopy(fArray, 0, this.ay, 0, this.a1);
        this.ay[this.a1] = f4;
        System.arraycopy(this.ax, 0, fArray, 0, this.a1);
        this.ax = null;
        this.ax = new float[this.a1 + 1];
        System.arraycopy(fArray, 0, this.ax, 0, this.a1);
        this.ax[this.a1] = f5;
        System.arraycopy(this.aw, 0, fArray, 0, this.a1);
        this.aw = null;
        this.aw = new float[this.a1 + 1];
        System.arraycopy(fArray, 0, this.aw, 0, this.a1);
        this.aw[this.a1] = f6;
        ++this.a1;
        fArray = null;
        if (n != 0) {
            aa = !aa;
        }
    }

    public b(int n) {
        this.af = n;
        this.a_ = new float[0];
        this.az = new float[0];
        this.a0 = new float[0];
        this.ay = new float[0];
        this.ax = new float[0];
        this.aw = new float[0];
        this.ah = new byte[n * n * 3];
        this.ab = false;
    }

    public b() {
        this.a_ = new float[0];
        this.az = new float[0];
        this.a0 = new float[0];
        this.ay = new float[0];
        this.ax = new float[0];
        this.aw = new float[0];
        this.ab = false;
    }
}

