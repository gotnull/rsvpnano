/*
 * Decompiled with CFR 0.152.
 */
public class a {
    public int[] cn;
    public int[] cm;
    public int[] cl;
    public int[] ck;
    public int[] cj;
    public int[] ci;
    public int[] ch;
    public int[] cg;
    public int[] cf;
    public int[] ce;
    public int[] cd;
    public int[] cc;
    public int[] ca;
    public int[] b9;
    public int[] b8;
    public int[] b7;
    public int[] b6;
    public int[] b5;
    public int[] b4;
    public int[] b3;
    public int[] b2;
    public int[] b1;
    public int[] b0;
    public int[] b_;
    public int[] bz;
    public int[] by;
    public int[] bx;
    public int[] bw;
    public int[] bv;
    public int[] bu;
    public int[] bt;
    public int[] bs;
    public int[] br;
    public float[] bq;
    public float[] bp;
    public float[] bo;
    public int[] bn;
    public int bm;
    public int[] bl;
    public int bk;
    public int[] bj;
    public int[] bi;
    public int[] bh;
    public int[] bg;
    public int[] bf;
    public int be;
    public int[] bd;
    public float[] bc;
    public int ba;
    public float a9;
    public int a8;
    public float a7;
    public float a6;
    public int m;
    public float a5;
    public float a4;
    public int a3;
    public float a2;
    public int a1;
    public int a0;
    public int a_;
    public int az;
    public int ay;
    public int q;
    public int ax;
    public static int aw;
    public int[] av;
    public int au;
    public int n;
    public double[][] at = new double[4][10];
    public int as;
    public float ar;
    public float aq;
    public float ap;
    public float ao;
    public float an;
    public float am;
    public float al;
    public float ak;
    public float aj;

    /*
     * Unable to fully structure code
     */
    public final void y(int var1_1, int var2_2, int var3_3, int var4_4, int var5_5) {
        a.aw = var1_1;
        this.as = var4_4;
        this.bt = new int[a.aw];
        this.bs = new int[a.aw];
        this.br = new int[a.aw];
        this.bq = new float[this.as];
        this.bp = new float[this.as];
        this.bo = new float[this.as];
        this.bn = new int[this.as];
        this.cn = new int[a.aw];
        this.cm = new int[a.aw];
        this.cl = new int[a.aw];
        this.ck = new int[a.aw];
        this.cj = new int[a.aw];
        this.ci = new int[a.aw];
        this.ch = new int[a.aw];
        this.cg = new int[a.aw];
        this.cf = new int[a.aw];
        this.ce = new int[a.aw];
        this.cd = new int[a.aw];
        this.cc = new int[a.aw];
        this.ca = new int[a.aw];
        this.b9 = new int[a.aw];
        this.b8 = new int[a.aw];
        this.b7 = new int[a.aw];
        this.b6 = new int[a.aw];
        this.b5 = new int[a.aw];
        this.b4 = new int[a.aw];
        this.b3 = new int[a.aw];
        this.b2 = new int[a.aw];
        this.b1 = new int[a.aw];
        this.b0 = new int[a.aw];
        this.b_ = new int[a.aw];
        this.bz = new int[a.aw];
        this.by = new int[a.aw];
        this.bx = new int[a.aw];
        this.bw = new int[a.aw];
        this.bv = new int[a.aw];
        this.bu = new int[a.aw];
        this.at[2][7] = 0.0;
        this.bl = new int[2304];
        this.m = 1;
        this.bj = new int[1600];
        this.bi = new int[1600];
        this.bh = new int[a.aw + 20];
        this.i();
        var6_6 = 0;
        if (true) ** GOTO lbl74
        do {
            ++this.n;
            var7_7 = 0;
            while (var7_7 < 10) {
                var8_8 = 0;
                while (var8_8 < 10) {
                    this.bi[var6_6 + var7_7 * 10 + var8_8 * 20] = (byte)(var6_6 - var7_7 + var8_8);
                    ++var8_8;
                }
                ++var7_7;
            }
lbl74:
            // 2 sources

        } while (++var6_6 < 10);
        switch (this.m) {
            case 1: {
                this.q();
                this.g(var5_5);
                this.h();
                this.k(var5_5);
                this.bk = 0;
                while (this.bk < 1600) {
                    this.bi[this.bk] = 0;
                    ++this.bk;
                }
                break;
            }
            case 0: {
                this.g(this.ba);
                this.ba = 1;
                this.ba = 1;
                this.bk = 0;
                while (this.bk < 1600) {
                    this.bi[this.bk] = -2;
                    ++this.bk;
                }
                break;
            }
        }
        this.a9 = 0.0f;
        this.a4 = 0.0f;
        this.a6 = 0.0f;
        this.az = this.bi[0] + 1;
        this.a3 = 891;
        this.ba = 0;
        if (var5_5 != 1) {
            this.n = this.ba + 500;
            this.ba += 500;
        }
    }

    /*
     * Unable to fully structure code
     */
    public final void x() {
        var2_1 = 0;
        if (true) ** GOTO lbl30
        do {
            (byte)var2_1;
            var5_3 = (int)(Math.sin(3.1414999961853027 * ((double)var2_1 + 0.5) / 11.0) * 23.899999618530273);
            var1_2 = 0;
            if (true) ** GOTO lbl18
            do {
                this.n = (int)this.bq[var2_1];
                this.bq[var2_1 * 23 + var1_2] = (int)(Math.cos(6.2829999923706055 * (double)var1_2 / 23.0) * (double)var5_3);
                this.bo[var2_1 * 23 + var1_2] = (int)(Math.sin(6.2829999923706055 * (double)var1_2 / 23.0) * (double)var5_3);
                this.bp[var2_1 * 23 + var1_2] = (int)(Math.cos(3.1414999961853027 * ((double)var2_1 + 0.5) / 11.0) * 23.899999618530273);
                this.bn[var2_1 * 23 + var1_2] = (int)(this.bq[var2_1 * 23 + var1_2] + 24.0f + (this.bp[var2_1 * 23 + var1_2] + 24.0f) * 300.0f);
                this.bm = this.as;
lbl18:
                // 2 sources

            } while (++var1_2 < 23);
            this.bq[255] = 0.0f;
            this.bp[255] = 23.0f;
            this.bo[255] = 0.0f;
            this.bq[254] = 0.0f;
            this.bp[254] = 23.0f;
            this.bo[254] = 0.0f;
            this.bq[253] = 0.0f;
            this.bp[253] = -23.0f;
            this.bo[253] = 0.0f;
            ++var2_1;
lbl30:
            // 2 sources

        } while (var2_1 < 11);
        switch (this.az) {
            case 1: {
                var1_2 = 0;
                while (var1_2 < 23) {
                    var2_1 = 0;
                    while (var2_1 < 180) {
                        this.n = var2_1;
                        var3_4 = (int)(Math.cos(6.283180236816406 * (double)var2_1 / 180.0) * (double)var1_2 + 24.5);
                        var4_6 = (int)(Math.sin(6.283180236816406 * (double)var2_1 / 180.0) * (double)var1_2 + 24.5);
                        this.bl[var3_4 + var4_6 * 48] = 1;
                        ++var2_1;
                    }
                    ++var1_2;
                }
                return;
            }
            case 0: {
                var1_2 = 0;
                while (var1_2 < 21) {
                    var2_1 = 0;
                    while (var2_1 < 180) {
                        this.n = var2_1;
                        var3_5 = (int)(Math.sin(6.283180236816406 * (double)var2_1 / 180.0) * (double)var1_2 + 48.0);
                        var4_7 = (int)(Math.exp(6.283180236816406 * (double)var2_1 / 180.0) * (double)var1_2 + 48.0);
                        this.bl[var3_5 + var4_7 * 48] = 1;
                        ++var2_1;
                    }
                    ++var1_2;
                }
                this.ba = 2;
                return;
            }
        }
    }

