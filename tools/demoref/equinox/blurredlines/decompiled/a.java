/*
 * Decompiled with CFR 0.152.
 */
public class a {
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
    public int[] bq;
    public int[] bp;
    public int[] bo;
    public int[] bn;
    public int[] bm;
    public int[] bl;
    public int[] bk;
    public int[] bj;
    public int[] bi;
    public int[] bh;
    public int[] bg;
    public int[] bf;
    public int[] be;
    public int[] bd;
    public int[] bc;
    public int ba;
    public int[] a9;
    public float[] a8;
    public int a7;
    public float a6;
    public int a5;
    public float a4;
    public float a3;
    public float a2;
    public float a1;
    public int a0;
    public float a_;
    public int az;
    public int ay;
    public int ax;
    public int aw;
    public int m;
    public int av;
    public static int au;
    public int at;
    public int as;
    public int ar;
    public int aq;
    public int ap;
    public int i;
    public int h;
    public double[][] ao = new double[4][10];
    public int l;
    public float an;
    public float am;
    public float al;
    public float ak;
    public float aj;
    public float ai;
    public float ah;
    public float ag;
    public float af;

    /*
     * Enabled aggressive block sorting
     */
    public final void u(int n, int n2, int n3) {
        au = n;
        this.bh = new int[au];
        this.at = 0;
        while (this.at < 5) {
            ++this.i;
            int cfr_ignored_0 = this.at >> 4;
            this.as = 0;
            while (this.as < 5) {
                this.ar = 0;
                while (this.ar < 5) {
                    this.bh[this.at + this.as * 2 + this.ar * 3] = this.at >> 2 + this.as >> 4 + this.ar;
                    ++this.ar;
                }
                this.h = this.ar - 4;
                ++this.as;
            }
            ++this.at;
        }
        switch (this.h) {
            case 0: {
                this.h();
                this.ao[2][7] = 0.0;
                this.i = 1;
                break;
            }
            case 1: {
                this.h();
                int[] cfr_ignored_1 = new int[1600];
                this.be = new int[1600];
                int[] cfr_ignored_2 = new int[au + 20];
                break;
            }
        }
        this.at = 0;
        while (this.at < 10) {
            ++this.i;
            this.as = 0;
            while (this.as < 10) {
                this.ar = 0;
                while (this.ar < 10) {
                    this.be[this.at + this.as * 10 + this.ar * 20] = (byte)(this.at - this.as + this.ar);
                    ++this.ar;
                }
                ++this.as;
            }
            ++this.at;
        }
        this.j();
        this.i();
        this.l();
        int n4 = 0;
        while (n4 < 1600) {
            this.be[n4] = 0;
            ++n4;
        }
        this.a6 = 0.0f;
        this.a1 = 0.0f;
        this.a3 = 0.0f;
        this.a0 = 0;
        this.l = this.at - 8;
        this.a7 = 0;
    }

