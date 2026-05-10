/*
 * Decompiled with CFR 0.152.
 */
public class b {
    public static int[] b7;
    public static int[] b6;
    public static int[] b5;
    public static int[] b4;
    public static int[] b3;
    public static int[] b2;
    public static int[] b1;
    public static int[] b0;
    public static short[] b_;
    public byte bz;
    public byte by;
    public byte bx;
    public byte bw;
    public byte bv;
    public byte bu;
    public byte bt;
    public byte bs;
    public byte br;
    public byte bq;
    public byte bp;
    public byte bo;
    public byte bn;
    public byte bm;
    public byte bl;
    public byte bk;
    public g bj = new g();
    public g bi = new g();
    public g bh = new g();
    public g bg = new g();
    public g bf = new g();
    public int[] be;

    /*
     * Enabled aggressive block sorting
     */
    public final void u() {
        this.bj.eu = 0L;
        this.bj.ev = 0L;
        this.bj.et = 16;
        this.bi.eu = 0L;
        this.bi.ev = 0L;
        this.bi.et = 16;
        this.bh.eu = 0L;
        this.bh.ev = 0L;
        this.bh.et = 16;
        this.bg.eu = 0L;
        this.bg.ev = 0L;
        this.bg.et = 1;
        this.bf.eu = 0L;
        this.bf.ev = 0L;
        this.bf.et = 1;
        this.be = new int[10000];
        int n = 0;
        while (n < 5000) {
            this.be[n * 2] = (int)(Math.random() * 33.0) - 16;
            this.be[n * 2 + 1] = -this.be[n * 2];
            ++n;
        }
        n = 0;
        while (n < 14) {
            this.t((byte)n, (byte)0);
            ++n;
        }
    }

    public final void t(byte by, byte by2) {
        switch (by) {
            case 0: {
                this.bz = by2;
                break;
            }
            case 1: {
                this.by = by2;
                break;
            }
            case 2: {
                this.bx = by2;
                break;
            }
            case 3: {
                this.bw = by2;
                break;
            }
            case 4: {
                this.bv = by2;
                break;
            }
            case 5: {
                this.bu = by2;
                break;
            }
            case 6: {
                this.bt = by2;
                break;
            }
            case 7: {
                this.bs = by2;
                break;
            }
            case 8: {
                this.br = by2;
                break;
            }
            case 9: {
                this.bq = by2;
                break;
            }
            case 10: {
                this.bp = by2;
                break;
            }
            case 11: {
                this.bo = by2;
                break;
            }
            case 12: {
                this.bn = by2;
                break;
            }
            case 13: {
                if (by2 == -1) break;
                this.bm = by2;
                this.bf.eu = 0L;
                break;
            }
            case 14: {
                this.bl = by2;
                break;
            }
            case 15: {
                this.bk = by2;
                break;
            }
        }
    }

    public final void s(short[] sArray) {
        int n = sArray.length;
        if (n >= 10000) {
            return;
        }
        this.n();
        this.r(this.bj, n);
        this.r(this.bi, n);
        this.r(this.bh, n);
        this.o(n);
        this.p(n);
        int n2 = 0;
        while (n2 < n) {
            sArray[n2] = 0;
            ++n2;
        }
        this.l(this.bj, this.br, this.bs, sArray, n);
        this.l(this.bi, this.bq, this.bs >> 1, sArray, n);
        this.l(this.bh, this.bp, this.bs >> 2, sArray, n);
    }

