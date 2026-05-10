/*
 * Decompiled with CFR 0.152.
 */
public final class a
implements Runnable {
    public int a1 = 0;
    public boolean a0 = false;
    public Thread a_ = null;
    public float ay;
    public float ax;
    public float aw;
    public float av;
    public float as;
    public float ap;
    public float am;
    public float aj;
    public float ai;
    public float ag;
    public float ae;
    public float ad;
    public float ac;
    public float ab;
    public float aa;
    public float c2;
    public int c1;
    public float c0;
    public float c_;
    public float cw;
    public float cv;
    public float cu;
    public float cq;
    public float co;
    public float cn;
    public float cm;
    public float cl;
    public float ck;
    public float cj;
    public float ci;
    public float ch;
    public float[] cg;
    public float[] cf;
    public float[] ce;
    public float[] cd;
    public float[] cc;
    public float[] ca;
    public float[] b9;
    public float[] b8;
    public float[] b7;
    public float[] b6;
    public float[] b5;
    public float[] b4;
    public float[] b3;
    public float[] b2;
    public float[] b1;
    public int b0;
    public int b_;
    public b a9;
    public boolean a8;
    public float a7;
    public float a6;
    public float a5;
    public float a4;
    public int a3;
    public static int a2;

    /*
     * Handled impossible loop by duplicating code
     * Enabled aggressive block sorting
     */
    public final void u(b b2, int n) {
        block11: {
            int n2;
            block10: {
                block9: {
                    block8: {
                        block7: {
                            block6: {
                                this.a1 = n;
                                this.a9 = b2;
                                this.cu = this.a9.aq;
                                this.cq = this.a9.ap;
                                this.co = this.a9.ao;
                                this.cn = this.a9.an;
                                this.cm = this.a9.am;
                                this.cl = this.a9.al;
                                this.a3 = this.a9.ai;
                                this.c_ = this.a9.av;
                                this.c0 = this.a9.au;
                                this.cw = this.a9.at;
                                this.cv = this.a9.as;
                                this.a4 = this.a9.ar;
                                this.cg = this.a9.a_;
                                this.cf = this.a9.az;
                                this.ce = this.a9.a0;
                                this.b0 = 0;
                                this.b_ = 0;
                                this.a8 = false;
                                n2 = 0;
                                if (!true) break block6;
                                a.u(",'*~");
                                if (n2 >= this.a1) break block7;
                            }
                            do {
                                this.b3[n2] = 0.0f;
                                ++n2;
                                a.u(",'*~");
                            } while (n2 < this.a1);
                        }
                        System.arraycopy(this.b3, 0, this.b2, 0, this.a1);
                        System.arraycopy(this.b3, 0, this.b1, 0, this.a1);
                        n2 = 0;
                        if (!true) break block8;
                        a.u(",'*~");
                        if (n2 >= this.a1 * 2) break block9;
                    }
                    do {
                        this.cd[n2] = this.cm;
                        ++n2;
                        a.u(",'*~");
                    } while (n2 < this.a1 * 2);
                }
                System.arraycopy(this.cd, 0, this.cc, 0, this.a1 * 2);
                System.arraycopy(this.cd, 0, this.ca, 0, this.a1 * 2);
                n2 = 0;
                if (!true) break block10;
                a.u(",'*~");
                if (n2 >= this.a1 * 2) break block11;
            }
            do {
                this.b9[n2] = 0.0f;
                ++n2;
                a.u(",'*~");
            } while (n2 < this.a1 * 2);
        }
        System.arraycopy(this.b9, 0, this.b6, 0, this.a1 * 2);
        System.arraycopy(this.b9, 0, this.b8, 0, this.a1 * 2);
        System.arraycopy(this.b9, 0, this.b5, 0, this.a1 * 2);
        System.arraycopy(this.b9, 0, this.b7, 0, this.a1 * 2);
        System.arraycopy(this.b9, 0, this.b4, 0, this.a1 * 2);
    }

    public final boolean t() {
        return this.a0;
    }

    public final synchronized void al() {
        a.u(",'*~");
        if (this.a_ != null) {
            return;
        }
        this.a0 = true;
        this.a_ = new Thread(this);
        this.a_.start();
    }

    public final synchronized void ak() {
        this.a_ = null;
    }

    /*
     * Enabled aggressive block sorting
     * Enabled unnecessary exception pruning
     * Enabled aggressive exception aggregation
     */
    public final void run() {
        a a2;
        while (!this.a8 & this.a_ != null) {
            try {
                Thread.yield();
                Thread.sleep(10L);
            }
            catch (InterruptedException interruptedException) {
                // empty catch block
            }
            a2 = this;
            synchronized (a2) {
                if (this.a_ == null) {
                    this.a0 = false;
                    return;
                }
            }
            this.u(this.b3, this.b2, this.b1);
            int n = (this.a9.af - this.a9.ae) / 2 * 3 + (this.a9.af - this.a9.ae) / 2 * this.a9.af * 3 + this.b_ * this.a9.af * 3;
            int n2 = this.a9.ae;
            int n3 = 0;
            while (n3 < n2) {
                this.a9.ah[n + n3 * 3] = (byte)(this.b3[n3] * 255.0f);
                this.a9.ah[n + n3 * 3 + 1] = (byte)(this.b2[n3] * 255.0f);
                this.a9.ah[n + n3 * 3 + 2] = (byte)(this.b1[n3] * 255.0f);
                ++n3;
            }
            this.a9.ac = ++this.b_;
            if (this.a9.ae == 0) continue;
            this.a9.ag = 100 * this.b_ / this.a9.ae;
        }
        if (this.a8) {
            this.a9.ag = 100;
            this.a9.ab = true;
        }
        a2 = this;
        synchronized (a2) {
            this.a_ = null;
            this.a0 = false;
            return;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void u(float[] fArray, float[] fArray2, float[] fArray3) {
        int n = 0;
        while (this.b0 < 3 || n < 2) {
            ++n;
            System.arraycopy(this.cc, 0, this.cd, 0, this.a9.ae * 2);
            System.arraycopy(this.ca, 0, this.cc, 0, this.a9.ae * 2);
            int n2 = 0;
            while (n2 < this.a9.ae * 2) {
                this.ca[n2] = this.cm;
                ++n2;
            }
            this.ck = (this.cq - this.cu) / (float)(this.a9.ae * 2);
            this.cj = (this.cn - this.co) / (float)(this.a9.ae * 2);
            this.ci = (this.cl - this.cm) / (float)this.a9.ak;
            this.ch = this.ci / (float)this.a9.aj;
            this.a6 = this.co + (float)this.b0 * this.cj;
            this.a7 = this.cu;
            if (this.a9.ad == 1) {
                this.aj();
            } else if (this.a9.ad == 2) {
                this.ai();
            } else if (this.a9.ad == 3) {
                this.ah();
            } else if (this.a9.ad == 4) {
                this.ag();
            } else if (this.a9.ad == 5) {
                this.af();
            } else if (this.a9.ad == 6) {
                this.ae();
            } else if (this.a9.ad == 7) {
                this.ad();
            } else if (this.a9.ad == 8) {
                this.ac();
            } else if (this.a9.ad == 9) {
                this.ab();
            } else if (this.a9.ad == 10) {
                this.aa();
            } else if (this.a9.ad == 11) {
                this.z();
            } else if (this.a9.ad == 12) {
                this.y();
            } else if (this.a9.ad == 13) {
                this.x();
            } else if (this.a9.ad == 14) {
                this.w();
            } else if (this.a9.ad == 15) {
                this.v();
            }
            if (this.b0 > 0) {
                this.u(this.b0 - 1);
            }
            n2 = this.b0 - 2;
            int n3 = 0;
            while (n3 < this.a9.ae) {
                int n4 = n3 * 2;
                fArray[n3] = (this.b9[n4] + this.b9[n4 + 1] + this.b6[n4] + this.b6[n4 + 1]) / 4.0f;
                fArray2[n3] = (this.b8[n4] + this.b8[n4 + 1] + this.b5[n4] + this.b5[n4 + 1]) / 4.0f;
                fArray3[n3] = (this.b7[n4] + this.b7[n4 + 1] + this.b4[n4] + this.b4[n4 + 1]) / 4.0f;
                ++n3;
            }
            ++this.b0;
        }
        if (this.b0 == this.a9.ae * 2 - 1) {
            this.a8 = true;
        }
    }

    /*
     * Unable to fully structure code
     */
    private final void u(int var1_1) {
        block15: {
            block14: {
                var4_2 = a.a2;
                System.arraycopy(this.b6, 0, this.b9, 0, this.a9.ae * 2);
                System.arraycopy(this.b5, 0, this.b8, 0, this.a9.ae * 2);
                System.arraycopy(this.b4, 0, this.b7, 0, this.a9.ae * 2);
                var2_3 = 0;
                if (var4_2 == 0) ** GOTO lbl13
                block0: while (true) {
                    v0 = this;
                    do {
                        v0.b6[var2_3] = 0.0f;
                        ++var2_3;
lbl13:
                        // 2 sources

                        if (var2_3 < this.a9.ae * 2) continue block0;
                        System.arraycopy(this.b6, 0, this.b5, 0, this.a9.ae * 2);
                        System.arraycopy(this.b6, 0, this.b4, 0, this.a9.ae * 2);
                        this.ai = 2.0f * this.ck;
                        this.ae = 2.0f * this.cj;
                        this.c2 = this.ai * this.ae;
                        v0 = this;
                    } while (var4_2 != 0);
                    break;
                }
                if ((v0.b_ + (this.a9.af - this.a9.ae) / 2) % 2 != 1) break block14;
                this.b6[0] = 0.301961f;
                this.b5[0] = 0.360784f;
                this.b4[0] = 0.741176f;
                this.b6[this.a9.ae * 2 - 1] = 0.301961f;
                this.b5[this.a9.ae * 2 - 1] = 0.360784f;
                this.b4[this.a9.ae * 2 - 1] = 0.741176f;
                if (var4_2 == 0) break block15;
            }
            this.b6[0] = 0.0f;
            this.b5[0] = 0.0f;
            this.b4[0] = 0.0f;
            this.b6[this.a9.ae * 2 - 1] = 0.0f;
            this.b5[this.a9.ae * 2 - 1] = 0.0f;
            this.b4[this.a9.ae * 2 - 1] = 0.0f;
        }
        var2_3 = 1;
        if (var4_2 == 0) ** GOTO lbl86
        block2: while (true) {
            block16: {
                if (this.cc[var2_3] == this.cm) break block16;
                this.aj = this.cc[var2_3 - 1];
                this.am = this.cc[var2_3 + 1];
                this.ap = this.cd[var2_3];
                this.as = this.ca[var2_3];
                this.ag = this.am - this.aj;
                this.ad = this.as - this.ap;
                this.ab = -this.ae * this.ag;
                this.aa = -this.ad * this.ai;
                this.ax = 0.0f;
                this.aw = 0.0f;
                this.av = 0.0f;
                this.ay = (float)Math.sqrt(this.ab * this.ab + this.aa * this.aa + this.c2 * this.c2);
                this.c1 = this.a9.a1 - 1;
                if (var4_2 == 0) ** GOTO lbl64
                block3: while (true) {
                    this.ac = Math.max(0.0f, (this.ab * this.cg[this.c1] + this.aa * this.cf[this.c1] + this.c2 * this.ce[this.c1]) / this.ay);
                    this.ac *= this.ac;
                    this.ac *= this.ac + 1.0f;
                    this.ax += this.ac * this.a9.ay[this.c1];
                    this.aw += this.ac * this.a9.ax[this.c1];
                    this.av += this.ac * this.a9.aw[this.c1];
                    do {
                        --this.c1;
lbl64:
                        // 2 sources

                        if (this.c1 >= 0) continue block3;
                        this.ax = Math.max(0.0f, Math.min(1.0f, this.ax));
                        this.aw = Math.max(0.0f, Math.min(1.0f, this.aw));
                        this.av = Math.max(0.0f, Math.min(1.0f, this.av));
                        this.b6[var2_3] = this.ax;
                        this.b5[var2_3] = this.aw;
                        this.b4[var2_3] = this.av;
                    } while (var4_2 != 0 && var4_2 != 0);
                    break;
                }
                ** GOTO lbl84
            }
            do {
                block18: {
                    block17: {
                        if ((this.b_ + (this.a9.af - this.a9.ae) / 2) % 2 != 1) break block17;
                        this.b6[var2_3] = 0.301961f;
                        this.b5[var2_3] = 0.360784f;
                        this.b4[var2_3] = 0.741176f;
                        if (var4_2 == 0) break block18;
                    }
                    this.b6[var2_3] = 0.0f;
                    this.b5[var2_3] = 0.0f;
                    this.b4[var2_3] = 0.0f;
                }
                ++var2_3;
lbl86:
                // 2 sources

                if (var2_3 < this.a9.ae * 2 - 1) continue block2;
                var2_3 = 1;
            } while (var4_2 != 0);
            break;
        }
        if (var4_2 == 0) ** GOTO lbl96
        do {
            if ((double)(var3_4 = Math.abs(this.b6[var2_3 - 1] - this.b6[var2_3]) + Math.abs(this.b6[var2_3] - this.b6[var2_3 + 1]) + Math.abs(this.b5[var2_3 - 1] - this.b5[var2_3]) + Math.abs(this.b5[var2_3] - this.b5[var2_3 + 1]) + Math.abs(this.b4[var2_3 - 1] - this.b4[var2_3]) + Math.abs(this.b4[var2_3] - this.b4[var2_3 + 1])) > 0.4000000059604645) {
                this.b6[var2_3] = (this.b6[var2_3 - 1] + this.b6[var2_3] + this.b6[var2_3 + 1]) / 3.0f;
                this.b5[var2_3] = (this.b5[var2_3 - 1] + this.b5[var2_3] + this.b5[var2_3 + 1]) / 3.0f;
                this.b4[var2_3] = (this.b4[var2_3 - 1] + this.b4[var2_3] + this.b4[var2_3 + 1]) / 3.0f;
            }
            ++var2_3;
lbl96:
            // 2 sources

        } while (var2_3 < this.a9.ae * 2 - 1);
    }

    /*
     * Unable to fully structure code
     */
    private final void aj() {
        v0 = this.a9.ae * 2;
        a.u(",'*~");
        var1_1 = v0;
        var11_2 = 0;
        if (true) ** GOTO lbl54
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6;
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6;
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl54:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ai() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6;
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = -var9_10 * var6_7 - this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 - this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6;
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = -var9_10 * var6_7 - this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 - this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ah() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)(Math.cos((var5_6 * var6_7 + var7_8 + var8_9) * 3.0f) * Math.cos((var5_6 + var6_7 * var7_8 + var8_9) * 2.0f));
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)(Math.cos((var5_6 * var6_7 + var7_8 + var8_9) * 3.0f) * Math.cos((var5_6 + var6_7 * var7_8 + var8_9) * 2.0f));
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ag() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void af() {
        var1_1 = this.a9.ae * 2;
        var15_2 = 0;
        if (true) ** GOTO lbl57
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var11_11 = var5_6;
                    var12_12 = var6_7;
                    var13_13 = var7_8;
                    var14_14 = var8_9;
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                    var6_7 = 2.0f * (var11_11 * var12_12 - var13_13 * var14_14) + this.c0;
                    var7_8 = 2.0f * (var11_11 * var13_13 - var12_12 * var14_14) + this.cw;
                    var8_9 = 2.0f * (var11_11 * var14_14 + var13_13 * var12_12) + this.cv;
                    var10_10 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_10 < 0.0f);
                if (var10_10 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var11_11 = var5_6;
                            var12_12 = var6_7;
                            var13_13 = var7_8;
                            var14_14 = var8_9;
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                            var6_7 = 2.0f * (var11_11 * var12_12 - var13_13 * var14_14) + this.c0;
                            var7_8 = 2.0f * (var11_11 * var13_13 - var12_12 * var14_14) + this.cw;
                            var8_9 = 2.0f * (var11_11 * var14_14 + var13_13 * var12_12) + this.cv;
                            var10_10 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_10 < 0.0f);
                        if (var10_10 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var15_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var15_2;
lbl57:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ae() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ad() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6;
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0 + (float)(Math.cos((var5_6 * var6_7 + var7_8 + var8_9) * 1.0f) * Math.cos((var5_6 + var6_7 + var7_8 + var8_9) * 2.0f) / 2.0);
                    var7_8 = var9_10 * var7_8 + this.cw + (float)(Math.sin((var5_6 * var7_8 + var6_7 + var8_9) * 1.0f) * Math.cos((var5_6 - var6_7 * var7_8 + var8_9) * 2.0f) / 2.0);
                    var8_9 = var9_10 * var8_9 + this.cv + (float)(Math.cos((var7_8 * var6_7 + var5_6 + var8_9) * 1.0f) * Math.sin((var5_6 * var6_7 - var7_8 + var8_9) * 2.0f) / 2.0);
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6;
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0 + (float)(Math.cos((var5_6 * var6_7 + var7_8 + var8_9) * 1.0f) * Math.cos((var5_6 + var6_7 + var7_8 + var8_9) * 2.0f) / 2.0);
                            var7_8 = var9_10 * var7_8 + this.cw + (float)(Math.sin((var5_6 * var7_8 + var6_7 + var8_9) * 1.0f) * Math.cos((var5_6 - var6_7 * var7_8 + var8_9) * 2.0f) / 2.0);
                            var8_9 = var9_10 * var8_9 + this.cv + (float)(Math.cos((var7_8 * var6_7 + var5_6 + var8_9) * 1.0f) * Math.sin((var5_6 * var6_7 - var7_8 + var8_9) * 2.0f) / 2.0);
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ac() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)(Math.atan(var5_6 + var6_7 + var7_8 + var8_9) * Math.cos(var5_6 - var6_7 * var7_8 + var8_9) - Math.sin(var5_6 * var7_8 + var6_7 + var8_9));
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)(Math.atan(var5_6 + var6_7 + var7_8 + var8_9) * Math.cos(var5_6 - var6_7 * var7_8 + var8_9) - Math.sin(var5_6 * var7_8 + var6_7 + var8_9));
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void ab() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 * var6_7 * var7_8 + var8_9) * 2.0f);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 * var6_7 * var7_8 + var8_9) * 2.0f);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void aa() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 * var6_7 * var7_8 + var8_9) * 2.0f);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * (float)(Math.sin(var7_8) + (double)this.cw);
                    var8_9 = var9_10 * (float)(Math.cos(var8_9) + (double)this.cv);
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 * var6_7 * var7_8 + var8_9) * 2.0f);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * (float)(Math.sin(var7_8) + (double)this.cw);
                            var8_9 = var9_10 * (float)(Math.cos(var8_9) + (double)this.cv);
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void z() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 * var6_7 - var7_8 + var8_9) * 3.0f);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = (float)(Math.sin(var9_10) * (double)var8_9 + (double)this.cw);
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 * var6_7 - var7_8 + var8_9) * 3.0f);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = (float)(Math.sin(var9_10) * (double)var8_9 + (double)this.cw);
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void y() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 - var6_7 - var7_8 + var8_9) * 3.0f);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 / Math.max(Math.abs(var7_8), 1.0f) + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var8_9 + this.cw;
                    var8_9 = var9_10 * var7_8 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.cos((var5_6 - var6_7 - var7_8 + var8_9) * 3.0f);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 / Math.max(Math.abs(var7_8), 1.0f) + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var8_9 + this.cw;
                            var8_9 = var9_10 * var7_8 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void x() {
        var1_1 = this.a9.ae * 2;
        var12_2 = 0;
        if (true) ** GOTO lbl53
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                    var10_11 = 3.0f * var5_6 + (float)Math.cos((double)(var5_6 - var6_7 + var7_8) * 2.5);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var10_11 * var7_8 + this.cw;
                    var8_9 = var9_10 * var8_9 + this.cv;
                    var11_12 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var11_12 < 0.0f);
                if (var11_12 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                            var10_11 = 3.0f * var5_6 + (float)Math.cos((double)(var5_6 - var6_7 + var7_8) * 2.5);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var10_11 * var7_8 + this.cw;
                            var8_9 = var9_10 * var8_9 + this.cv;
                            var11_12 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var11_12 < 0.0f);
                        if (var11_12 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var12_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var12_2;
lbl53:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void w() {
        var1_1 = this.a9.ae * 2;
        var12_2 = 0;
        if (true) ** GOTO lbl53
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                    var10_11 = 3.0f * var5_6 + (float)Math.cos((double)(var5_6 - var6_7 + var7_8) * 2.5);
                    var5_6 = var5_6 * var5_6 + var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var10_11 * var7_8 + this.cw;
                    var8_9 = var10_11 * var8_9 + this.cv;
                    var11_12 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var11_12 < 0.0f);
                if (var11_12 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.sin((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5);
                            var10_11 = 3.0f * var5_6 + (float)Math.cos((double)(var5_6 - var6_7 + var7_8) * 2.5);
                            var5_6 = var5_6 * var5_6 + var6_7 * var6_7 - var7_8 * var7_8 - var8_9 * var8_9 + this.c_;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var10_11 * var7_8 + this.cw;
                            var8_9 = var10_11 * var8_9 + this.cv;
                            var11_12 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var11_12 < 0.0f);
                        if (var11_12 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var12_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var12_2;
lbl53:
            // 2 sources

        } while (var1_1 > 0);
    }

    /*
     * Unable to fully structure code
     */
    private final void v() {
        var1_1 = this.a9.ae * 2;
        var11_2 = 0;
        if (true) ** GOTO lbl51
        do {
            this.a5 = this.cl;
            var2_3 = this.a9.ak;
            while (var2_3 > 0) {
                var5_6 = this.a7;
                var6_7 = this.a6;
                var7_8 = this.a5;
                var8_9 = this.a4;
                var4_5 = this.a3;
                do {
                    var9_10 = 2.0f * var5_6 + (float)Math.cos((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5 * (double)this.cw);
                    var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_ + this.c0;
                    var6_7 = var9_10 * var6_7 + this.c0;
                    var7_8 = var9_10 * var7_8 + this.cw;
                    var8_9 = -var9_10 * var8_9 + this.cv;
                    var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                } while (--var4_5 > 0 && var10_11 < 0.0f);
                if (var10_11 < 0.0f) {
                    this.a5 += this.ci - this.ch;
                    var3_4 = this.a9.aj;
                    while (var3_4 > 0) {
                        var5_6 = this.a7;
                        var6_7 = this.a6;
                        var7_8 = this.a5;
                        var8_9 = this.a4;
                        var4_5 = this.a3;
                        do {
                            var9_10 = 2.0f * var5_6 + (float)Math.cos((double)(var5_6 - var6_7 + var7_8 + var8_9) * 1.5 * (double)this.cw);
                            var5_6 = var5_6 * var5_6 - var6_7 * var6_7 - var7_8 * var7_8 + var8_9 * var8_9 + this.c_ + this.c0;
                            var6_7 = var9_10 * var6_7 + this.c0;
                            var7_8 = var9_10 * var7_8 + this.cw;
                            var8_9 = -var9_10 * var8_9 + this.cv;
                            var10_11 = var5_6 * var5_6 + var6_7 * var6_7 + var7_8 * var7_8 + var8_9 * var8_9 - 4.0f;
                        } while (--var4_5 > 0 && var10_11 < 0.0f);
                        if (var10_11 < 0.0f) break;
                        --var3_4;
                        this.a5 -= this.ch;
                    }
                    this.ca[var11_2] = this.a5;
                    break;
                }
                --var2_3;
                this.a5 -= this.ci;
            }
            this.a7 += this.ck;
            --var1_1;
            ++var11_2;
lbl51:
            // 2 sources

        } while (var1_1 > 0);
    }

    public a(int n) {
        this.a1 = n;
        this.b3 = new float[n];
        this.b2 = new float[n];
        this.b1 = new float[n];
        this.cd = new float[n * 2];
        this.cc = new float[n * 2];
        this.ca = new float[n * 2];
        this.b9 = new float[n * 2];
        this.b8 = new float[n * 2];
        this.b7 = new float[n * 2];
        this.b6 = new float[n * 2];
        this.b5 = new float[n * 2];
        this.b4 = new float[n * 2];
    }

    private static String u(String string) {
        char[] cArray = string.toCharArray();
        int n = cArray.length;
        int n2 = 0;
        while (n2 < n) {
            int n3;
            int n4 = n2;
            char c = cArray[n4];
            switch (n2 % 5) {
                case 0: {
                    n3 = 88;
                    break;
                }
                case 1: {
                    n3 = 79;
                    break;
                }
                case 2: {
                    n3 = 67;
                    break;
                }
                case 3: {
                    n3 = 13;
                    break;
                }
                default: {
                    n3 = 13;
                }
            }
            cArray[n4] = (char)(c ^ n3);
            ++n2;
        }
        return new String(cArray);
    }
}