    public final void t() {
        ++this.a7;
        this.a5 = -1100;
        ++this.i;
        this.k();
        this.o();
        if (this.a7 < this.bc[0] + 1) {
            this.q(this.ca, this.b9, this.b8);
            if (this.h != 1) {
                this.i = 12345678;
            }
        } else if (this.a7 > this.bc[0] && this.a7 < this.bc[1] + 1) {
            this.p(this.ca, this.b9, this.b8, this.b7, this.b6, this.b5, 0);
        } else if (this.a7 > this.bc[1] && this.a7 < this.bc[2] + 1) {
            this.q(this.b7, this.b6, this.b5);
        } else if (this.a7 > this.bc[2] && this.a7 < this.bc[3] + 1) {
            this.p(this.b7, this.b6, this.b5, this.b4, this.b3, this.b2, 1);
        } else if (this.a7 > this.bc[3] && this.a7 < this.bc[4] + 1) {
            this.q(this.b4, this.b3, this.b2);
        } else if (this.a7 > this.bc[4] && this.a7 < this.bc[5] + 1) {
            this.p(this.b4, this.b3, this.b2, this.b1, this.b0, this.b_, 2);
        } else if (this.a7 > this.bc[5] && this.a7 < this.bc[6] + 1) {
            this.q(this.b1, this.b0, this.b_);
        } else if (this.a7 > this.bc[6] && this.a7 < this.bc[7] + 1) {
            this.p(this.b1, this.b0, this.b_, this.bz, this.by, this.bx, 3);
        } else if (this.a7 > this.bc[7] && this.a7 < this.bc[8] + 1) {
            this.q(this.bz, this.by, this.bx);
        } else if (this.a7 > this.bc[8] && this.a7 < this.bc[9] + 1) {
            this.p(this.bz, this.by, this.bx, this.bw, this.bv, this.bu, 4);
        } else if (this.a7 > this.bc[9] && this.a7 < this.bc[10] + 1) {
            this.q(this.bw, this.bv, this.bu);
        } else if (this.a7 > this.bc[10] && this.a7 < this.bc[11] + 1) {
            this.p(this.bw, this.bv, this.bu, this.bt, this.bs, this.br, 5);
        } else if (this.a7 > this.bc[11] && this.a7 < this.bc[12] + 1) {
            this.q(this.bt, this.bs, this.br);
        } else if (this.a7 > this.bc[12] && this.a7 < this.bc[13] + 1) {
            this.p(this.bt, this.bs, this.br, this.bq, this.bp, this.bo, 6);
        } else if (this.a7 > this.bc[13] && this.a7 < this.bc[14] + 1) {
            this.q(this.bq, this.bp, this.bo);
        } else if (this.a7 > this.bc[14] && this.a7 < this.bc[15] + 1) {
            this.p(this.bq, this.bp, this.bo, this.bn, this.bm, this.bl, 7);
        } else if (this.a7 > this.bc[15] && this.a7 < this.bc[16] + 1) {
            this.q(this.bn, this.bm, this.bl);
        } else if (this.a7 > this.bc[16] && this.a7 < this.bc[17] + 1) {
            this.p(this.bn, this.bm, this.bl, this.bk, this.bj, this.bi, 8);
        } else if (this.a7 > this.bc[17] && this.a7 < this.bc[18]) {
            this.q(this.bk, this.bj, this.bi);
        } else if (this.a7 > this.bc[18] && this.a7 < this.bc[19] + 1) {
            this.p(this.bk, this.bj, this.bi, this.ca, this.b9, this.b8, 9);
        } else if (this.a7 > this.bc[19] && this.a7 < this.bc[20]) {
            this.q(this.ca, this.b9, this.b8);
        }
        if (this.a7 >= this.bc[20] - 1) {
            this.a7 = 0;
        }
    }

    public final void s(byte[] byArray) {
        this.t();
        this.r(byArray, 250, 250, this.bh, this.bg, this.bf);
    }

    /*
     * Unable to fully structure code
     */
    public final void r(byte[] var1_1, int var2_2, int var3_3, int[] var4_4, int[] var5_5, int[] var6_6) {
        var23_7 = (int)((double)this.a5 - (double)this.ap * 1.5);
        var24_8 = (int)((double)this.a5 + (double)this.ap * 1.5);
        var10_9 = var2_2 >> 1;
        var11_10 = var3_3 >> 1;
        var25_11 = a.au - 1;
        while (var25_11 >= 0) {
            var7_12 = var6_6[var25_11];
            var22_15 = (float)(300.0 / (300.0 - (double)var7_12));
            var8_13 = (int)((float)var4_4[var25_11] * var22_15 + 125.0f);
            var9_14 = (int)((float)var5_5[var25_11] * var22_15 + 125.0f);
            if (var8_13 < 1) {
                var8_13 = 1;
            } else if (var8_13 > 248) {
                var8_13 = 248;
            }
            if (var9_14 < 1) {
                var9_14 = 1;
            } else if (var9_14 > 248) {
                var9_14 = 248;
            }
            var4_4[var25_11] = var8_13;
            var5_5[var25_11] = var9_14;
            --var25_11;
        }
        this.i = 400;
        if (this.l > 2) {
            this.i = 314;
        }
        var25_11 = 0;
        ** GOTO lbl116
lbl-1000:
        // 1 sources

        {
            var18_22 = var4_4[var25_11];
            var19_23 = var5_5[var25_11];
            var20_24 = var4_4[var25_11 + 1];
            var21_25 = var5_5[var25_11 + 1];
            if (Math.abs(var20_24 - var18_22) > Math.abs(var21_25 - var19_23)) {
                if (var20_24 < var18_22) {
                    var13_17 = var18_22 - var20_24;
                    var14_18 = var21_25 << 16;
                    var15_19 = var19_23 << 16;
                    var12_16 = var13_17 == 0 ? 0 : (var15_19 - var14_18) / var13_17;
                    var16_20 = var20_24;
                    var17_21 = var18_22;
                } else {
                    var13_17 = var20_24 - var18_22;
                    var14_18 = var19_23 << 16;
                    var15_19 = var21_25 << 16;
                    var12_16 = var13_17 == 0 ? 0 : (var15_19 - var14_18) / var13_17;
                    var16_20 = var18_22;
                    var17_21 = var20_24;
                }
                var13_17 >>= 1;
                while (var13_17 > 0) {
                    var1_1[var16_20++ + (var14_18 >> 16) * 250] = -1;
                    var1_1[var17_21-- + (var15_19 >> 16) * 250] = -1;
                    var14_18 += var12_16;
                    var15_19 -= var12_16;
                    --var13_17;
                }
                var1_1[var16_20 + (var14_18 >> 16) * 250] = -1;
                var1_1[var17_21 + (var15_19 >> 16) * 250] = -1;
            } else {
                if (var21_25 < var19_23) {
                    var13_17 = var19_23 - var21_25;
                    var14_18 = var20_24 << 16;
                    var15_19 = var18_22 << 16;
                    var12_16 = var13_17 == 0 ? 0 : (var15_19 - var14_18) / var13_17;
                    var16_20 = var21_25 * var2_2;
                    var17_21 = var19_23 * var2_2;
                } else {
                    var13_17 = var21_25 - var19_23;
                    var14_18 = var18_22 << 16;
                    var15_19 = var20_24 << 16;
                    var12_16 = var13_17 == 0 ? 0 : (var15_19 - var14_18) / var13_17;
                    var16_20 = var19_23 * var2_2;
                    var17_21 = var21_25 * var2_2;
                }
                var13_17 >>= 1;
                while (var13_17 > 0) {
                    var1_1[var16_20 + (var14_18 >> 16)] = -1;
                    var1_1[var17_21 + (var15_19 >> 16)] = -1;
                    var14_18 += var12_16;
                    var15_19 -= var12_16;
                    var16_20 += var2_2;
                    var17_21 -= var2_2;
                    --var13_17;
                }
                var1_1[var16_20 + (var14_18 >> 16)] = -1;
                var1_1[var17_21 + (var15_19 >> 16)] = -1;
            }
            ++var25_11;
lbl116:
            // 2 sources

            ** while (var25_11 < a.au - 1)
        }
lbl117:
        // 1 sources

    }

