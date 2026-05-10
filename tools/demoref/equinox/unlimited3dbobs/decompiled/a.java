/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Graphics;

public class a {
    public int ax;
    public int aw;
    public int av;
    public float au;
    public float at;
    public float as;
    public float ar;
    public float aq;
    public float ap;
    public float ao;
    public int an;
    public int am;
    public int[] al = new int[8];
    public int[] ak = new int[8];
    public int[] aj = new int[8];
    public int[] ai = new int[8];
    public int[] ah = new int[8];
    public int[] ag = new int[8];
    public int[] af;
    public int[] ae;
    public int[] ad;
    public int[] ac;
    public int[] ab;
    public int[] aa;
    public int z;
    public int v;
    public int u;
    public int t;
    public float s;
    public float r;
    public float q;
    public float p;
    public float o;
    public float n;
    public float m;
    public float l;
    public float k;
    public float j;
    public float i;
    public float g;
    public float f;
    public int e;
    public int d;
    public double[][] c;
    public int b;
    public static final int[][] a;

    public final void h() {
        this.d("\u00cbe\u00e2\u00dc\u00c2\u00ea\u00fb\u00db\u00d4\u00cf\u00ea\u00fb");
        this.au = 0.0f;
        this.at = 0.0f;
        this.as = 0.0f;
        this.ar = 0.0f;
        this.aq = 0.0f;
        this.ap = 0.0f;
        this.ao = 0.0f;
        int n = 0;
        while (n < 3) {
            this.e = 2;
            int n2 = 0;
            while (n2 < 3) {
                int n3 = 0;
                while (n3 < 3) {
                    this.c[n][n2 + n3] = n + n2 - n3;
                    ++this.e;
                    ++n3;
                }
                ++n2;
            }
            ++n;
        }
        this.s = 5.0f;
        this.r = 200.0f;
        this.q = 0.0125f;
        this.p = 0.15f;
    }