    public final void w(int n) {
        ++this.ba;
        this.n = this.ba;
        if (n == 1) {
            double cfr_ignored_0 = n >> 1;
            if (this.ba < 160) {
                this.a3 -= 6;
            }
            if (this.ba > this.bf[18] - 120 - 40) {
                this.a3 += 6;
                byte cfr_ignored_1 = (byte)this.a3;
            }
        } else {
            this.a3 = 0;
        }
        this.a8 = -1100;
        this.j();
        this.l();
        if (this.ba < this.bf[0] + 1) {
            this.o(this.cn, this.cm, this.cl);
        } else if (this.ba > this.bf[0] && this.ba < this.bf[1] + 1) {
            this.m(this.cn, this.cm, this.cl, this.ck, this.cj, this.ci, 0);
        } else if (this.ba > this.bf[1] && this.ba < this.bf[2] + 1) {
            byte cfr_ignored_2 = (byte)this.bf[2];
            this.o(this.ck, this.cj, this.ci);
        } else if (this.ba > this.bf[2] && this.ba < this.bf[3] + 1) {
            byte cfr_ignored_3 = (byte)(this.bf[2] >> 1);
            this.m(this.ck, this.cj, this.ci, this.ch, this.cg, this.cf, 1);
        } else if (this.ba > this.bf[3] && this.ba < this.bf[4] + 1) {
            byte cfr_ignored_4 = (byte)(this.bf[2] >> 2);
            this.o(this.ch, this.cg, this.cf);
        } else if (this.ba > this.bf[4] && this.ba < this.bf[5] + 1) {
            this.m(this.ch, this.cg, this.cf, this.ce, this.cd, this.cc, 2);
        } else if (this.ba > this.bf[5] && this.ba < this.bf[6] + 1) {
            this.o(this.ce, this.cd, this.cc);
        } else if (this.ba > this.bf[6] && this.ba < this.bf[7] + 1) {
            this.m(this.ce, this.cd, this.cc, this.ca, this.b9, this.b8, 3);
        } else if (this.ba > this.bf[7] && this.ba < this.bf[8] + 1) {
            this.o(this.ca, this.b9, this.b8);
        } else if (this.ba > this.bf[8] && this.ba < this.bf[9] + 1) {
            this.m(this.ca, this.b9, this.b8, this.b7, this.b6, this.b5, 4);
        } else if (this.ba > this.bf[9] && this.ba < this.bf[10] + 1) {
            this.o(this.b7, this.b6, this.b5);
        } else if (this.ba > this.bf[10] && this.ba < this.bf[11] + 1) {
            this.m(this.b7, this.b6, this.b5, this.b4, this.b3, this.b2, 5);
        } else if (this.ba > this.bf[11] && this.ba < this.bf[12] + 1) {
            this.o(this.b4, this.b3, this.b2);
        } else if (this.ba > this.bf[12] && this.ba < this.bf[13] + 1) {
            this.m(this.b4, this.b3, this.b2, this.b1, this.b0, this.b_, 6);
        } else if (this.ba > this.bf[13] && this.ba < this.bf[14] + 1) {
            this.o(this.b1, this.b0, this.b_);
        } else if (this.ba > this.bf[14] && this.ba < this.bf[15] + 1) {
            this.m(this.b1, this.b0, this.b_, this.bz, this.by, this.bx, 7);
        } else if (this.ba > this.bf[15] && this.ba < this.bf[16] + 1) {
            this.o(this.bz, this.by, this.bx);
        } else if (this.ba > this.bf[16] && this.ba < this.bf[17] + 1) {
            byte cfr_ignored_5 = (byte)(this.bf[17] >> 4);
            if (n == 1) {
                this.n = this.ba >> 2;
                this.m(this.bz, this.by, this.bx, this.bw, this.bv, this.bu, 8);
            } else {
                this.m(this.bz, this.by, this.bx, this.cn, this.cm, this.cl, 8);
            }
        } else if (this.ba > this.bf[17] && this.ba < this.bf[18]) {
            if (n == 1) {
                this.n = this.ba;
                this.o(this.bw, this.bv, this.bu);
            } else {
                this.o(this.cn, this.cm, this.cl);
            }
        }
        if (n == 2) {
            this.o(n, this.bq, this.bp, this.bo, this.bn);
        }
        if (this.ba >= this.bf[18] - 1) {
            this.n = this.ba;
            this.ba = 0;
            if (n != 1) {
                this.ba += 500;
            }
            this.a3 = 891;
            if (n == 2) {
                this.a3 = 0;
            }
        }
    }