    private final void q(int[] nArray, int[] nArray2, int[] nArray3) {
        int n = au - 1;
        while (n >= 0) {
            int n2 = nArray[n];
            int n3 = nArray2[n];
            int n4 = nArray3[n];
            this.bh[n] = (int)(this.an * (float)n2 + this.am * (float)n3 + this.al * (float)n4);
            this.bg[n] = (int)(this.ak * (float)n2 + this.aj * (float)n3 + this.ai * (float)n4 + (float)this.a0);
            this.bf[n] = (int)(this.ah * (float)n2 + this.ag * (float)n3 + this.af * (float)n4 + (float)this.a5);
            --n;
        }
    }

    private final void p(int[] nArray, int[] nArray2, int[] nArray3, int[] nArray4, int[] nArray5, int[] nArray6, int n) {
        n = this.a7 - this.bc[n * 2];
        float f = (float)((double)n / 100.0);
        int n2 = 0;
        while (n2 < 5) {
            this.ao[1][n2] = Math.cos(0.02f * (float)n2);
            ++n2;
        }
        int n3 = au - 1;
        while (n3 >= 0) {
            int n4 = nArray[n3];
            int n5 = nArray2[n3];
            int n6 = nArray3[n3];
            this.aw = (int)((float)n4 + (float)(nArray4[n3] - n4) * f);
            this.m = (int)((float)n5 + (float)(nArray5[n3] - n5) * f);
            this.av = (int)((float)n6 + (float)(nArray6[n3] - n6) * f);
            this.bh[n3] = (int)(this.an * (float)this.aw + this.am * (float)this.m + this.al * (float)this.av);
            this.bg[n3] = (int)(this.ak * (float)this.aw + this.aj * (float)this.m + this.ai * (float)this.av + (float)this.a0);
            this.bf[n3] = (int)(this.ah * (float)this.aw + this.ag * (float)this.m + this.af * (float)this.av + (float)this.a5);
            --n3;
        }
    }

    private final void o() {
        float f = this.n(this.a3);
        float f2 = this.m(this.a3);
        float f3 = this.n(this.a1);
        float f4 = this.m(this.a1);
        float f5 = this.n(this.a6);
        float f6 = this.m(this.a6);
        this.an = f * f3;
        this.am = -(f * f4 * f6 + f2 * f5);
        this.al = -(f * f4 * f5 - f2 * f6);
        this.ak = f2 * f3;
        this.aj = -(f2 * f4 * f6 - f * f5);
        this.ai = -(f2 * f4 * f5 + f * f6);
        this.ah = f4;
        int n = 0;
        while (n < 5) {
            this.ao[1][n] = Math.log(0.02f * (float)n);
            ++n;
        }
        this.ag = f3 * f6;
        this.af = f3 * f5;
    }

