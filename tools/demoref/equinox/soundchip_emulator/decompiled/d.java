/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;

/*
 * Illegal identifiers - consider using --renameillegalidents true
 */
public class d {
    public int[] el;
    public int[] ek;
    public int[] ej;
    public int[] ei;
    public int[] eh;
    public int[] eg;
    public int[] ef;
    public int[] ee;
    public int[] ed;
    public int[] ec;
    public int[] eb;
    public int[] ea;
    public int[] d9;
    public int[] d8;
    public int[] d7;
    public int[] d6;
    public int[] d5;
    public int[] d4;
    public int[] d3;
    public int[] d2;
    public int[] d1;
    public int[] d0;
    public int[] d_;
    public int[] dz;
    public int[] dy;
    public int[] dx;
    public int[] dw;
    public int[] dv;
    public int[] du;
    public int[] dt;
    public int[] ds;
    public int[] dr;
    public int[] dq;
    public int[] dp;
    public int do;
    public int[] dn;
    public float[] dm;
    public int dl;
    public float dk;
    public int dj;
    public float di;
    public float dh;
    public float dg;
    public float df;
    public int de;
    public float dd;
    public int dc;
    public int db;
    public int da;
    public int c9;
    public int c8;
    public int c7;
    public int c6;
    public int c5;
    public int c4;
    public static int c0;
    public c c_;
    public int cz;
    public double[][] cy = new double[4][10];
    public float cx;
    public float cw;
    public float cv;
    public float cu;
    public float ct;
    public float cs;
    public float cr;
    public float cq;
    public float cp;

    public final void f(int n, int n2, int n3) {
        c0 = n;
        this.d0 = new int[c0];
        this.d_ = new int[c0];
        this.dz = new int[c0];
        this.el = new int[c0];
        this.ek = new int[c0];
        this.ej = new int[c0];
        this.ei = new int[c0];
        this.eh = new int[c0];
        this.eg = new int[c0];
        this.ef = new int[c0];
        this.ee = new int[c0];
        this.ed = new int[c0];
        this.ec = new int[c0];
        this.eb = new int[c0];
        this.ea = new int[c0];
        this.d9 = new int[c0];
        this.d8 = new int[c0];
        this.d7 = new int[c0];
        this.d6 = new int[c0];
        this.d5 = new int[c0];
        this.d4 = new int[c0];
        this.d3 = new int[c0];
        this.d2 = new int[c0];
        this.d1 = new int[c0];
        this.dy = new int[c0];
        this.dx = new int[c0];
        this.dw = new int[c0];
        this.dv = new int[c0];
        this.du = new int[c0];
        this.dt = new int[c0];
        this.ds = new int[c0];
        this.dr = new int[c0];
        this.cy[2][7] = 0.0;
        int[] cfr_ignored_0 = new int[1400];
        int[] cfr_ignored_1 = new int[c0 + 10];
        this.y();
        this.x();
        this.aa();
        this.dk = 0.0f;
        this.df = 0.0f;
        this.dh = 0.0f;
        this.de = 1272;
    }

    public final void ah(Graphics graphics, int n, int n2, int n3, Image image, Image image2, Image image3, Image image4, Image image5) {
        if (n3 == 1) {
            this.ag();
            this.af(graphics, n, n2, n3);
            return;
        }
        if (n3 == 2) {
            this.c_.v(graphics, image2, image3, image4, image5);
            return;
        }
        if (n3 == 3) {
            this.ag();
            this.ae(graphics, n, n2, n3);
            return;
        }
        if (n3 == 4) {
            this.c_.w(graphics, image);
        }
    }