    public final void v(byte[] byArray, int n, int n2, int n3, int[] nArray, int[] nArray2, int[] nArray3) {
        int n4;
        int n5;
        int n6 = (int)((double)this.a8 - 600.0);
        int n7 = (int)((double)this.a8 + 600.0);
        int n8 = (n7 - n6) / 6;
        int n9 = 143;
        int n10 = n2 / 2 - 7;
        switch (this.az) {
            case 1: {
                float f;
                n5 = aw - 1;
                while (n5 >= 0) {
                    n4 = nArray3[n5];
                    f = (float)(300.0 / (300.0 - (double)n4));
                    nArray[n5] = (int)((float)nArray[n5] * f + 150.0f - 7.0f);
                    nArray2[n5] = (int)((float)nArray2[n5] * f + (float)n10);
                    this.n = n5--;
                }
                break;
            }
            case 0: {
                float f;
                n5 = 0;
                while (n5 < aw) {
                    n4 = nArray3[n5] / 2;
                    f = (float)(300.0 - (double)n4);
                    nArray[n5] = (int)((float)nArray2[n5] * f * 2.0f + (float)(n9 / 2));
                    ++n5;
                }
                break;
            }
        }
        this.r(nArray3, this.bj);
        n5 = 0;
        while (n5 < aw) {
            int n11 = this.bh[n5] - 1;
            int n12 = nArray[n11];
            int n13 = nArray2[n11];
            n4 = nArray3[n11];
            if (n12 < 0) {
                n12 = 0;
            } else if (n12 >= 284) {
                n12 = 284;
            }
            if (n13 < 0) {
                n13 = 0;
            } else if (n13 >= n2 - 16) {
                n13 = n2 + -16;
            }
            n11 = n4 < n6 + n8 ? 5 : (n4 < n6 + n8 * 2 ? 4 : (n4 < n6 + n8 * 3 ? 3 : (n4 < n6 + n8 * 4 ? 2 : (n4 < n6 + n8 * 5 ? 1 : 0))));
            int n14 = n12 + 300 * n13;
            this.f(byArray, n14, n11 + 1);
            ++n5;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void u(byte[] byArray, int n, int n2, int n3, int[] nArray, int[] nArray2, int[] nArray3) {
        int n4;
        int n5 = (int)((double)this.a8 - 600.0);
        int n6 = (int)((double)this.a8 + 600.0);
        int n7 = (n6 - n5) / 6;
        int n8 = 126;
        int n9 = n2 / 2 - 24;
        int n10 = aw - 1;
        while (n10 >= 0) {
            n4 = nArray3[n10];
            float f = (float)(300.0 / (300.0 - (double)n4));
            nArray[n10] = (int)((float)nArray[n10] * f + 150.0f - 24.0f);
            nArray2[n10] = (int)((float)nArray2[n10] * f + (float)n9);
            --n10;
        }
        this.r(nArray3, this.bj);
        n10 = 0;
        while (n10 < aw) {
            int n11 = this.bh[n10] - 1;
            int n12 = nArray[n11];
            int n13 = nArray2[n11];
            n4 = nArray3[n11];
            if (n12 < 0) {
                n12 = 0;
            } else if (n12 >= 252) {
                n12 = 251;
            }
            if (n13 < 0) {
                n13 = 0;
            } else if (n13 >= n2 - 48) {
                n13 = n2 - 48 - 1;
            }
            n11 = n4 < n5 + n7 ? 5 : (n4 < n5 + n7 * 2 ? 4 : (n4 < n5 + n7 * 3 ? 3 : (n4 < n5 + n7 * 4 ? 2 : (n4 < n5 + n7 * 5 ? 1 : 0))));
            int n14 = n12 + 300 * n13;
            this.t(byArray, n14, n11 + 1, this.bl, this.bn);
            ++n10;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void t(byte[] byArray, int n, int n2, int[] nArray, int[] nArray2) {
        int n3;
        int n4 = 0;
        this.n = n;
        int n5 = n;
        int n6 = 48;
        while (n6 > 0) {
            this.n = n6 >> 2;
            n3 = 16;
            while (n3 > 0) {
                if (byArray[n5] == -1 && nArray[n4] == 1) {
                    byArray[n5] = 0;
                }
                if (byArray[++n5] == -1 && nArray[++n4] == 1) {
                    byArray[n5] = 0;
                }
                if (byArray[++n5] == -1 && nArray[++n4] == 1) {
                    byArray[n5] = 0;
                }
                ++n5;
                ++n4;
                --n3;
            }
            n5 += 252;
            --n6;
        }
        switch (this.az) {
            case 0: {
                n3 = 0;
                while (true) {
                    if (n3 >= this.bm >> 1) {
                        return;
                    }
                    byArray[n + nArray2[n3]] = -128;
                    ++n3;
                }
            }
            case 1: {
                n3 = this.bm - 1;
                while (n3 >= 0) {
                    byArray[n + nArray2[n3]] = -1;
                    --n3;
                }
                return;
            }
        }
    }

    public final void s(byte[] byArray, int n, int n2, int n3, int n4) {
        this.n = 1;
        this.w(n2);
        if (n == 1) {
            this.v(byArray, n3, n4, n, this.bt, this.bs, this.br);
        } else {
            this.u(byArray, n3, n4, n, this.bt, this.bs, this.br);
        }
        if (n == 1) {
            this.n = n;
            this.p(byArray, n3, n4);
        }
    }

    /*
     * Unable to fully structure code
     */
    private final void r(int[] var1_1, int[] var2_2) {
        System.arraycopy(this.bi, 0, var2_2, 0, 1600);
        var5_3 = 0;
        while (var5_3 < a.aw) {
            block5: {
                var3_4 = var1_1[var5_3];
                var4_5 = var3_4 - this.a8 + 800;
                if (var2_2[var4_5] != 0) ** GOTO lbl10
                var2_2[var4_5] = var5_3 + 1;
                break block5;
lbl-1000:
                // 1 sources

                {
                    ++var4_5;
lbl10:
                    // 2 sources

                    ** while (var2_2[var4_5] != 0)
                }
lbl11:
                // 1 sources

                var2_2[var4_5] = var5_3 + 1;
            }
            ++var5_3;
        }
        var4_5 = 0;
        this.bk = 0;
        if (true) ** GOTO lbl24
        do {
            this.at[1][this.bk] = Math.sin(0.02f * (float)this.bk);
            ++this.bk;
lbl24:
            // 2 sources

        } while (this.bk < 2);
        var5_3 = 0;
        while (var5_3 < 1600) {
            if (var2_2[var5_3] != 0) {
                this.bh[var4_5] = var2_2[var5_3];
                ++var4_5;
            }
            ++var5_3;
        }
    }

    private final void q() {
        this.av = new int[720];
        float cfr_ignored_0 = (float)Math.exp(5.0);
        int n = 0;
        while (n < 360) {
            this.av[n] = (int)(Math.cos((double)0.034906f * (double)n * 6.0) * 5.0);
            this.av[n + 360] = this.av[n];
            ++n;
        }
    }

    public final void p(byte[] byArray, int n, int n2) {
        int n3 = (int)((double)n2 * 4.0 / 5.0);
        int n4 = (n3 - 4) * n;
        int n5 = n3 * n;
        int n6 = this.au;
        float cfr_ignored_0 = (float)Math.exp(5.0);
        int n7 = n3;
        while (n7 < n2) {
            int n8 = 0;
            while (n8 < 5) {
                byArray[n5++] = -1;
                ++n8;
            }
            int n9 = n4 + this.av[n6];
            n8 = 5;
            while (n8 < 295) {
                byArray[n5++] = (byte)(byArray[n9 + n8] | 0x40);
                ++n8;
            }
            n4 -= 1200;
            n8 = 0;
            while (n8 < 5) {
                byArray[n5++] = -1;
                ++n8;
            }
            ++n6;
            ++n7;
        }
        ++this.au;
        if (this.au > 359) {
            this.au = 0;
        }
    }

    private final void o(int[] nArray, int[] nArray2, int[] nArray3) {
        int n = aw - 1;
        while (n >= 0) {
            this.n = n;
            int n2 = nArray[n];
            int n3 = nArray2[n];
            int n4 = nArray3[n];
            this.bt[n] = (int)(this.ar * (float)n2 + this.aq * (float)n3 + this.ap * (float)n4);
            this.bs[n] = (int)(this.ao * (float)n2 + this.an * (float)n3 + this.am * (float)n4 + (float)this.a3);
            this.br[n] = (int)(this.al * (float)n2 + this.ak * (float)n3 + this.aj * (float)n4 + (float)this.a8);
            --n;
        }
    }

    private final void o(int n, float[] fArray, float[] fArray2, float[] fArray3, int[] nArray) {
        this.n = this.bm = 0;
        int n2 = 0;
        while (n2 < this.as) {
            float f = fArray[n2];
            float f2 = fArray2[n2];
            float f3 = fArray3[n2];
            int n3 = (int)(this.al * f + this.ak * f2 + this.aj * f3 + 0.0f);
            if (n3 >= 0) {
                int n4 = (int)((double)(this.ar * f + this.aq * f2 + this.ap * f3) + 24.5);
                int n5 = (int)((double)(this.ao * f + this.an * f2 + this.am * f3) + 24.5);
                nArray[this.bm] = n4 + n5 * 300;
                ++this.bm;
            }
            ++n2;
        }
    }

    public final void n(byte[] byArray, byte[] byArray2) {
        int n = 0;
        while (n < 3300) {
            byArray2[n] = (byArray[n / 8] >> 7 - n % 8 & 1) == 1 ? -1 : 0;
            ++n;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void m(int[] nArray, int[] nArray2, int[] nArray3, int[] nArray4, int[] nArray5, int[] nArray6, int n) {
        n = this.ba - this.bf[n * 2];
        float f = (float)n / 100.0f;
        this.bk = 0;
        while (this.bk < 5) {
            this.at[1][this.bk] = Math.cos(0.02f * (float)this.bk);
            ++this.bk;
        }
        int n2 = aw - 1;
        while (n2 >= 0) {
            int n3 = nArray[n2];
            int n4 = nArray2[n2];
            int n5 = nArray3[n2];
            this.ay = (int)((float)n3 + (float)(nArray4[n2] - n3) * f);
            this.q = (int)((float)n4 + (float)(nArray5[n2] - n4) * f);
            this.ax = (int)((float)n5 + (float)(nArray6[n2] - n5) * f);
            this.bt[n2] = (int)(this.ar * (float)this.ay + this.aq * (float)this.q + this.ap * (float)this.ax);
            this.bs[n2] = (int)(this.ao * (float)this.ay + this.an * (float)this.q + this.am * (float)this.ax + (float)this.a3);
            this.br[n2] = (int)(this.al * (float)this.ay + this.ak * (float)this.q + this.aj * (float)this.ax + (float)this.a8);
            --n2;
        }
    }

    private final void l() {
        float f = (float)Math.cos(this.a6);
        float f2 = (float)Math.sin(this.a6);
        float f3 = (float)Math.cos(this.a4);
        float f4 = (float)Math.sin(this.a4);
        float f5 = (float)Math.cos(this.a9);
        float f6 = (float)Math.sin(this.a9);
        this.ar = f * f3;
        this.aq = -(f * f4 * f6 + f2 * f5);
        this.ap = -(f * f4 * f5 - f2 * f6);
        this.ao = f2 * f3;
        this.an = -(f2 * f4 * f6 - f * f5);
        this.am = -(f2 * f4 * f5 + f * f6);
        this.al = f4;
        this.bk = 0;
        while (this.bk < 2) {
            this.at[1][this.bk] = Math.log(0.02f * (float)this.bk);
            ++this.bk;
        }
        this.ak = f3 * f6;
        this.aj = f3 * f5;
    }

    private final void k(int n) {
        this.a_ = 8;
        this.bc = new float[this.a_ * 3];
        this.bd = new int[this.a_ + 1];
        this.bd[0] = 150;
        this.bc[0] = 0.333333f;
        this.bc[1] = 0.666667f;
        this.bc[2] = 0.0f;
        this.bd[1] = 100;
        this.bc[3] = 0.666667f;
        this.bc[4] = 0.666667f;
        this.bc[5] = 0.666667f;
        this.bd[2] = 100;
        this.bc[6] = 0.666667f;
        this.bc[7] = 1.0f;
        this.bc[8] = 0.666667f;
        this.bd[3] = 100;
        this.bc[9] = 0.666667f;
        this.bc[10] = 0.333333f;
        this.bc[11] = 0.666667f;
        this.bd[4] = 50;
        this.bc[12] = 0.666667f;
        this.bc[13] = 0.666667f;
        this.bc[14] = 1.333333f;
        float cfr_ignored_0 = (float)Math.cos(1.57f);
        this.bd[5] = 50;
        this.bc[15] = 0.666667f;
        this.bc[16] = 1.0f;
        this.bc[17] = 1.333333f;
        this.bd[6] = 150;
        this.bc[18] = 0.666667f;
        this.bc[19] = -0.666667f;
        this.bc[20] = 1.0f;
        this.bd[7] = 100;
        this.bc[21] = 0.666667f;
        this.bc[22] = -0.333333f;
        this.bc[23] = 0.666667f;
        if (n == 2) {
            int n2 = 0;
            while (n2 < this.a_ * 3) {
                byte cfr_ignored_1 = (byte)this.bc[n2];
                int n3 = n2++;
                this.bc[n3] = this.bc[n3] * 1.2f;
            }
        }
    }

    private final void j() {
        this.a5 = this.bc[3 * this.a0];
        this.a2 = this.bc[1 + 3 * this.a0];
        this.a7 = this.bc[2 + 3 * this.a0];
        this.a6 += 0.017453f * this.a5;
        this.a4 += 0.017453f * this.a2;
        this.a9 += 0.017453f * this.a7;
        ++this.a1;
        if (this.a1 >= this.bd[this.a0]) {
            this.a1 = 0;
            ++this.a0;
            if (this.a0 >= this.a_) {
                ++this.n;
                this.a0 = 0;
                this.bk = 0;
                while (this.bk < 10) {
                    this.at[2][this.bk] = Math.exp((double)this.bk * (double)0.03f);
                    ++this.bk;
                }
            }
        }
    }

    private final void i() {
        int n = 0;
        this.be = 10;
        this.bg = new int[this.be * 2];
        this.bf = new int[this.be * 2];
        this.bg[0] = 1000;
        this.bg[1] = 100;
        this.bg[2] = 200;
        this.bg[3] = 100;
        this.bg[4] = 200;
        this.bg[5] = 100;
        this.bg[6] = 200;
        this.bg[7] = 100;
        this.bg[8] = 200;
        this.bg[9] = 100;
        this.bg[10] = 200;
        this.bg[11] = 100;
        this.bg[12] = 150;
        this.bg[13] = 100;
        this.bg[14] = 200;
        this.bg[15] = 100;
        this.bg[16] = 200;
        this.bg[17] = 100;
        this.bg[18] = 200;
        int n2 = 0;
        while (n2 < this.be * 2 - 1) {
            this.bf[n2] = n += this.bg[n2];
            this.bk = 0;
            while (this.bk < 10) {
                this.at[2][this.bk] = Math.cos((double)this.bk * (double)0.03f);
                ++this.bk;
            }
            ++n2;
        }
    }

    private final void h() {
        this.ba = 0;
    }

    private final void g(int n) {
        int n2;
        int n3;
        int n4 = 0;
        this.ay = (int)Math.sqrt(aw);
        if (n == 1) {
            n3 = 0;
            while (n3 < this.ay) {
                n2 = 0;
                while (n2 < this.ay) {
                    this.cl[n3 + n2 * this.ay] = (int)(Math.cos((double)6.283f / (double)this.ay * (double)n3 * 2.0) * Math.sin((double)3.14159f / (double)this.ay * (double)n2 * 2.0) * 400.0 / 5.0);
                    this.cn[n3 + n2 * this.ay] = (int)((double)(400 * n3 / this.ay) * (double)2.2f - 440.0);
                    this.cm[n3 + n2 * this.ay] = (int)((double)(400 * n2 / this.ay) * (double)2.2f - 440.0);
                    ++n2;
                }
                ++n3;
            }
        } else {
            n3 = 0;
            while (n3 < this.ay) {
                n2 = 0;
                while (n2 < this.ay) {
                    this.cl[n3 + n2 * this.ay] = (int)(Math.cos((double)6.283f / (double)this.ay * (double)n3 * 2.0) * Math.sin((double)3.14159f / (double)this.ay * (double)n2 * 2.0) * 400.0 / 6.0);
                    this.cn[n3 + n2 * this.ay] = (int)((double)(400 * n3 / this.ay) * (double)2.3f - 360.0);
                    this.cm[n3 + n2 * this.ay] = (int)((double)(400 * n2 / this.ay) * (double)2.3f - 360.0);
                    ++n2;
                }
                ++n3;
            }
        }
        if (n == 1) {
            byte cfr_ignored_0 = (byte)this.ay;
            n3 = 0;
            while (n3 < this.ay) {
                n2 = 0;
                while (n2 < this.ay) {
                    this.ci[n3 + n2 * this.ay] = (n3 - this.ay / 2) * (n3 - this.ay / 2) * (n2 - this.ay / 2) * (n2 - this.ay / 2) * 400 / (this.ay * this.ay * this.ay * this.ay / 16) - 200;
                    this.ck[n3 + n2 * this.ay] = 400 * n3 / this.ay * 2 - 400;
                    this.cj[n3 + n2 * this.ay] = 400 * n2 / this.ay * 2 - 400;
                    ++n2;
                }
                ++n3;
            }
        } else {
            n3 = 0;
            while (n3 < this.ay) {
                n2 = 0;
                while (n2 < this.ay) {
                    this.ci[n3 + n2 * this.ay] = (int)((double)((n3 - this.ay / 2) * (n3 - this.ay / 2) * (n2 - this.ay / 2) * (n2 - this.ay / 2) * 400 / (this.ay * this.ay * this.ay * this.ay / 16)) - 160.0);
                    this.ck[n3 + n2 * this.ay] = (int)((double)(400 * n3 / this.ay) * 2.5 - 400.0);
                    this.cj[n3 + n2 * this.ay] = (int)((double)(400 * n2 / this.ay) * 2.5 - 400.0);
                    ++n2;
                }
                ++n3;
            }
        }
        Math.sqrt(aw * 2);
        n3 = 0;
        while (n3 < this.ay) {
            n2 = 0;
            while (n2 < this.ay) {
                this.cf[n3 + n2 * this.ay] = (int)((double)(-(n3 - this.ay / 2) * (n3 - this.ay / 2) * (n2 - this.ay / 2) * (n2 - this.ay / 2) * 400 / (this.ay * this.ay * this.ay * this.ay / 16)) + Math.cos((double)1.2566f * (double)(n3 * n2)) * (double)this.ay * (double)this.ay / 2.0 + 160.0);
                this.ch[n3 + n2 * this.ay] = 400 * n3 / this.ay * 2 - 400;
                this.cg[n3 + n2 * this.ay] = 400 * n2 / this.ay * 2 - 400;
                ++n2;
            }
            ++n3;
        }
        n4 = 1;
        if (n == 2) {
            n4 = 5;
        }
        n3 = 0;
        while (n3 < aw) {
            this.cc[n3] = (int)(Math.cos((double)0.069811f * (double)n3 * (double)n4) * Math.sin((double)0.10472f * (double)n3 * (double)n4) * 400.0 * (double)0.9f);
            this.ce[n3] = (int)(Math.sin((double)0.089757f * (double)n3 * (double)n4) * Math.sin((double)0.062832f * (double)n3 * (double)n4) * 400.0 * (double)0.9f);
            this.cd[n3] = (int)(Math.sin((double)0.125664f * (double)n3 * (double)n4) * 400.0);
            ++n3;
        }
        n3 = 0;
        while (n3 < aw) {
            this.ca[n3] = (int)(Math.cos((double)0.6283f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3) * 400.0);
            this.b8[n3] = (int)(Math.sin((double)0.6283f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3) * 400.0);
            this.b9[n3] = 800 / aw * n3 - 400;
            ++n3;
        }
        n3 = 0;
        while (n3 < aw) {
            this.b7[n3] = (int)(Math.sin((double)0.6283f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3 / 2.0) * 400.0);
            this.b5[n3] = (int)(Math.cos((double)0.6283f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3 / 2.0) * 400.0);
            this.b6[n3] = (int)((double)(800 / aw * n3) * 1.5 - 600.0);
            ++n3;
        }
        n3 = 0;
        while (n3 < aw) {
            this.b4[n3] = (int)(Math.sin((double)0.61598f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3 / 2.0) * 400.0);
            this.b2[n3] = (int)(Math.cos((double)0.61598f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3 / 2.0) * 400.0);
            this.b3[n3] = (int)((double)(800 / aw * n3) * 1.5 - 600.0);
            float cfr_ignored_1 = this.b3[n3];
            ++n3;
        }
        n3 = 0;
        while (n3 < aw) {
            this.b1[n3] = (int)(Math.cos((double)0.6283f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3 * 1.5) * 400.0);
            this.b_[n3] = (int)(Math.sin((double)0.6283f * (double)n3) * Math.sin((double)6.28318f / (double)aw * (double)n3 * 1.5) * 400.0);
            this.b0[n3] = 800 / aw * n3 - 400;
            ++n3;
        }
        n3 = 0;
        while (n3 < aw) {
            this.bx[n3] = 400 * n3 / aw * 2 - 400;
            this.bz[n3] = (int)(Math.cos((double)6.283f / (double)aw * (double)n3 * 10.0) * 400.0);
            this.by[n3] = (int)(Math.sin((double)6.283f / (double)aw * (double)n3 * 10.0) * 400.0);
            float cfr_ignored_2 = this.by[n3];
            ++n3;
        }
        n3 = 0;
        while (n3 < aw) {
            this.bu[n3] = (int)(600.0 * Math.random() - 300.0);
            this.bw[n3] = (int)(600.0 * Math.random() - 300.0);
            this.bv[n3] = (int)(600.0 * Math.random() - 300.0);
            ++n3;
        }
    }

    public a() {
        int[] nArray = new int[184];
        nArray[0] = 1;
        nArray[1] = 1;
        nArray[2] = 1;
        nArray[3] = 1;
        nArray[4] = 1;
        nArray[5] = 1;
        nArray[8] = 1;
        nArray[9] = 1;
        nArray[10] = 1;
        nArray[11] = 1;
        nArray[15] = 1;
        nArray[16] = 1;
        nArray[21] = 1;
        nArray[23] = 1;
        nArray[24] = 1;
        nArray[30] = 1;
        nArray[31] = 1;
        nArray[35] = 1;
        nArray[38] = 1;
        nArray[39] = 1;
        nArray[44] = 1;
        nArray[46] = 1;
        nArray[47] = 1;
        nArray[53] = 1;
        nArray[54] = 1;
        nArray[58] = 1;
        nArray[61] = 1;
        nArray[62] = 1;
        nArray[67] = 1;
        nArray[69] = 1;
        nArray[70] = 1;
        nArray[71] = 1;
        nArray[72] = 1;
        nArray[76] = 1;
        nArray[77] = 1;
        nArray[81] = 1;
        nArray[85] = 1;
        nArray[86] = 1;
        nArray[87] = 1;
        nArray[88] = 1;
        nArray[89] = 1;
        nArray[92] = 1;
        nArray[93] = 1;
        nArray[99] = 1;
        nArray[100] = 1;
        nArray[104] = 1;
        nArray[107] = 1;
        nArray[108] = 1;
        nArray[109] = 1;
        nArray[112] = 1;
        nArray[113] = 1;
        nArray[115] = 1;
        nArray[116] = 1;
        nArray[122] = 1;
        nArray[123] = 1;
        nArray[126] = 1;
        nArray[130] = 1;
        nArray[131] = 1;
        nArray[135] = 1;
        nArray[136] = 1;
        nArray[138] = 1;
        nArray[139] = 1;
        nArray[140] = 1;
        nArray[141] = 1;
        nArray[142] = 1;
        nArray[143] = 1;
        nArray[145] = 1;
        nArray[146] = 1;
        nArray[147] = 1;
        nArray[148] = 1;
        nArray[149] = 1;
        nArray[150] = 1;
        nArray[153] = 1;
        nArray[154] = 1;
        nArray[158] = 1;
        nArray[159] = 1;
        nArray[161] = 1;
        nArray[162] = 1;
        nArray[163] = 1;
        nArray[164] = 1;
        nArray[165] = 1;
        nArray[166] = 1;
        nArray[169] = 1;
        nArray[170] = 1;
        nArray[171] = 1;
        nArray[174] = 1;
        nArray[176] = 1;
        nArray[177] = 1;
        nArray[181] = 1;
        nArray[182] = 1;
    }

    private final void f(byte[] byArray, int n, int n2) {
        switch (n2) {
            case 1: {
                byArray[5 + n] = 61;
                byArray[6 + n] = 45;
                byArray[7 + n] = 30;
                byArray[8 + n] = 45;
                byArray[9 + n] = 57;
                byArray[303 + n] = 57;
                byArray[304 + n] = 38;
                byArray[305 + n] = 28;
                byArray[306 + n] = 19;
                byArray[307 + n] = 20;
                byArray[308 + n] = 25;
                byArray[309 + n] = 33;
                byArray[310 + n] = 38;
                byArray[311 + n] = 57;
                byArray[602 + n] = 45;
                byArray[603 + n] = 27;
                byArray[604 + n] = 20;
                byArray[605 + n] = 7;
                byArray[606 + n] = 3;
                byArray[607 + n] = 7;
                byArray[608 + n] = 20;
                byArray[609 + n] = 25;
                byArray[610 + n] = 26;
                byArray[611 + n] = 30;
                byArray[612 + n] = 47;
                byArray[901 + n] = 45;
                byArray[902 + n] = 23;
                byArray[903 + n] = 11;
                byArray[904 + n] = 3;
                byArray[905 + n] = 1;
                byArray[906 + n] = 3;
                byArray[907 + n] = 20;
                byArray[908 + n] = 26;
                byArray[909 + n] = 30;
                byArray[910 + n] = 30;
                byArray[911 + n] = 30;
                byArray[912 + n] = 30;
                byArray[913 + n] = 60;
                byArray[1200 + n] = 57;
                byArray[1201 + n] = 28;
                byArray[1202 + n] = 10;
                byArray[1203 + n] = 3;
                byArray[1204 + n] = 1;
                byArray[1205 + n] = 1;
                byArray[1206 + n] = 11;
                byArray[1207 + n] = 25;
                byArray[1208 + n] = 32;
                byArray[1209 + n] = 32;
                byArray[1210 + n] = 32;
                byArray[1211 + n] = 32;
                byArray[1212 + n] = 28;
                byArray[1213 + n] = 45;
                byArray[1500 + n] = 38;
                byArray[1501 + n] = 20;
                byArray[1502 + n] = 3;
                byArray[1503 + n] = 1;
                byArray[1504 + n] = 1;
                byArray[1505 + n] = 3;
                byArray[1506 + n] = 20;
                byArray[1507 + n] = 28;
                byArray[1508 + n] = 32;
                byArray[1509 + n] = 30;
                byArray[1510 + n] = 26;
                byArray[1511 + n] = 32;
                byArray[1512 + n] = 30;
                byArray[1513 + n] = 33;
                byArray[1514 + n] = 57;
                byArray[1800 + n] = 30;
                byArray[1801 + n] = 11;
                byArray[1802 + n] = 1;
                byArray[1803 + n] = 1;
                byArray[1804 + n] = 1;
                byArray[1805 + n] = 7;
                byArray[1806 + n] = 22;
                byArray[1807 + n] = 32;
                byArray[1808 + n] = 26;
                byArray[1809 + n] = 23;
                byArray[1810 + n] = 19;
                byArray[1811 + n] = 30;
                byArray[1812 + n] = 30;
                byArray[1813 + n] = 28;
                byArray[1814 + n] = 38;
                byArray[2100 + n] = 22;
                byArray[2101 + n] = 3;
                byArray[2102 + n] = 1;
                byArray[2103 + n] = 1;
                byArray[2104 + n] = 3;
                byArray[2105 + n] = 13;
                byArray[2106 + n] = 30;
                byArray[2107 + n] = 32;
                byArray[2108 + n] = 28;
                byArray[2109 + n] = 23;
                byArray[2110 + n] = 25;
                byArray[2111 + n] = 28;
                byArray[2112 + n] = 19;
                byArray[2113 + n] = 21;
                byArray[2114 + n] = 32;
                byArray[2400 + n] = 29;
                byArray[2401 + n] = 11;
                byArray[2402 + n] = 3;
                byArray[2403 + n] = 3;
                byArray[2404 + n] = 7;
                byArray[2405 + n] = 23;
                byArray[2406 + n] = 30;
                byArray[2407 + n] = 26;
                byArray[2408 + n] = 28;
                byArray[2409 + n] = 32;
                byArray[2410 + n] = 30;
                byArray[2411 + n] = 25;
                byArray[2412 + n] = 20;
                byArray[2413 + n] = 23;
                byArray[2414 + n] = 38;
                byArray[2700 + n] = 38;
                byArray[2701 + n] = 23;
                byArray[2702 + n] = 11;
                byArray[2703 + n] = 7;
                byArray[2704 + n] = 17;
                byArray[2705 + n] = 26;
                byArray[2706 + n] = 27;
                byArray[2707 + n] = 20;
                byArray[2708 + n] = 23;
                byArray[2709 + n] = 30;
                byArray[2710 + n] = 26;
                byArray[2711 + n] = 20;
                byArray[2712 + n] = 20;
                byArray[2713 + n] = 33;
                byArray[2714 + n] = 57;
                byArray[3000 + n] = 57;
                byArray[3001 + n] = 32;
                byArray[3002 + n] = 23;
                byArray[3003 + n] = 23;
                byArray[3004 + n] = 27;
                byArray[3005 + n] = 32;
                byArray[3006 + n] = 27;
                byArray[3007 + n] = 20;
                byArray[3008 + n] = 24;
                byArray[3009 + n] = 25;
                byArray[3010 + n] = 20;
                byArray[3011 + n] = 10;
                byArray[3012 + n] = 20;
                byArray[3013 + n] = 35;
                byArray[3301 + n] = 57;
                byArray[3302 + n] = 30;
                byArray[3303 + n] = 28;
                byArray[3304 + n] = 32;
                byArray[3305 + n] = 32;
                byArray[3306 + n] = 25;
                byArray[3307 + n] = 23;
                byArray[3308 + n] = 22;
                byArray[3309 + n] = 20;
                byArray[3310 + n] = 10;
                byArray[3311 + n] = 13;
                byArray[3312 + n] = 25;
                byArray[3313 + n] = 57;
                byArray[3602 + n] = 45;
                byArray[3603 + n] = 30;
                byArray[3604 + n] = 30;
                byArray[3605 + n] = 30;
                byArray[3606 + n] = 26;
                byArray[3607 + n] = 25;
                byArray[3608 + n] = 23;
                byArray[3609 + n] = 11;
                byArray[3610 + n] = 13;
                byArray[3611 + n] = 26;
                byArray[3612 + n] = 45;
                byArray[3903 + n] = 57;
                byArray[3904 + n] = 33;
                byArray[3905 + n] = 26;
                byArray[3906 + n] = 25;
                byArray[3907 + n] = 19;
                byArray[3908 + n] = 20;
                byArray[3909 + n] = 19;
                byArray[3910 + n] = 32;
                byArray[3911 + n] = 57;
                byArray[4205 + n] = 38;
                byArray[4206 + n] = 38;
                byArray[4207 + n] = 30;
                byArray[4208 + n] = 30;
                byArray[4209 + n] = 38;
                return;
            }
            case 2: {
                byArray[5 + n] = 61;
                byArray[6 + n] = 50;
                byArray[7 + n] = 33;
                byArray[8 + n] = 50;
                byArray[9 + n] = 61;
                byArray[303 + n] = 61;
                byArray[304 + n] = 43;
                byArray[305 + n] = 32;
                byArray[306 + n] = 25;
                byArray[307 + n] = 23;
                byArray[308 + n] = 28;
                byArray[309 + n] = 35;
                byArray[310 + n] = 43;
                byArray[311 + n] = 61;
                byArray[602 + n] = 50;
                byArray[603 + n] = 31;
                byArray[604 + n] = 23;
                byArray[605 + n] = 10;
                byArray[606 + n] = 5;
                byArray[607 + n] = 10;
                byArray[608 + n] = 23;
                byArray[609 + n] = 28;
                byArray[610 + n] = 30;
                byArray[611 + n] = 33;
                byArray[612 + n] = 54;
                byArray[901 + n] = 50;
                byArray[902 + n] = 25;
                byArray[903 + n] = 16;
                byArray[904 + n] = 5;
                byArray[905 + n] = 2;
                byArray[906 + n] = 5;
                byArray[907 + n] = 23;
                byArray[908 + n] = 31;
                byArray[909 + n] = 33;
                byArray[910 + n] = 33;
                byArray[911 + n] = 33;
                byArray[912 + n] = 33;
                byArray[913 + n] = 61;
                byArray[1200 + n] = 61;
                byArray[1201 + n] = 32;
                byArray[1202 + n] = 16;
                byArray[1203 + n] = 5;
                byArray[1204 + n] = 2;
                byArray[1205 + n] = 2;
                byArray[1206 + n] = 17;
                byArray[1207 + n] = 28;
                byArray[1208 + n] = 35;
                byArray[1209 + n] = 35;
                byArray[1210 + n] = 33;
                byArray[1211 + n] = 35;
                byArray[1212 + n] = 32;
                byArray[1213 + n] = 50;
                byArray[1500 + n] = 43;
                byArray[1501 + n] = 23;
                byArray[1502 + n] = 5;
                byArray[1503 + n] = 2;
                byArray[1504 + n] = 2;
                byArray[1505 + n] = 7;
                byArray[1506 + n] = 23;
                byArray[1507 + n] = 32;
                byArray[1508 + n] = 35;
                byArray[1509 + n] = 32;
                byArray[1510 + n] = 31;
                byArray[1511 + n] = 35;
                byArray[1512 + n] = 33;
                byArray[1513 + n] = 38;
                byArray[1514 + n] = 60;
                byArray[1800 + n] = 33;
                byArray[1801 + n] = 17;
                byArray[1802 + n] = 2;
                byArray[1803 + n] = 2;
                byArray[1804 + n] = 2;
                byArray[1805 + n] = 10;
                byArray[1806 + n] = 25;
                byArray[1807 + n] = 34;
                byArray[1808 + n] = 32;
                byArray[1809 + n] = 24;
                byArray[1810 + n] = 24;
                byArray[1811 + n] = 33;
                byArray[1812 + n] = 32;
                byArray[1813 + n] = 32;
                byArray[1814 + n] = 43;
                byArray[2100 + n] = 25;
                byArray[2101 + n] = 5;
                byArray[2102 + n] = 2;
                byArray[2103 + n] = 2;
                byArray[2104 + n] = 7;
                byArray[2105 + n] = 19;
                byArray[2106 + n] = 32;
                byArray[2107 + n] = 35;
                byArray[2108 + n] = 31;
                byArray[2109 + n] = 25;
                byArray[2110 + n] = 29;
                byArray[2111 + n] = 31;
                byArray[2112 + n] = 24;
                byArray[2113 + n] = 25;
                byArray[2114 + n] = 35;
                byArray[2400 + n] = 32;
                byArray[2401 + n] = 17;
                byArray[2402 + n] = 5;
                byArray[2403 + n] = 7;
                byArray[2404 + n] = 10;
                byArray[2405 + n] = 25;
                byArray[2406 + n] = 33;
                byArray[2407 + n] = 31;
                byArray[2408 + n] = 32;
                byArray[2409 + n] = 35;
                byArray[2410 + n] = 33;
                byArray[2411 + n] = 28;
                byArray[2412 + n] = 23;
                byArray[2413 + n] = 25;
                byArray[2414 + n] = 43;
                byArray[2700 + n] = 43;
                byArray[2701 + n] = 25;
                byArray[2702 + n] = 16;
                byArray[2703 + n] = 10;
                byArray[2704 + n] = 19;
                byArray[2705 + n] = 30;
                byArray[2706 + n] = 31;
                byArray[2707 + n] = 23;
                byArray[2708 + n] = 25;
                byArray[2709 + n] = 33;
                byArray[2710 + n] = 31;
                byArray[2711 + n] = 23;
                byArray[2712 + n] = 23;
                byArray[2713 + n] = 36;
                byArray[2714 + n] = 61;
                byArray[3000 + n] = 61;
                byArray[3001 + n] = 35;
                byArray[3002 + n] = 24;
                byArray[3003 + n] = 25;
                byArray[3004 + n] = 31;
                byArray[3005 + n] = 35;
                byArray[3006 + n] = 31;
                byArray[3007 + n] = 23;
                byArray[3008 + n] = 25;
                byArray[3009 + n] = 28;
                byArray[3010 + n] = 23;
                byArray[3011 + n] = 16;
                byArray[3012 + n] = 23;
                byArray[3013 + n] = 38;
                byArray[3301 + n] = 61;
                byArray[3302 + n] = 33;
                byArray[3303 + n] = 32;
                byArray[3304 + n] = 35;
                byArray[3305 + n] = 35;
                byArray[3306 + n] = 31;
                byArray[3307 + n] = 25;
                byArray[3308 + n] = 25;
                byArray[3309 + n] = 23;
                byArray[3310 + n] = 16;
                byArray[3311 + n] = 20;
                byArray[3312 + n] = 28;
                byArray[3313 + n] = 61;
                byArray[3602 + n] = 50;
                byArray[3603 + n] = 33;
                byArray[3604 + n] = 33;
                byArray[3605 + n] = 32;
                byArray[3606 + n] = 31;
                byArray[3607 + n] = 26;
                byArray[3608 + n] = 25;
                byArray[3609 + n] = 16;
                byArray[3610 + n] = 20;
                byArray[3611 + n] = 31;
                byArray[3612 + n] = 50;
                byArray[3903 + n] = 61;
                byArray[3904 + n] = 35;
                byArray[3905 + n] = 32;
                byArray[3906 + n] = 28;
                byArray[3907 + n] = 24;
                byArray[3908 + n] = 23;
                byArray[3909 + n] = 24;
                byArray[3910 + n] = 33;
                byArray[3911 + n] = 60;
                byArray[4205 + n] = 43;
                byArray[4206 + n] = 43;
                byArray[4207 + n] = 33;
                byArray[4208 + n] = 33;
                byArray[4209 + n] = 43;
                return;
            }
            case 3: {
                byArray[5 + n] = 62;
                byArray[6 + n] = 54;
                byArray[7 + n] = 36;
                byArray[8 + n] = 54;
                byArray[9 + n] = 62;
                byArray[303 + n] = 62;
                byArray[304 + n] = 47;
                byArray[305 + n] = 35;
                byArray[306 + n] = 27;
                byArray[307 + n] = 24;
                byArray[308 + n] = 32;
                byArray[309 + n] = 40;
                byArray[310 + n] = 46;
                byArray[311 + n] = 62;
                byArray[602 + n] = 54;
                byArray[603 + n] = 33;
                byArray[604 + n] = 25;
                byArray[605 + n] = 17;
                byArray[606 + n] = 8;
                byArray[607 + n] = 17;
                byArray[608 + n] = 25;
                byArray[609 + n] = 32;
                byArray[610 + n] = 33;
                byArray[611 + n] = 36;
                byArray[612 + n] = 58;
                byArray[901 + n] = 54;
                byArray[902 + n] = 29;
                byArray[903 + n] = 19;
                byArray[904 + n] = 8;
                byArray[905 + n] = 4;
                byArray[906 + n] = 8;
                byArray[907 + n] = 25;
                byArray[908 + n] = 33;
                byArray[909 + n] = 36;
                byArray[910 + n] = 35;
                byArray[911 + n] = 36;
                byArray[912 + n] = 36;
                byArray[913 + n] = 62;
                byArray[1200 + n] = 62;
                byArray[1201 + n] = 34;
                byArray[1202 + n] = 20;
                byArray[1203 + n] = 8;
                byArray[1204 + n] = 4;
                byArray[1205 + n] = 4;
                byArray[1206 + n] = 19;
                byArray[1207 + n] = 32;
                byArray[1208 + n] = 39;
                byArray[1209 + n] = 39;
                byArray[1210 + n] = 36;
                byArray[1211 + n] = 39;
                byArray[1212 + n] = 35;
                byArray[1213 + n] = 54;
                byArray[1500 + n] = 47;
                byArray[1501 + n] = 25;
                byArray[1502 + n] = 9;
                byArray[1503 + n] = 4;
                byArray[1504 + n] = 4;
                byArray[1505 + n] = 10;
                byArray[1506 + n] = 25;
                byArray[1507 + n] = 35;
                byArray[1508 + n] = 39;
                byArray[1509 + n] = 35;
                byArray[1510 + n] = 34;
                byArray[1511 + n] = 39;
                byArray[1512 + n] = 35;
                byArray[1513 + n] = 41;
                byArray[1514 + n] = 61;
                byArray[1800 + n] = 36;
                byArray[1801 + n] = 19;
                byArray[1802 + n] = 4;
                byArray[1803 + n] = 4;
                byArray[1804 + n] = 4;
                byArray[1805 + n] = 18;
                byArray[1806 + n] = 29;
                byArray[1807 + n] = 36;
                byArray[1808 + n] = 35;
                byArray[1809 + n] = 27;
                byArray[1810 + n] = 26;
                byArray[1811 + n] = 36;
                byArray[1812 + n] = 36;
                byArray[1813 + n] = 35;
                byArray[1814 + n] = 47;
                byArray[2100 + n] = 29;
                byArray[2101 + n] = 8;
                byArray[2102 + n] = 4;
                byArray[2103 + n] = 4;
                byArray[2104 + n] = 8;
                byArray[2105 + n] = 23;
                byArray[2106 + n] = 35;
                byArray[2107 + n] = 39;
                byArray[2108 + n] = 34;
                byArray[2109 + n] = 29;
                byArray[2110 + n] = 31;
                byArray[2111 + n] = 33;
                byArray[2112 + n] = 27;
                byArray[2113 + n] = 27;
                byArray[2114 + n] = 39;
                byArray[2400 + n] = 36;
                byArray[2401 + n] = 19;
                byArray[2402 + n] = 8;
                byArray[2403 + n] = 10;
                byArray[2404 + n] = 18;
                byArray[2405 + n] = 27;
                byArray[2406 + n] = 36;
                byArray[2407 + n] = 33;
                byArray[2408 + n] = 35;
                byArray[2409 + n] = 39;
                byArray[2410 + n] = 36;
                byArray[2411 + n] = 31;
                byArray[2412 + n] = 25;
                byArray[2413 + n] = 29;
                byArray[2414 + n] = 47;
                byArray[2700 + n] = 47;
                byArray[2701 + n] = 27;
                byArray[2702 + n] = 19;
                byArray[2703 + n] = 16;
                byArray[2704 + n] = 23;
                byArray[2705 + n] = 33;
                byArray[2706 + n] = 34;
                byArray[2707 + n] = 25;
                byArray[2708 + n] = 29;
                byArray[2709 + n] = 36;
                byArray[2710 + n] = 34;
                byArray[2711 + n] = 25;
                byArray[2712 + n] = 25;
                byArray[2713 + n] = 41;
                byArray[2714 + n] = 62;
                byArray[3000 + n] = 62;
                byArray[3001 + n] = 43;
                byArray[3002 + n] = 27;
                byArray[3003 + n] = 27;
                byArray[3004 + n] = 34;
                byArray[3005 + n] = 39;
                byArray[3006 + n] = 34;
                byArray[3007 + n] = 25;
                byArray[3008 + n] = 29;
                byArray[3009 + n] = 31;
                byArray[3010 + n] = 25;
                byArray[3011 + n] = 19;
                byArray[3012 + n] = 25;
                byArray[3013 + n] = 41;
                byArray[3301 + n] = 61;
                byArray[3302 + n] = 36;
                byArray[3303 + n] = 35;
                byArray[3304 + n] = 39;
                byArray[3305 + n] = 39;
                byArray[3306 + n] = 33;
                byArray[3307 + n] = 28;
                byArray[3308 + n] = 29;
                byArray[3309 + n] = 25;
                byArray[3310 + n] = 19;
                byArray[3311 + n] = 23;
                byArray[3312 + n] = 32;
                byArray[3313 + n] = 61;
                byArray[3602 + n] = 54;
                byArray[3603 + n] = 36;
                byArray[3604 + n] = 36;
                byArray[3605 + n] = 35;
                byArray[3606 + n] = 33;
                byArray[3607 + n] = 31;
                byArray[3608 + n] = 27;
                byArray[3609 + n] = 19;
                byArray[3610 + n] = 23;
                byArray[3611 + n] = 33;
                byArray[3612 + n] = 54;
                byArray[3903 + n] = 61;
                byArray[3904 + n] = 39;
                byArray[3905 + n] = 35;
                byArray[3906 + n] = 31;
                byArray[3907 + n] = 27;
                byArray[3908 + n] = 25;
                byArray[3909 + n] = 26;
                byArray[3910 + n] = 36;
                byArray[3911 + n] = 61;
                byArray[4205 + n] = 47;
                byArray[4206 + n] = 47;
                byArray[4207 + n] = 36;
                byArray[4208 + n] = 36;
                byArray[4209 + n] = 47;
                return;
            }
            case 4: {
                byArray[5 + n] = 63;
                byArray[6 + n] = 58;
                byArray[7 + n] = 41;
                byArray[8 + n] = 58;
                byArray[9 + n] = 63;
                byArray[303 + n] = 63;
                byArray[304 + n] = 54;
                byArray[305 + n] = 37;
                byArray[306 + n] = 31;
                byArray[307 + n] = 27;
                byArray[308 + n] = 35;
                byArray[309 + n] = 42;
                byArray[310 + n] = 54;
                byArray[311 + n] = 63;
                byArray[602 + n] = 58;
                byArray[603 + n] = 36;
                byArray[604 + n] = 27;
                byArray[605 + n] = 19;
                byArray[606 + n] = 12;
                byArray[607 + n] = 19;
                byArray[608 + n] = 27;
                byArray[609 + n] = 34;
                byArray[610 + n] = 36;
                byArray[611 + n] = 40;
                byArray[612 + n] = 62;
                byArray[901 + n] = 58;
                byArray[902 + n] = 33;
                byArray[903 + n] = 22;
                byArray[904 + n] = 12;
                byArray[905 + n] = 6;
                byArray[906 + n] = 16;
                byArray[907 + n] = 28;
                byArray[908 + n] = 36;
                byArray[909 + n] = 41;
                byArray[910 + n] = 40;
                byArray[911 + n] = 41;
                byArray[912 + n] = 41;
                byArray[913 + n] = 63;
                byArray[1200 + n] = 63;
                byArray[1201 + n] = 37;
                byArray[1202 + n] = 22;
                byArray[1203 + n] = 12;
                byArray[1204 + n] = 6;
                byArray[1205 + n] = 6;
                byArray[1206 + n] = 24;
                byArray[1207 + n] = 34;
                byArray[1208 + n] = 42;
                byArray[1209 + n] = 47;
                byArray[1210 + n] = 41;
                byArray[1211 + n] = 42;
                byArray[1212 + n] = 40;
                byArray[1213 + n] = 58;
                byArray[1500 + n] = 51;
                byArray[1501 + n] = 27;
                byArray[1502 + n] = 12;
                byArray[1503 + n] = 6;
                byArray[1504 + n] = 9;
                byArray[1505 + n] = 15;
                byArray[1506 + n] = 29;
                byArray[1507 + n] = 40;
                byArray[1508 + n] = 42;
                byArray[1509 + n] = 40;
                byArray[1510 + n] = 36;
                byArray[1511 + n] = 42;
                byArray[1512 + n] = 40;
                byArray[1513 + n] = 46;
                byArray[1514 + n] = 62;
                byArray[1800 + n] = 41;
                byArray[1801 + n] = 24;
                byArray[1802 + n] = 6;
                byArray[1803 + n] = 6;
                byArray[1804 + n] = 6;
                byArray[1805 + n] = 19;
                byArray[1806 + n] = 33;
                byArray[1807 + n] = 41;
                byArray[1808 + n] = 39;
                byArray[1809 + n] = 31;
                byArray[1810 + n] = 31;
                byArray[1811 + n] = 41;
                byArray[1812 + n] = 40;
                byArray[1813 + n] = 40;
                byArray[1814 + n] = 51;
                byArray[2100 + n] = 33;
                byArray[2101 + n] = 16;
                byArray[2102 + n] = 6;
                byArray[2103 + n] = 6;
                byArray[2104 + n] = 16;
                byArray[2105 + n] = 25;
                byArray[2106 + n] = 40;
                byArray[2107 + n] = 42;
                byArray[2108 + n] = 36;
                byArray[2109 + n] = 33;
                byArray[2110 + n] = 34;
                byArray[2111 + n] = 36;
                byArray[2112 + n] = 31;
                byArray[2113 + n] = 31;
                byArray[2114 + n] = 42;
                byArray[2400 + n] = 37;
                byArray[2401 + n] = 21;
                byArray[2402 + n] = 16;
                byArray[2403 + n] = 15;
                byArray[2404 + n] = 19;
                byArray[2405 + n] = 32;
                byArray[2406 + n] = 41;
                byArray[2407 + n] = 36;
                byArray[2408 + n] = 40;
                byArray[2409 + n] = 42;
                byArray[2410 + n] = 41;
                byArray[2411 + n] = 35;
                byArray[2412 + n] = 27;
                byArray[2413 + n] = 33;
                byArray[2414 + n] = 51;
                byArray[2700 + n] = 54;
                byArray[2701 + n] = 31;
                byArray[2702 + n] = 22;
                byArray[2703 + n] = 19;
                byArray[2704 + n] = 25;
                byArray[2705 + n] = 36;
                byArray[2706 + n] = 37;
                byArray[2707 + n] = 27;
                byArray[2708 + n] = 33;
                byArray[2709 + n] = 41;
                byArray[2710 + n] = 36;
                byArray[2711 + n] = 27;
                byArray[2712 + n] = 28;
                byArray[2713 + n] = 46;
                byArray[2714 + n] = 63;
                byArray[3000 + n] = 63;
                byArray[3001 + n] = 42;
                byArray[3002 + n] = 31;
                byArray[3003 + n] = 32;
                byArray[3004 + n] = 37;
                byArray[3005 + n] = 42;
                byArray[3006 + n] = 36;
                byArray[3007 + n] = 27;
                byArray[3008 + n] = 33;
                byArray[3009 + n] = 35;
                byArray[3010 + n] = 27;
                byArray[3011 + n] = 24;
                byArray[3012 + n] = 27;
                byArray[3013 + n] = 46;
                byArray[3301 + n] = 62;
                byArray[3302 + n] = 41;
                byArray[3303 + n] = 40;
                byArray[3304 + n] = 42;
                byArray[3305 + n] = 42;
                byArray[3306 + n] = 36;
                byArray[3307 + n] = 33;
                byArray[3308 + n] = 33;
                byArray[3309 + n] = 27;
                byArray[3310 + n] = 23;
                byArray[3311 + n] = 25;
                byArray[3312 + n] = 35;
                byArray[3313 + n] = 62;
                byArray[3602 + n] = 58;
                byArray[3603 + n] = 41;
                byArray[3604 + n] = 40;
                byArray[3605 + n] = 40;
                byArray[3606 + n] = 36;
                byArray[3607 + n] = 34;
                byArray[3608 + n] = 29;
                byArray[3609 + n] = 22;
                byArray[3610 + n] = 25;
                byArray[3611 + n] = 36;
                byArray[3612 + n] = 58;
                byArray[3903 + n] = 63;
                byArray[3904 + n] = 47;
                byArray[3905 + n] = 37;
                byArray[3906 + n] = 34;
                byArray[3907 + n] = 31;
                byArray[3908 + n] = 27;
                byArray[3909 + n] = 31;
                byArray[3910 + n] = 41;
                byArray[3911 + n] = 62;
                byArray[4205 + n] = 51;
                byArray[4206 + n] = 54;
                byArray[4207 + n] = 41;
                byArray[4208 + n] = 41;
                byArray[4209 + n] = 54;
                return;
            }
            case 5: {
                byArray[5 + n] = 63;
                byArray[6 + n] = 62;
                byArray[7 + n] = 44;
                byArray[8 + n] = 62;
                byArray[9 + n] = 63;
                byArray[303 + n] = 63;
                byArray[304 + n] = 55;
                byArray[305 + n] = 42;
                byArray[306 + n] = 34;
                byArray[307 + n] = 31;
                byArray[308 + n] = 37;
                byArray[309 + n] = 48;
                byArray[310 + n] = 55;
                byArray[311 + n] = 63;
                byArray[602 + n] = 63;
                byArray[603 + n] = 42;
                byArray[604 + n] = 32;
                byArray[605 + n] = 24;
                byArray[606 + n] = 18;
                byArray[607 + n] = 24;
                byArray[608 + n] = 31;
                byArray[609 + n] = 37;
                byArray[610 + n] = 41;
                byArray[611 + n] = 46;
                byArray[612 + n] = 63;
                byArray[901 + n] = 63;
                byArray[902 + n] = 34;
                byArray[903 + n] = 25;
                byArray[904 + n] = 18;
                byArray[905 + n] = 9;
                byArray[906 + n] = 18;
                byArray[907 + n] = 31;
                byArray[908 + n] = 41;
                byArray[909 + n] = 46;
                byArray[910 + n] = 46;
                byArray[911 + n] = 42;
                byArray[912 + n] = 46;
                byArray[913 + n] = 63;
                byArray[1200 + n] = 63;
                byArray[1201 + n] = 41;
                byArray[1202 + n] = 26;
                byArray[1203 + n] = 18;
                byArray[1204 + n] = 9;
                byArray[1205 + n] = 9;
                byArray[1206 + n] = 26;
                byArray[1207 + n] = 40;
                byArray[1208 + n] = 48;
                byArray[1209 + n] = 48;
                byArray[1210 + n] = 46;
                byArray[1211 + n] = 46;
                byArray[1212 + n] = 42;
                byArray[1213 + n] = 62;
                byArray[1500 + n] = 55;
                byArray[1501 + n] = 32;
                byArray[1502 + n] = 18;
                byArray[1503 + n] = 9;
                byArray[1504 + n] = 9;
                byArray[1505 + n] = 18;
                byArray[1506 + n] = 31;
                byArray[1507 + n] = 42;
                byArray[1508 + n] = 48;
                byArray[1509 + n] = 42;
                byArray[1510 + n] = 41;
                byArray[1511 + n] = 48;
                byArray[1512 + n] = 42;
                byArray[1513 + n] = 52;
                byArray[1514 + n] = 63;
                byArray[1800 + n] = 46;
                byArray[1801 + n] = 26;
                byArray[1802 + n] = 9;
                byArray[1803 + n] = 9;
                byArray[1804 + n] = 9;
                byArray[1805 + n] = 24;
                byArray[1806 + n] = 36;
                byArray[1807 + n] = 46;
                byArray[1808 + n] = 42;
                byArray[1809 + n] = 33;
                byArray[1810 + n] = 34;
                byArray[1811 + n] = 42;
                byArray[1812 + n] = 47;
                byArray[1813 + n] = 42;
                byArray[1814 + n] = 55;
                byArray[2100 + n] = 36;
                byArray[2101 + n] = 18;
                byArray[2102 + n] = 9;
                byArray[2103 + n] = 9;
                byArray[2104 + n] = 18;
                byArray[2105 + n] = 28;
                byArray[2106 + n] = 42;
                byArray[2107 + n] = 48;
                byArray[2108 + n] = 41;
                byArray[2109 + n] = 36;
                byArray[2110 + n] = 37;
                byArray[2111 + n] = 41;
                byArray[2112 + n] = 34;
                byArray[2113 + n] = 34;
                byArray[2114 + n] = 46;
                byArray[2400 + n] = 42;
                byArray[2401 + n] = 25;
                byArray[2402 + n] = 18;
                byArray[2403 + n] = 18;
                byArray[2404 + n] = 24;
                byArray[2405 + n] = 34;
                byArray[2406 + n] = 44;
                byArray[2407 + n] = 41;
                byArray[2408 + n] = 42;
                byArray[2409 + n] = 48;
                byArray[2410 + n] = 46;
                byArray[2411 + n] = 37;
                byArray[2412 + n] = 31;
                byArray[2413 + n] = 36;
                byArray[2414 + n] = 55;
                byArray[2700 + n] = 55;
                byArray[2701 + n] = 34;
                byArray[2702 + n] = 26;
                byArray[2703 + n] = 24;
                byArray[2704 + n] = 28;
                byArray[2705 + n] = 41;
                byArray[2706 + n] = 41;
                byArray[2707 + n] = 32;
                byArray[2708 + n] = 36;
                byArray[2709 + n] = 42;
                byArray[2710 + n] = 41;
                byArray[2711 + n] = 32;
                byArray[2712 + n] = 31;
                byArray[2713 + n] = 52;
                byArray[2714 + n] = 63;
                byArray[3000 + n] = 63;
                byArray[3001 + n] = 48;
                byArray[3002 + n] = 34;
                byArray[3003 + n] = 34;
                byArray[3004 + n] = 41;
                byArray[3005 + n] = 48;
                byArray[3006 + n] = 42;
                byArray[3007 + n] = 31;
                byArray[3008 + n] = 36;
                byArray[3009 + n] = 40;
                byArray[3010 + n] = 31;
                byArray[3011 + n] = 26;
                byArray[3012 + n] = 31;
                byArray[3013 + n] = 51;
                byArray[3301 + n] = 63;
                byArray[3302 + n] = 46;
                byArray[3303 + n] = 42;
                byArray[3304 + n] = 48;
                byArray[3305 + n] = 48;
                byArray[3306 + n] = 41;
                byArray[3307 + n] = 36;
                byArray[3308 + n] = 36;
                byArray[3309 + n] = 31;
                byArray[3310 + n] = 26;
                byArray[3311 + n] = 27;
                byArray[3312 + n] = 41;
                byArray[3313 + n] = 63;
                byArray[3602 + n] = 62;
                byArray[3603 + n] = 46;
                byArray[3604 + n] = 42;
                byArray[3605 + n] = 42;
                byArray[3606 + n] = 41;
                byArray[3607 + n] = 37;
                byArray[3608 + n] = 34;
                byArray[3609 + n] = 25;
                byArray[3610 + n] = 27;
                byArray[3611 + n] = 41;
                byArray[3612 + n] = 62;
                byArray[3903 + n] = 63;
                byArray[3904 + n] = 54;
                byArray[3905 + n] = 42;
                byArray[3906 + n] = 37;
                byArray[3907 + n] = 33;
                byArray[3908 + n] = 31;
                byArray[3909 + n] = 34;
                byArray[3910 + n] = 46;
                byArray[3911 + n] = 63;
                byArray[4205 + n] = 55;
                byArray[4206 + n] = 55;
                byArray[4207 + n] = 44;
                byArray[4208 + n] = 44;
                byArray[4209 + n] = 55;
                return;
            }
            case 6: {
                byArray[5 + n] = 63;
                byArray[6 + n] = 63;
                byArray[7 + n] = 48;
                byArray[8 + n] = 63;
                byArray[9 + n] = 63;
                byArray[303 + n] = 63;
                byArray[304 + n] = 59;
                byArray[305 + n] = 48;
                byArray[306 + n] = 37;
                byArray[307 + n] = 34;
                byArray[308 + n] = 42;
                byArray[309 + n] = 53;
                byArray[310 + n] = 59;
                byArray[311 + n] = 63;
                byArray[602 + n] = 63;
                byArray[603 + n] = 46;
                byArray[604 + n] = 34;
                byArray[605 + n] = 26;
                byArray[606 + n] = 21;
                byArray[607 + n] = 26;
                byArray[608 + n] = 34;
                byArray[609 + n] = 42;
                byArray[610 + n] = 44;
                byArray[611 + n] = 52;
                byArray[612 + n] = 63;
                byArray[901 + n] = 63;
                byArray[902 + n] = 41;
                byArray[903 + n] = 28;
                byArray[904 + n] = 21;
                byArray[905 + n] = 15;
                byArray[906 + n] = 21;
                byArray[907 + n] = 34;
                byArray[908 + n] = 44;
                byArray[909 + n] = 52;
                byArray[910 + n] = 48;
                byArray[911 + n] = 48;
                byArray[912 + n] = 52;
                byArray[913 + n] = 63;
                byArray[1200 + n] = 63;
                byArray[1201 + n] = 48;
                byArray[1202 + n] = 27;
                byArray[1203 + n] = 21;
                byArray[1204 + n] = 15;
                byArray[1205 + n] = 15;
                byArray[1206 + n] = 28;
                byArray[1207 + n] = 42;
                byArray[1208 + n] = 53;
                byArray[1209 + n] = 52;
                byArray[1210 + n] = 52;
                byArray[1211 + n] = 53;
                byArray[1212 + n] = 48;
                byArray[1213 + n] = 63;
                byArray[1500 + n] = 59;
                byArray[1501 + n] = 34;
                byArray[1502 + n] = 21;
                byArray[1503 + n] = 15;
                byArray[1504 + n] = 15;
                byArray[1505 + n] = 21;
                byArray[1506 + n] = 34;
                byArray[1507 + n] = 48;
                byArray[1508 + n] = 52;
                byArray[1509 + n] = 48;
                byArray[1510 + n] = 44;
                byArray[1511 + n] = 52;
                byArray[1512 + n] = 48;
                byArray[1513 + n] = 56;
                byArray[1514 + n] = 63;
                byArray[1800 + n] = 52;
                byArray[1801 + n] = 28;
                byArray[1802 + n] = 15;
                byArray[1803 + n] = 15;
                byArray[1804 + n] = 15;
                byArray[1805 + n] = 26;
                byArray[1806 + n] = 41;
                byArray[1807 + n] = 49;
                byArray[1808 + n] = 48;
                byArray[1809 + n] = 36;
                byArray[1810 + n] = 37;
                byArray[1811 + n] = 48;
                byArray[1812 + n] = 48;
                byArray[1813 + n] = 48;
                byArray[1814 + n] = 59;
                byArray[2100 + n] = 41;
                byArray[2101 + n] = 21;
                byArray[2102 + n] = 15;
                byArray[2103 + n] = 15;
                byArray[2104 + n] = 24;
                byArray[2105 + n] = 31;
                byArray[2106 + n] = 48;
                byArray[2107 + n] = 52;
                byArray[2108 + n] = 44;
                byArray[2109 + n] = 41;
                byArray[2110 + n] = 42;
                byArray[2111 + n] = 44;
                byArray[2112 + n] = 37;
                byArray[2113 + n] = 36;
                byArray[2114 + n] = 52;
                byArray[2400 + n] = 48;
                byArray[2401 + n] = 28;
                byArray[2402 + n] = 21;
                byArray[2403 + n] = 24;
                byArray[2404 + n] = 26;
                byArray[2405 + n] = 37;
                byArray[2406 + n] = 48;
                byArray[2407 + n] = 44;
                byArray[2408 + n] = 48;
                byArray[2409 + n] = 52;
                byArray[2410 + n] = 49;
                byArray[2411 + n] = 42;
                byArray[2412 + n] = 34;
                byArray[2413 + n] = 41;
                byArray[2414 + n] = 59;
                byArray[2700 + n] = 59;
                byArray[2701 + n] = 37;
                byArray[2702 + n] = 28;
                byArray[2703 + n] = 26;
                byArray[2704 + n] = 31;
                byArray[2705 + n] = 44;
                byArray[2706 + n] = 46;
                byArray[2707 + n] = 34;
                byArray[2708 + n] = 41;
                byArray[2709 + n] = 48;
                byArray[2710 + n] = 46;
                byArray[2711 + n] = 34;
                byArray[2712 + n] = 34;
                byArray[2713 + n] = 55;
                byArray[2714 + n] = 63;
                byArray[3000 + n] = 63;
                byArray[3001 + n] = 53;
                byArray[3002 + n] = 37;
                byArray[3003 + n] = 37;
                byArray[3004 + n] = 46;
                byArray[3005 + n] = 53;
                byArray[3006 + n] = 44;
                byArray[3007 + n] = 34;
                byArray[3008 + n] = 41;
                byArray[3009 + n] = 42;
                byArray[3010 + n] = 34;
                byArray[3011 + n] = 28;
                byArray[3012 + n] = 34;
                byArray[3013 + n] = 56;
                byArray[3301 + n] = 63;
                byArray[3302 + n] = 48;
                byArray[3303 + n] = 48;
                byArray[3304 + n] = 53;
                byArray[3305 + n] = 52;
                byArray[3306 + n] = 44;
                byArray[3307 + n] = 41;
                byArray[3308 + n] = 41;
                byArray[3309 + n] = 34;
                byArray[3310 + n] = 28;
                byArray[3311 + n] = 31;
                byArray[3312 + n] = 42;
                byArray[3313 + n] = 63;
                byArray[3602 + n] = 63;
                byArray[3603 + n] = 49;
                byArray[3604 + n] = 48;
                byArray[3605 + n] = 48;
                byArray[3606 + n] = 44;
                byArray[3607 + n] = 42;
                byArray[3608 + n] = 36;
                byArray[3609 + n] = 28;
                byArray[3610 + n] = 31;
                byArray[3611 + n] = 48;
                byArray[3612 + n] = 63;
                byArray[3903 + n] = 63;
                byArray[3904 + n] = 55;
                byArray[3905 + n] = 48;
                byArray[3906 + n] = 41;
                byArray[3907 + n] = 37;
                byArray[3908 + n] = 34;
                byArray[3909 + n] = 37;
                byArray[3910 + n] = 49;
                byArray[3911 + n] = 63;
                byArray[4205 + n] = 59;
                byArray[4206 + n] = 59;
                byArray[4207 + n] = 48;
                byArray[4208 + n] = 49;
                byArray[4209 + n] = 59;
                return;
            }
        }
    }
}