    public final void r(g g2, int n) {
        if (g2.ep) {
            this.q(g2, n);
            return;
        }
        long l = g2.ev;
        long l2 = g2.eu;
        int n2 = g2.et;
        int n3 = 0;
        while (n3 < n) {
            if ((l2 += l) >= 0x1000000L) {
                l2 &= 0xFFFFFFL;
                n2 = -n2;
            }
            g2.eo[n3] = n2;
            ++n3;
        }
        g2.eu = l2;
        g2.et = n2;
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void q(g g2, int n) {
        long l = g2.es;
        long l2 = g2.er;
        boolean bl = g2.eq;
        long l3 = g2.ev;
        long l4 = g2.eu;
        int n2 = g2.et;
        int n3 = 0;
        while (n3 < n) {
            if ((l4 += l3) >= 0x1000000L) {
                l4 &= 0xFFFFFFL;
                n2 = -n2;
            }
            if ((l2 += l) >= 0x1000000L) {
                l2 &= 0xFFFFFFL;
                bl = !bl;
            }
            g2.eo[n3] = bl ? n2 : 0;
            ++n3;
        }
        g2.er = l2;
        g2.eq = bl;
        g2.eu = l4;
        g2.et = n2;
    }

    public final void p(int n) {
        long l = this.bg.ev;
        long l2 = this.bg.eu;
        int n2 = 0;
        int n3 = 0;
        while (n3 < n) {
            if ((l2 += l) >= 0x1000000L) {
                l2 &= 0xFFFFFFL;
                ++n2;
            }
            this.bg.eo[n3] = this.be[n2];
            ++n3;
        }
        this.bg.eu = l2;
    }

    /*
     * Unable to fully structure code
     */
    public final void o(int var1_1) {
        block28: {
            block27: {
                var2_2 = this.bf.eu;
                var4_3 = this.bf.ev;
                if (this.bf.et != 0) break block27;
                var6_4 = 0;
                while (var6_4 < var1_1) {
                    this.bf.eo[var6_4] = 0;
                    ++var6_4;
                }
                break block28;
            }
            switch (this.bm & 15) {
                case 0: 
                case 1: 
                case 2: 
                case 3: 
                case 9: {
                    var6_5 = 0;
                    while (var6_5 < var1_1) {
                        v0 = this.bf.eo;
                        if (var2_2 > 0x1FFFFFFFL) {
                            v1 = 17L;
                        } else {
                            var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                            v1 = var2_2 >> 24;
                        }
                        v0[var6_5] = b.b6[(int)v1];
                        ++var6_5;
                    }
                    break;
                }
                case 4: 
                case 5: 
                case 6: 
                case 7: 
                case 15: {
                    var6_6 = 0;
                    while (var6_6 < var1_1) {
                        v2 = this.bf.eo;
                        if (var2_2 > 0x1FFFFFFFL) {
                            v3 = 17L;
                        } else {
                            var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                            v3 = var2_2 >> 24;
                        }
                        v2[var6_6] = b.b7[(int)v3];
                        ++var6_6;
                    }
                    break;
                }
                case 8: {
                    var6_7 = 0;
                    while (var6_7 < var1_1) {
                        var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                        this.bf.eo[var6_7] = b.b1[(int)(var2_2 >> 24)];
                        ++var6_7;
                    }
                    break;
                }
                case 10: {
                    var6_8 = 0;
                    while (var6_8 < var1_1) {
                        var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                        this.bf.eo[var6_8] = b.b0[(int)(var2_2 >> 24)];
                        ++var6_8;
                    }
                    break;
                }
                case 12: {
                    var6_9 = 0;
                    while (var6_9 < var1_1) {
                        var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                        this.bf.eo[var6_9] = b.b2[(int)(var2_2 >> 24)];
                        ++var6_9;
                    }
                    break;
                }
                case 14: {
                    var6_10 = 0;
                    while (var6_10 < var1_1) {
                        var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                        this.bf.eo[var6_10] = b.b3[(int)(var2_2 >> 24)];
                        ++var6_10;
                    }
                    break;
                }
                case 11: {
                    var6_11 = 0;
                    while (var6_11 < var1_1) {
                        v4 = this.bf.eo;
                        if (var2_2 > 0x1FFFFFFFL) {
                            v5 = 17L;
                        } else {
                            var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                            v5 = var2_2 >> 24;
                        }
                        v4[var6_11] = b.b4[(int)v5];
                        ++var6_11;
                    }
                    break;
                }
                case 13: {
                    var6_12 = 0;
                    if (true) ** GOTO lbl96
                    do {
                        v6 = this.bf.eo;
                        if (var2_2 > 0x1FFFFFFFL) {
                            v7 = 17L;
                        } else {
                            var2_2 = var2_2 + var4_3 & 0x1FFFFFFFL;
                            v7 = var2_2 >> 24;
                        }
                        v6[var6_12] = b.b5[(int)v7];
                        ++var6_12;
lbl96:
                        // 2 sources

                    } while (var6_12 < var1_1);
                    break;
                }
            }
        }
        this.bf.eu = var2_2;
    }

    public final void n() {
        g[] gArray = new g[]{null, this.bj, this.bi, this.bh};
        int n = 1;
        while (n < 4) {
            gArray[n].ep = false;
            ++n;
        }
        this.m(this.by >> 4 & 3, this.by >> 6 & 3, this.bt >> 5 & 7, this.bl & 0xFF);
        this.m(this.bw >> 4 & 3, this.bw >> 6 & 3, this.br >> 5 & 7, this.bk & 0xFF);
        this.bj.am(this.by, this.bz);
        this.bi.am(this.bw, this.bx);
        this.bh.am(this.bu, this.bv);
        double d2 = (double)(this.bn << 8) + (double)(this.bo & 0xFF);
        if (d2 == 0.0) {
            this.bf.et = 0;
            this.bf.ev = 0;
        } else {
            this.bf.ev = (long)(2000000.0 / (d2 * 256.0) / 44100.0 * 16.0 * 65536.0 * 256.0);
            this.bf.et = this.bf.et == 0 ? 1 : this.bf.et;
        }
        this.bg.am((byte)0, (byte)(this.bt & 0x1F));
    }

    private final void m(int n, int n2, int n3, int n4) {
        g[] gArray = new g[]{null, this.bj, this.bi, this.bh};
        int[] nArray = new int[]{0, 4, 10, 16, 50, 64, 100, 200};
        if (n == 0) {
            return;
        }
        switch (n2) {
            case 0: {
                if (n4 == 0) {
                    n4 = 256;
                }
                n3 = nArray[n3];
                double d2 = 0.0;
                if (n3 != 0) {
                    long l;
                    d2 = 2457600.0 / (double)(n3 * n4);
                    gArray[n].ep = true;
                    gArray[n].es = l = (long)(d2 / 44100.0 * 65536.0 * 256.0);
                    break;
                }
                gArray[n].er = 0L;
                gArray[n].es = 0L;
                gArray[n].ep = false;
                break;
            }
            case 1: {
                break;
            }
            case 2: {
                System.out.println("SineSID");
                break;
            }
            case 3: {
                System.out.println("SyncBuzzer");
                break;
            }
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void l(g g2, byte by, int n, short[] sArray, int n2) {
        if ((by & 0x10) != 0) {
            switch (n & 9) {
                case 0: {
                    int n3 = 0;
                    while (true) {
                        if (n3 >= n2) {
                            return;
                        }
                        int n4 = n3;
                        sArray[n4] = (short)(sArray[n4] + b_[15 + (g2.eo[n3] * this.bg.eo[n3] * this.bf.eo[n3] >> 8)]);
                        ++n3;
                    }
                }
                case 1: {
                    int n5 = 0;
                    while (true) {
                        if (n5 >= n2) {
                            return;
                        }
                        int n6 = n5;
                        sArray[n6] = (short)(sArray[n6] + b_[15 + (this.bg.eo[n5] * this.bf.eo[n5] >> 4)]);
                        ++n5;
                    }
                }
                case 8: {
                    int n7 = 0;
                    while (n7 < n2) {
                        int n8 = n7;
                        sArray[n8] = (short)(sArray[n8] + b_[15 + (g2.eo[n7] * this.bf.eo[n7] >> 4)]);
                        ++n7;
                    }
                    return;
                }
            }
            int n9 = 0;
            while (true) {
                if (n9 >= n2) {
                    return;
                }
                int n10 = n9;
                sArray[n10] = (short)(sArray[n10] + b_[15 + this.bf.eo[n9]]);
                ++n9;
            }
        }
        int n11 = by & 0xF;
        switch (n & 9) {
            case 0: {
                int n12 = 0;
                while (true) {
                    if (n12 >= n2) {
                        return;
                    }
                    int n13 = n12;
                    sArray[n13] = (short)(sArray[n13] + b_[(g2.eo[n12] * this.bg.eo[n12] * n11 >> 8) + 15]);
                    ++n12;
                }
            }
            case 1: {
                int n14 = 0;
                while (true) {
                    if (n14 >= n2) {
                        return;
                    }
                    int n15 = n14;
                    sArray[n15] = (short)(sArray[n15] + b_[(this.bg.eo[n14] * n11 >> 4) + 15]);
                    ++n14;
                }
            }
            case 8: {
                int n16 = 0;
                while (n16 < n2) {
                    int n17 = n16;
                    sArray[n17] = (short)(sArray[n17] + b_[(g2.eo[n16] * n11 >> 4) + 15]);
                    ++n16;
                }
                return;
            }
        }
    }

    static {
        int[] nArray = new int[32];
        nArray[0] = 0;
        nArray[1] = 1;
        nArray[2] = 2;
        nArray[3] = 3;
        nArray[4] = 4;
        nArray[5] = 5;
        nArray[6] = 6;
        nArray[7] = 7;
        nArray[8] = 8;
        nArray[9] = 9;
        nArray[10] = 10;
        nArray[11] = 11;
        nArray[12] = 12;
        nArray[13] = 13;
        nArray[14] = 14;
        nArray[15] = 15;
        nArray[16] = 0;
        nArray[17] = 0;
        nArray[18] = 0;
        nArray[19] = 0;
        nArray[20] = 0;
        nArray[21] = 0;
        nArray[22] = 0;
        nArray[23] = 0;
        nArray[24] = 0;
        nArray[25] = 0;
        nArray[26] = 0;
        nArray[27] = 0;
        nArray[28] = 0;
        nArray[29] = 0;
        nArray[30] = 0;
        nArray[31] = 0;
        b7 = nArray;
        b6 = new int[]{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        b5 = new int[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
        b4 = new int[]{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
        b3 = new int[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        b2 = new int[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        b1 = new int[]{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        b0 = new int[]{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        b_ = new short[]{-10922, -7668, -4933, -3395, -2539, -2025, -1684, -1342, -1170, -1001, -828, -659, -487, -319, -180, 0, 180, 319, 487, 659, 828, 1001, 1170, 1342, 1684, 2025, 2539, 3395, 4933, 7668, 10922};
    }
}