    /*
     * Unable to fully structure code
     */
    private final void ag() {
        block27: {
            block26: {
                ++this.dl;
                if (this.dl < 108) {
                    this.de -= 12;
                }
                if (this.dl > this.dp[12] - 108) {
                    this.de += 12;
                }
                this.dj = -1100;
                this.z();
                this.ab();
                if (this.dl >= this.dp[0] + 1) break block26;
                this.ad(this.el, this.ek, this.ej);
                break block27;
            }
            if (this.dl <= this.dp[0]) ** GOTO lbl-1000
            if (this.dl < this.dp[1] + 1) {
                this.ac(this.el, this.ek, this.ej, this.ei, this.eh, this.eg, 0);
            } else if (this.dl > this.dp[1] && this.dl < this.dp[2] + 1) {
                this.ad(this.ei, this.eh, this.eg);
            } else if (this.dl > this.dp[2] && this.dl < this.dp[3] + 1) {
                this.ac(this.ei, this.eh, this.eg, this.ef, this.ee, this.ed, 1);
            } else if (this.dl > this.dp[3] && this.dl < this.dp[4] + 1) {
                this.ad(this.ef, this.ee, this.ed);
            } else if (this.dl > this.dp[4] && this.dl < this.dp[5] + 1) {
                this.ac(this.ef, this.ee, this.ed, this.ec, this.eb, this.ea, 2);
            } else if (this.dl > this.dp[5] && this.dl < this.dp[6] + 1) {
                this.ad(this.ec, this.eb, this.ea);
            } else if (this.dl > this.dp[6] && this.dl < this.dp[7] + 1) {
                this.ac(this.ec, this.eb, this.ea, this.d9, this.d8, this.d7, 3);
            } else if (this.dl > this.dp[7] && this.dl < this.dp[8] + 1) {
                this.ad(this.d9, this.d8, this.d7);
            } else if (this.dl > this.dp[8] && this.dl < this.dp[9] + 1) {
                this.ac(this.d9, this.d8, this.d7, this.d6, this.d5, this.d4, 4);
            } else if (this.dl > this.dp[9] && this.dl < this.dp[10] + 1) {
                this.ad(this.d6, this.d5, this.d4);
            } else if (this.dl > this.dp[10] && this.dl < this.dp[11] + 1) {
                this.ac(this.d6, this.d5, this.d4, this.d3, this.d2, this.d1, 5);
            } else if (this.dl > this.dp[11] && this.dl < this.dp[12] + 1) {
                this.ad(this.d3, this.d2, this.d1);
            }
        }
        if (this.dl > this.dp[12]) {
            this.dl = 0;
            this.de = 1272;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void af(Graphics graphics, int n, int n2, int n3) {
        int n4 = (int)((double)this.dj - (double)this.cz * 1.5);
        int n5 = (int)((double)this.dj + (double)this.cz * 1.5);
        int n6 = (n5 - n4) / 6;
        int n7 = n / 2 + 40;
        int n8 = n2 / 2;
        int n9 = 0;
        while (n9 < c0) {
            int n10 = this.dz[n9];
            float f2 = (float)(300.0 / (300.0 - (double)n10));
            int n11 = (int)((float)this.d0[n9] * f2 + (float)n7);
            int n12 = (int)((float)this.d_[n9] * f2 + (float)n8);
            int n13 = n10 < n4 + n6 ? 115 : (n10 < n4 + n6 * 2 ? 140 : (n10 < n4 + n6 * 3 ? 175 : (n10 < n4 + n6 * 4 ? 190 : (n10 < n4 + n6 * 5 ? 215 : 240))));
            if (this.dl > 6) {
                graphics.setColor(new Color(n13 - 60, n13 - 30, n13 + 15));
                graphics.drawLine(this.ds[n9], this.dr[n9], n11, n12);
            }
            this.ds[n9] = this.du[n9];
            this.dr[n9] = this.dt[n9];
            this.du[n9] = this.dw[n9];
            this.dt[n9] = this.dv[n9];
            this.dw[n9] = this.dy[n9];
            this.dv[n9] = this.dx[n9];
            this.dy[n9] = n11;
            this.dx[n9] = n12;
            ++n9;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void ae(Graphics graphics, int n, int n2, int n3) {
        int n4 = (int)((double)this.dj - (double)this.cz * 1.5);
        int n5 = (int)((double)this.dj + (double)this.cz * 1.5);
        int n6 = (n5 - n4) / 6;
        int n7 = n / 2 + 40;
        int n8 = n2 / 2;
        int n9 = 0;
        while (n9 < c0) {
            int n10 = this.dz[n9];
            float f2 = (float)(300.0 / (300.0 - (double)n10));
            int n11 = (int)((float)this.d0[n9] * f2 + (float)n7);
            int n12 = (int)((float)this.d_[n9] * f2 + (float)n8);
            int n13 = n10 < n4 + n6 ? 115 : (n10 < n4 + n6 * 2 ? 140 : (n10 < n4 + n6 * 3 ? 175 : (n10 < n4 + n6 * 4 ? 190 : (n10 < n4 + n6 * 5 ? 215 : 240))));
            graphics.setColor(new Color(n13 - 60, n13 - 70, n13 + 15));
            graphics.drawLine(n11, n12, n11 + (22 - n9 % 43) / 2, n12 + (n9 % 40 - 20) / 2);
            ++n9;
        }
    }

    private final void ad(int[] nArray, int[] nArray2, int[] nArray3) {
        int n = 0;
        while (n < c0) {
            int n2 = nArray[n];
            int n3 = nArray2[n];
            int n4 = nArray3[n];
            this.d0[n] = (int)(this.cx * (float)n2 + this.cw * (float)n3 + this.cv * (float)n4);
            this.d_[n] = (int)(this.cu * (float)n2 + this.ct * (float)n3 + this.cs * (float)n4 + (float)this.de);
            this.dz[n] = (int)(this.cr * (float)n2 + this.cq * (float)n3 + this.cp * (float)n4 + (float)this.dj);
            ++n;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void ac(int[] nArray, int[] nArray2, int[] nArray3, int[] nArray4, int[] nArray5, int[] nArray6, int n) {
        n = this.dl - this.dp[n * 2];
        float f2 = (float)((double)n / 50.0);
        int n2 = 0;
        while (n2 < 5) {
            this.cy[1][n2] = Math.cos(0.02f * (float)n2);
            ++n2;
        }
        int n3 = 0;
        while (n3 < c0) {
            this.c6 = (int)((float)(nArray4[n3] - nArray[n3]) * f2);
            this.c5 = (int)((float)(nArray5[n3] - nArray2[n3]) * f2);
            this.c4 = (int)((float)(nArray6[n3] - nArray3[n3]) * f2);
            this.c9 = nArray[n3] + this.c6;
            this.c8 = nArray2[n3] + this.c5;
            this.c7 = nArray3[n3] + this.c4;
            this.d0[n3] = (int)(this.cx * (float)this.c9 + this.cw * (float)this.c8 + this.cv * (float)this.c7);
            this.d_[n3] = (int)(this.cu * (float)this.c9 + this.ct * (float)this.c8 + this.cs * (float)this.c7 + (float)this.de);
            this.dz[n3] = (int)(this.cr * (float)this.c9 + this.cq * (float)this.c8 + this.cp * (float)this.c7 + (float)this.dj);
            ++n3;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void ab() {
        float f2 = (float)Math.cos(this.dh);
        float f3 = (float)Math.sin(this.dh);
        float f4 = (float)Math.cos(this.df);
        float f5 = (float)Math.sin(this.df);
        float f6 = (float)Math.cos(this.dk);
        float f7 = (float)Math.sin(this.dk);
        this.cx = f2 * f4;
        this.cw = -(f2 * f5 * f7 + f3 * f6);
        this.cv = -(f2 * f5 * f6 - f3 * f7);
        this.cu = f3 * f4;
        this.ct = -(f3 * f5 * f7 - f2 * f6);
        this.cs = -(f3 * f5 * f6 + f2 * f7);
        this.cr = f5;
        int n = 0;
        while (n < 5) {
            this.cy[1][n] = Math.log(0.02f * (float)n);
            ++n;
        }
        this.cq = f4 * f7;
        this.cp = f4 * f6;
    }

    private final void aa() {
        this.da = 8;
        this.dm = new float[this.da * 3];
        this.dn = new int[this.da + 1];
        this.dn[0] = 150;
        this.dm[0] = 0.416667f;
        this.dm[1] = 0.833333f;
        this.dm[2] = 0.0f;
        this.dn[1] = 100;
        this.dm[3] = 0.833333f;
        this.dm[4] = 0.833333f;
        this.dm[5] = 0.833333f;
        this.dn[2] = 100;
        this.dm[6] = 0.833333f;
        this.dm[7] = 1.25f;
        this.dm[8] = 0.833333f;
        this.dn[3] = 100;
        this.dm[9] = 0.833333f;
        this.dm[10] = 1.666667f;
        this.dm[11] = 0.833333f;
        this.dn[4] = 50;
        this.dm[12] = 0.833333f;
        this.dm[13] = 1.666667f;
        this.dm[14] = 1.666667f;
        this.dn[5] = 50;
        this.dm[15] = 0.833333f;
        this.dm[16] = 1.25f;
        this.dm[17] = 2.083333f;
        this.dn[6] = 150;
        this.dm[18] = 0.833333f;
        this.dm[19] = -0.833333f;
        this.dm[20] = 1.666667f;
        this.dn[7] = 100;
        this.dm[21] = 0.833333f;
        this.dm[22] = -0.416667f;
        this.dm[23] = 1.25f;
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void z() {
        this.dg = this.dm[3 * this.db];
        this.dd = this.dm[1 + 3 * this.db];
        this.di = this.dm[2 + 3 * this.db];
        this.dh += 0.017453f * this.dg;
        this.df += 0.017453f * this.dd;
        this.dk += 0.017453f * this.di;
        ++this.dc;
        if (this.dc >= this.dn[this.db]) {
            this.dc = 0;
            ++this.db;
            if (this.db >= this.da) {
                this.db = 0;
                int n = 0;
                while (n < 10) {
                    this.cy[2][n] = Math.exp((double)n * (double)0.03f);
                    ++n;
                }
            }
        }
    }

    private final void y() {
        int n = 0;
        this.do = 7;
        this.dq = new int[this.do * 2];
        this.dp = new int[this.do * 2];
        this.dq[0] = 200;
        this.dq[1] = 50;
        this.dq[2] = 100;
        this.dq[3] = 50;
        this.dq[4] = 300;
        this.dq[5] = 50;
        this.dq[6] = 200;
        this.dq[7] = 50;
        this.dq[8] = 200;
        this.dq[9] = 50;
        this.dq[10] = 200;
        this.dq[11] = 50;
        this.dq[12] = 200;
        int n2 = 0;
        while (n2 < this.do * 2 - 1) {
            this.dp[n2] = n += this.dq[n2];
            int n3 = 0;
            while (n3 < 10) {
                this.cy[2][n3] = Math.cos((double)n3 * (double)0.03f);
                ++n3;
            }
            ++n2;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void x() {
        int n;
        int n2;
        int n3;
        this.cz = 400;
        this.c9 = (int)Math.sqrt(c0);
        int n4 = 0;
        while (n4 < this.c9) {
            n3 = 0;
            while (n3 < this.c9) {
                this.ej[n4 + n3 * this.c9] = (int)(Math.cos((double)6.283f / (double)this.c9 * (double)n4 * 2.0) * Math.sin((double)3.14159f / (double)this.c9 * (double)n3 * 2.0) * (double)this.cz / 5.0);
                this.el[n4 + n3 * this.c9] = (int)((double)(this.cz * n4 / this.c9) * (double)2.2f - (double)this.cz * (double)1.1f);
                this.ek[n4 + n3 * this.c9] = (int)((double)(this.cz * n3 / this.c9) * (double)2.2f - (double)this.cz * (double)1.1f);
                ++n3;
            }
            ++n4;
        }
        this.c9 = (int)Math.sqrt(c0);
        n3 = 0;
        while (n3 < this.c9) {
            n2 = 0;
            while (n2 < this.c9) {
                this.eg[n3 + n2 * this.c9] = (n3 - this.c9 / 2) * (n3 - this.c9 / 2) * (n2 - this.c9 / 2) * (n2 - this.c9 / 2) * this.cz / (this.c9 * this.c9 * this.c9 * this.c9 / 16) - this.cz / 2;
                this.ei[n3 + n2 * this.c9] = this.cz * n3 / this.c9 * 2 - this.cz;
                this.eh[n3 + n2 * this.c9] = this.cz * n2 / this.c9 * 2 - this.cz;
                ++n2;
            }
            ++n3;
        }
        this.c9 = (int)Math.sqrt(c0);
        Math.sqrt(c0 * 2);
        n2 = 0;
        while (n2 < this.c9) {
            n = 0;
            while (n < this.c9) {
                this.ed[n2 + n * this.c9] = (int)((double)(-(n2 - this.c9 / 2) * (n2 - this.c9 / 2) * (n - this.c9 / 2) * (n - this.c9 / 2) * this.cz / (this.c9 * this.c9 * this.c9 * this.c9 / 16 * 2)) + Math.cos((double)1.2566f * (double)(n2 * n)) * (double)this.c9 * (double)this.c9 / 2.0 + (double)this.cz / 2.5);
                this.ef[n2 + n * this.c9] = this.cz * n2 / this.c9 * 2 - this.cz;
                this.ee[n2 + n * this.c9] = this.cz * n / this.c9 * 2 - this.cz;
                ++n;
            }
            ++n2;
        }
        n = 0;
        while (n < c0) {
            this.ea[n] = (int)(Math.cos((double)0.069811f * (double)n) * Math.sin((double)0.10472f * (double)n) * (double)this.cz * (double)0.9f);
            this.ec[n] = (int)(Math.sin((double)0.089757f * (double)n) * Math.sin((double)0.062832f * (double)n) * (double)this.cz * (double)0.9f);
            this.eb[n] = (int)(Math.sin((double)0.125664f * (double)n) * (double)this.cz);
            ++n;
        }
        this.c9 = (int)Math.sqrt(c0);
        int n5 = 0;
        while (n5 < c0) {
            this.d9[n5] = (int)(Math.cos((double)0.6283f * (double)n5) * Math.sin((double)6.28318f / (double)c0 * (double)n5) * (double)this.cz);
            this.d7[n5] = (int)(Math.sin((double)0.6283f * (double)n5) * Math.sin((double)6.28318f / (double)c0 * (double)n5) * (double)this.cz);
            this.d8[n5] = c0 * 2 / c0 * n5 - c0;
            ++n5;
        }
        int n6 = 0;
        while (n6 < c0) {
            this.d4[n6] = this.cz * n6 / c0 * 2 - this.cz;
            this.d6[n6] = (int)(Math.cos((double)6.283f / (double)c0 * (double)n6 * 10.0) * (double)this.cz);
            this.d5[n6] = (int)(Math.sin((double)6.283f / (double)c0 * (double)n6 * 10.0) * (double)this.cz);
            ++n6;
        }
        int n7 = 0;
        while (n7 < c0) {
            this.d1[n7] = (int)((double)this.cz * 1.5 * Math.random() - (double)this.cz * 0.75);
            this.d3[n7] = (int)((double)this.cz * 1.5 * Math.random() - (double)this.cz * 0.75);
            this.d2[n7] = (int)((double)this.cz * 1.5 * Math.random() - (double)this.cz * 0.75);
            ++n7;
        }
    }

    public d() {
        this.c_ = new c();
    }
}