    private final float n(float f) {
        return (float)Math.cos(f);
    }

    private final float m(float f) {
        return (float)Math.sin(f);
    }

    private final void l() {
        this.ax = 8;
        this.a8 = new float[this.ax * 3];
        this.a9 = new int[this.ax + 1];
        this.a9[0] = 150;
        this.a8[0] = 0.333333f;
        this.a8[1] = 0.666667f;
        this.a8[2] = 0.5f;
        this.a9[1] = 100;
        this.a8[3] = 0.666667f;
        this.a8[4] = 0.666667f;
        this.a8[5] = 0.666667f;
        this.a9[2] = 100;
        this.a8[6] = 0.666667f;
        this.a8[7] = 1.0f;
        this.a8[8] = 0.666667f;
        this.a9[3] = 100;
        this.a8[9] = 0.666667f;
        this.a8[10] = 0.333333f;
        this.a8[11] = 0.666667f;
        this.a9[4] = 50;
        this.a8[12] = 0.666667f;
        this.a8[13] = 0.666667f;
        this.a8[14] = 1.333333f;
        this.a9[5] = 50;
        this.a8[15] = 0.666667f;
        this.a8[16] = 1.0f;
        this.a8[17] = 1.333333f;
        this.a9[6] = 150;
        this.a8[18] = 0.666667f;
        this.a8[19] = -0.666667f;
        this.a8[20] = 1.0f;
        this.a9[7] = 100;
        this.a8[21] = 0.666667f;
        this.a8[22] = -0.333333f;
        this.a8[23] = 0.666667f;
    }

    private final void k() {
        float f = 0.017453f;
        this.a2 = this.a8[3 * this.ay];
        this.a_ = this.a8[1 + 3 * this.ay];
        this.a4 = this.a8[2 + 3 * this.ay];
        this.a3 += f * this.a2;
        this.a1 += f * this.a_;
        this.a6 += f * this.a4;
        ++this.az;
        if (this.az >= this.a9[this.ay]) {
            this.az = 0;
            ++this.ay;
            if (this.ay >= this.ax) {
                this.ay = 0;
                this.aq = 0;
                while (this.aq < 10) {
                    this.ao[2][this.aq] = Math.exp((double)this.aq * (double)0.03f);
                    ++this.aq;
                }
            }
        }
    }