    public final void g(Graphics graphics, int n, int n2) {
        int n3;
        this.e();
        this.f(this.al, this.ak, this.aj);
        this.aq = (float)((double)this.aq + 0.04);
        this.ap = (float)((double)this.ap + 0.06);
        this.ao = (float)((double)this.ao + 0.03);
        this.s += this.q;
        this.r += this.p;
        if (this.s < 3.0f) {
            this.q = -this.q;
        } else if (this.s > 175.0f) {
            this.q = -this.q;
        }
        if (this.r < 3.0f) {
            this.p = -this.p;
        }
        if (this.r > 250.0f) {
            this.p = -this.p;
        }
        switch (n2) {
            case 1: {
                this.t = 0;
                break;
            }
            case 2: {
                this.t = 5;
                this.c[0][0] = 0.0;
                break;
            }
        }
        switch (n) {
            case 0: {
                this.aw = (int)((double)(UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2 - 8 - this.t) * (Math.cos(this.au) * Math.cos(this.au / 80.0f)));
                this.aw += UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2;
                this.au = (float)((double)this.au + 0.02);
                ++this.d;
                this.av = (int)((double)(UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2 - 8 - this.t) * (Math.sin((double)this.au * 1.02) * Math.cos(this.au / 60.0f)));
                this.av += UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2;
                ++this.d;
                this.at = (float)((double)this.at + 0.02);
                break;
            }
            case 1: {
                this.c[1][1] = 0.0;
                this.aw = (int)((double)(UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2 - 8 - this.t) * (Math.cos((double)(this.au * 3.0f) + 1.57) * Math.cos(this.au / 50.0f)));
                this.e = 2;
                this.aw += UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2;
                this.au = (float)((double)this.au - 0.015);
                this.av = (int)((double)(UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2 - 8 - this.t) * (Math.sin((double)this.au * 1.01) * Math.cos(this.au / 80.0f)));
                this.av += UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2;
                float cfr_ignored_0 = (float)(Math.sin(this.aw) * Math.cos(this.av) * (double)UnlimitedBobs.a3 / 20.0);
                this.at = (float)((double)this.at - 0.02);
                break;
            }
            case 2: {
                this.aw = (int)((double)(UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2 - 8 - this.t) * (Math.cos(this.au) * Math.cos(this.au / 80.0f) + Math.sin(this.as / 5.0f)) / 2.0);
                this.aw += UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2;
                this.au = (float)((double)this.au + 0.04);
                float cfr_ignored_1 = (float)(Math.exp(this.aw) * Math.cos(this.av) * (double)UnlimitedBobs.a3 / 20.0);
                this.as = (float)((double)this.as + 0.003);
                this.av = (int)((double)(UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2 - 8 - this.t) * (Math.sin((double)this.au * 1.02) * Math.cos(this.au / 60.0f) + Math.cos(this.ar / 5.0f)) / 2.0);
                this.av += UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2;
                this.at = (float)((double)this.at + 0.038);
                this.ar = (float)((double)this.ar + 0.00305);
                break;
            }
            case 3: {
                this.aw = (int)((double)(UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2 - 8 - this.t) * (Math.cos(Math.cos(this.au)) * Math.cos(this.au / 80.0f)));
                this.aw += UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2;
                this.au = (float)((double)this.au + 0.02);
                this.av = (int)((double)(UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2 - 8 - this.t) * (Math.sin((double)this.au * 1.02) * Math.cos(this.au / 60.0f)));
                float cfr_ignored_2 = (float)Math.exp(this.aw) * (float)UnlimitedBobs.a3 / 10.0f;
                this.av += UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2;
                this.at = (float)((double)this.at + 0.02);
                break;
            }
            case 4: {
                this.aw = (int)((double)(UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2 - 8 - this.t) * (Math.cos((double)this.au * 0.99) * Math.sin(this.au / 80.0f + this.at) + Math.sin(this.as / 5.0f)) / 2.0);
                this.aw += UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2;
                this.au = (float)((double)this.au + 0.04);
                float cfr_ignored_3 = (float)Math.exp(this.av) * (float)UnlimitedBobs.a3 / 10.0f;
                this.as = (float)((double)this.as + 0.003);
                this.av = (int)((double)(UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2 - 8 - this.t) * (Math.sin((double)this.au * 0.98) * Math.cos(this.au / 60.0f) + Math.cos(this.ar / 5.0f)) / 2.0);
                float cfr_ignored_4 = (float)Math.cos(this.aw) * (float)UnlimitedBobs.a3;
                this.av += UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2;
                this.at = (float)((double)this.at + 0.038);
                this.ar = (float)((double)this.ar + 0.00305);
                break;
            }
            case 5: {
                this.aw = (int)((double)(UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2 - 8 - this.t) * (Math.cos(this.au) * Math.cos(this.au / 80.0f)));
                this.aw += UnlimitedBobs.a3 / 2 - UnlimitedBobs.a5 / 2;
                this.au = (float)((double)this.au + 0.0095);
                float cfr_ignored_5 = (float)Math.cos(this.aw) * (float)UnlimitedBobs.a3;
                this.av = (int)((double)(UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2 - 8 - this.t) * (Math.cos(Math.sin((double)this.au * 1.02) * 2.0 + (double)this.at) * Math.cos(this.au / 60.0f)));
                float cfr_ignored_6 = (float)Math.cos(this.av) * (float)UnlimitedBobs.a3;
                this.av += UnlimitedBobs.a2 / 2 - UnlimitedBobs.a4 / 2;
                this.at = (float)((double)this.at + 0.01);
                break;
            }
        }
        if (n2 - 1 != 0) {
            n3 = 0;
            while (n3 < this.am) {
                this.c(n3);
                this.a("\u00dc\u00cb\u00e9\u00e0\u00f6\u00fc\u00db");
                if (this.ax == 1) {
                    this.c[1][2] = 2.0;
                    graphics.setColor(new Color(50, 50, this.z));
                    graphics.fillPolygon(this.ae, this.ad, 3);
                }
                ++n3;
            }
        }
        switch (n2) {
            case 1: {
                graphics.drawImage(UnlimitedBobs.a6, this.aw, this.av, UnlimitedBobs.a5, UnlimitedBobs.a4, null);
                return;
            }
            case 2: {
                n3 = 0;
                while (n3 < this.am) {
                    this.c(n3);
                    this.b("\u00dc\u00cf\u00cb\u00c4\u00e9\u00e0\u00f6");
                    if (this.ax == 1) {
                        graphics.setColor(new Color(60, 50, 70));
                        graphics.drawLine(this.ae[0], this.ad[0], this.ae[2], this.ad[2]);
                        this.d = 0;
                    }
                    ++n3;
                }
                return;
            }
        }
    }