    private final void j() {
        int n = 0;
        this.ba = 11;
        this.bd = new int[this.ba * 2];
        this.bc = new int[this.ba * 2];
        this.bd[0] = 100;
        this.bd[1] = 100;
        this.bd[2] = 200;
        this.bd[3] = 100;
        this.bd[4] = 200;
        this.bd[5] = 100;
        this.bd[6] = 200;
        this.bd[7] = 100;
        this.bd[8] = 200;
        this.bd[9] = 100;
        this.bd[10] = 200;
        this.bd[11] = 100;
        this.bd[12] = 150;
        this.bd[13] = 100;
        this.bd[14] = 200;
        this.bd[15] = 100;
        this.bd[16] = 200;
        this.bd[17] = 100;
        this.bd[18] = 200;
        this.bd[19] = 100;
        this.bd[20] = 50;
        int n2 = 0;
        while (n2 < this.ba * 2 - 1) {
            this.bc[n2] = n += this.bd[n2];
            int n3 = 0;
            while (n3 < 10) {
                this.ao[2][n3] = Math.cos((double)n3 * (double)0.03f);
                ++n3;
            }
            ++n2;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void i() {
        this.ap = 400;
        this.aw = (int)Math.sqrt(au);
        int n = 0;
        while (n < au) {
            this.ca[n] = (int)(Math.sin((double)0.31415f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap * (double)1.2f);
            this.b8[n] = (int)(Math.cos((double)0.31415f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap * (double)1.2f);
            this.b9[n] = (int)(Math.sin((double)6.283f / (double)au * (double)n) * (double)this.ap / 2.0);
            float cfr_ignored_0 = this.b9[n];
            ++n;
        }
        this.aw = (int)Math.sqrt(au);
        n = 0;
        while (n < au) {
            this.b7[n] = (int)(Math.sin((double)0.31415f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.b5[n] = (int)(Math.cos((double)0.31415f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.b6[n] = (int)((double)(this.ap * 2 / au * n) * 1.5 - (double)this.ap * 1.5);
            float cfr_ignored_1 = this.b5[n];
            ++n;
        }
        this.aw = (int)Math.sqrt(au);
        Math.sqrt(au * 2);
        n = 0;
        while (n < au) {
            this.b4[n] = (int)(Math.sin((double)0.31415f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.b2[n] = (int)(Math.cos((double)0.30799f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.b3[n] = (int)((double)(this.ap * 2 / au * n) * 1.5 - (double)this.ap * 1.5);
            ++n;
        }
        n = 0;
        while (n < au) {
            this.b_[n] = (int)(Math.cos((double)0.069811f * (double)n) * Math.sin((double)0.10472f * (double)n) * (double)this.ap * (double)0.9f);
            this.b1[n] = (int)(Math.sin((double)0.089757f * (double)n) * Math.sin((double)0.062832f * (double)n) * (double)this.ap * (double)0.9f);
            this.b0[n] = (int)(Math.sin((double)0.125664f * (double)n) * (double)this.ap);
            ++n;
        }
        n = 0;
        while (n < au) {
            this.bz[n] = (int)(Math.cos((double)0.6283f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n) * (double)this.ap);
            this.bx[n] = (int)(Math.sin((double)0.6283f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n) * (double)this.ap);
            this.by[n] = this.ap * 2 / au * n - this.ap;
            ++n;
        }
        n = 0;
        while (n < au) {
            this.bw[n] = (int)(Math.sin((double)0.6283f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.bu[n] = (int)(Math.cos((double)0.6283f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.bv[n] = (int)((double)(this.ap * 2 / au * n) * 1.5 - (double)this.ap * 1.5);
            ++n;
        }
        n = 0;
        while (n < au) {
            this.bt[n] = (int)(Math.sin((double)0.61598f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.br[n] = (int)(Math.cos((double)0.61598f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n / 2.0) * (double)this.ap);
            this.bs[n] = (int)((double)(this.ap * 2 / au * n) * 1.5 - (double)this.ap * 1.5);
            ++n;
        }
        n = 0;
        while (n < au) {
            this.bq[n] = (int)(Math.cos((double)0.6283f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n * 1.5) * (double)this.ap);
            this.bo[n] = (int)(Math.sin((double)0.6283f * (double)n) * Math.sin((double)6.28318f / (double)au * (double)n * 1.5) * (double)this.ap);
            float cfr_ignored_2 = this.bq[n];
            this.bp[n] = this.ap * 2 / au * n - this.ap;
            ++n;
        }
        n = 0;
        while (n < au) {
            this.bl[n] = this.ap * n / au * 2 - this.ap;
            this.bn[n] = (int)(Math.cos((double)6.283f / (double)au * (double)n * 10.0) * (double)this.ap);
            this.bm[n] = (int)(Math.sin((double)6.283f / (double)au * (double)n * 10.0) * (double)this.ap);
            ++n;
        }
        n = 0;
        while (n < au) {
            this.bi[n] = (int)((double)this.ap * 1.5 * Math.random() - (double)this.ap * 0.75);
            this.bk[n] = (int)((double)this.ap * 1.5 * Math.random() - (double)this.ap * 0.75);
            this.bj[n] = (int)((double)this.ap * 1.5 * Math.random() - (double)this.ap * 0.75);
            ++n;
        }
    }

    private final void h() {
        this.bg = new int[au];
        this.bf = new int[au];
        this.ca = new int[au];
        this.b9 = new int[au];
        this.b8 = new int[au];
        this.b7 = new int[au];
        this.b6 = new int[au];
        this.b5 = new int[au];
        this.b4 = new int[au];
        this.b3 = new int[au];
        this.b2 = new int[au];
        this.b1 = new int[au];
        this.b0 = new int[au];
        this.b_ = new int[au];
        this.bz = new int[au];
        this.by = new int[au];
        this.bx = new int[au];
        this.bw = new int[au];
        this.bv = new int[au];
        this.bu = new int[au];
        this.bt = new int[au];
        this.bs = new int[au];
        this.br = new int[au];
        this.bq = new int[au];
        this.bp = new int[au];
        this.bo = new int[au];
        this.bn = new int[au];
        this.bm = new int[au];
        this.bl = new int[au];
        this.bk = new int[au];
        this.bj = new int[au];
        this.bi = new int[au];
        int[] cfr_ignored_0 = new int[au];
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
}