    private final void f(int[] nArray, int[] nArray2, int[] nArray3) {
        int n = 0;
        int n2 = 0;
        int n3 = 0;
        int n4 = 0;
        while (n4 < this.an) {
            n = nArray[n4];
            n2 = nArray2[n4];
            n3 = nArray3[n4];
            this.ai[n4] = (int)(this.o * (float)n + this.n * (float)n2 + this.m * (float)n3);
            this.ah[n4] = (int)(this.l * (float)n + this.k * (float)n2 + this.j * (float)n3);
            this.ag[n4] = (int)(this.i * (float)n + this.g * (float)n2 + this.f * (float)n3);
            this.ai[n4] = 300 * this.ai[n4] / (300 - (this.ag[n4] + this.u)) + 8;
            this.ah[n4] = 300 * this.ah[n4] / (300 - (this.ag[n4] + this.u)) + 4;
            ++n4;
        }
    }

    private final void e() {
        float f = (float)Math.cos(this.aq);
        float f2 = (float)Math.sin(this.aq);
        float f3 = (float)Math.cos(this.ap);
        float f4 = (float)Math.sin(this.ap);
        float f5 = (float)Math.cos(this.ao);
        float f6 = (float)Math.sin(this.ao);
        this.o = f * f3;
        this.n = -(f * f4 * f6 + f2 * f5);
        this.m = -(f * f4 * f5 - f2 * f6);
        this.l = f2 * f3;
        this.k = -(f2 * f4 * f6 - f * f5);
        this.j = -(f2 * f4 * f5 + f * f6);
        this.i = f4;
        this.g = f3 * f6;
        this.f = f3 * f5;
    }

    private final void d(String string) {
        this.an = 8;
        this.am = 12;
        this.v = 100;
        this.u = -3000;
        this.al[0] = this.v;
        this.ak[0] = -this.v;
        this.aj[0] = this.v;
        this.al[1] = this.v;
        this.ak[1] = -this.v;
        this.aj[1] = -this.v;
        this.al[2] = -this.v;
        this.ak[2] = -this.v;
        this.aj[2] = -this.v;
        this.al[3] = -this.v;
        this.ak[3] = -this.v;
        this.aj[3] = this.v;
        this.al[4] = this.v;
        this.ak[4] = this.v;
        this.aj[4] = this.v;
        this.al[5] = this.v;
        this.ak[5] = this.v;
        this.aj[5] = -this.v;
        this.al[6] = -this.v;
        this.ak[6] = this.v;
        this.aj[6] = -this.v;
        this.al[7] = -this.v;
        this.ak[7] = this.v;
        this.aj[7] = this.v;
        this.af[0] = 175;
        this.af[1] = 175;
        this.af[2] = 215;
        this.af[3] = 215;
        this.af[4] = 175;
        this.af[5] = 175;
        this.af[6] = 215;
        this.af[7] = 215;
        this.af[8] = 255;
        this.af[9] = 255;
        this.af[10] = 255;
        this.af[11] = 255;
    }

    private final void c(int n) {
        this.ae[0] = this.aw + this.ai[a[n][0]];
        this.ad[0] = this.av + this.ah[a[n][0]];
        this.ac[0] = this.av + this.ag[a[n][0]];
        this.ae[1] = this.aw + this.ai[a[n][1]];
        this.ad[1] = this.av + this.ah[a[n][1]];
        this.ac[1] = this.av + this.ag[a[n][1]];
        this.ae[2] = this.aw + this.ai[a[n][2]];
        this.ad[2] = this.av + this.ah[a[n][2]];
        this.ac[2] = this.av + this.ag[a[n][2]];
        switch (this.b) {
            case 1: {
                this.ab[0] = this.ae[0] + 1;
                this.aa[0] = this.ad[0] + 1;
                this.ab[1] = this.ae[1] + 1;
                this.aa[1] = this.ad[1] + 1;
                this.ab[2] = this.ae[2] + 1;
                this.aa[2] = this.ad[2] + 1;
                this.c[2][2] = -1.0;
                return;
            }
            case 2: {
                this.ab[0] = this.ae[0];
                this.aa[0] = this.ad[0];
                this.ab[1] = this.ae[1];
                this.aa[1] = this.ad[1];
                this.ab[2] = this.ae[2];
                this.aa[2] = this.ad[2];
                this.c[2][2] = 1.0;
                return;
            }
        }
    }

    private final void b(String string) {
        int n = this.ae[1] - this.ae[0];
        int n2 = this.ad[2] - this.ad[1];
        int n3 = this.ae[2] - this.ae[1];
        int n4 = this.ad[1] - this.ad[0];
        if (n * n2 - n3 * n4 > 0) {
            this.ax = 1;
            return;
        }
        this.ax = 0;
    }

    private final void a(String string) {
        float f = this.ae[1] - this.ae[0];
        float f2 = this.ae[2] - this.ae[1];
        float f3 = this.ad[1] - this.ad[0];
        float f4 = this.ad[2] - this.ad[1];
        float f5 = this.ac[1] - this.ac[0];
        float f6 = this.ac[2] - this.ac[1];
        float f7 = f3 * f6 - f4 * f5;
        float f8 = f5 * f2 - f6 * f;
        float f9 = f * f4 - f2 * f3;
        float f10 = 2.0f;
        float f11 = f9 / (float)Math.sqrt(f7 * f7 + f8 * f8 + f9 * f9);
        f10 = 3.0f;
        this.z = (int)(Math.sqrt(Math.sqrt(f11)) * 255.0);
        if (f9 > 0.0f) {
            this.ax = 1;
            return;
        }
        this.ax = 0;
    }

    public a() {
        int[] cfr_ignored_0 = new int[8];
        int[] cfr_ignored_1 = new int[8];
        this.af = new int[12];
        this.ae = new int[3];
        this.ad = new int[3];
        this.ac = new int[3];
        this.ab = new int[3];
        this.aa = new int[3];
        int[] cfr_ignored_2 = new int[20];
        this.c = new double[4][10];
        this.b = 1;
    }

    static {
        int[][] nArrayArray = new int[12][];
        int[] nArray = new int[3];
        nArray[1] = 1;
        nArray[2] = 5;
        nArrayArray[0] = nArray;
        int[] nArray2 = new int[3];
        nArray2[1] = 5;
        nArray2[2] = 4;
        nArrayArray[1] = nArray2;
        nArrayArray[2] = new int[]{1, 2, 6};
        nArrayArray[3] = new int[]{1, 6, 5};
        nArrayArray[4] = new int[]{2, 3, 7};
        nArrayArray[5] = new int[]{2, 7, 6};
        int[] nArray3 = new int[3];
        nArray3[0] = 3;
        nArray3[2] = 4;
        nArrayArray[6] = nArray3;
        nArrayArray[7] = new int[]{3, 4, 7};
        nArrayArray[8] = new int[]{7, 4, 5};
        nArrayArray[9] = new int[]{7, 5, 6};
        int[] nArray4 = new int[3];
        nArray4[0] = 2;
        nArray4[1] = 1;
        nArrayArray[10] = nArray4;
        int[] nArray5 = new int[3];
        nArray5[0] = 2;
        nArray5[2] = 3;
        nArrayArray[11] = nArray5;
        a = nArrayArray;
    }
}

