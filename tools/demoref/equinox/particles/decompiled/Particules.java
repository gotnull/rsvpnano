/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;

public class Particules
extends Applet
implements Runnable {
    public Image ce;
    public byte[] cd;
    public MemoryImageSource ca;
    public Thread b9;
    public float b8;
    public float b7;
    public float b6;
    public float b5;
    public float b4;
    public float b3;
    public float b2;
    public float b1;
    public float b0;
    public float b_;
    public float bz;
    public float by;
    public float bx;
    public float bw;
    public float bv;
    public float bu;
    public float bt;
    public int bs;
    public int br;
    public int bq;
    public int bp;
    public int bo;
    public float bn;
    public float bm;
    public float bl;
    public int bk;
    public int bj;
    public int bi;
    public c bh;
    public d bg;
    public e bf;
    public byte[] be;
    public long bd;
    public long bc;
    public long ba;
    public int a9 = 500;
    public b[] a8;
    public a[] a7;
    public a a6;
    public int a5;
    public int a4;
    public int a3;
    public float[] a2;
    public float[] a1;
    public int a0;
    public int a_;
    public byte[] az;
    public byte[] ay;
    public byte[] ax;
    public int aw;
    public int[] av = new int[20];
    public int au;

    public final void init() {
        this.az = new byte[256];
        this.ay = new byte[256];
        this.ax = new byte[256];
        this.a2 = new float[3];
        this.be = new byte[3300];
        this.a1 = new float[29];
        this.a0 = -4;
        this.a_ = -1;
        this.a1[0] = 100.0f;
        this.a1[1] = 0.0f;
        this.a1[2] = 0.0f;
        this.a1[3] = 0.0f;
        this.a1[4] = 150.0f;
        this.a1[5] = -0.02f;
        this.a1[6] = 0.04f;
        this.a1[7] = 0.038f;
        this.a1[8] = 200.0f;
        this.a1[9] = 0.02f;
        this.a1[10] = -0.04f;
        this.a1[11] = -0.03f;
        this.a1[12] = 100.0f;
        this.a1[13] = -0.04f;
        this.a1[14] = 0.02f;
        this.a1[15] = 0.036f;
        this.a1[16] = 200.0f;
        this.a1[17] = 0.03f;
        this.a1[18] = -0.04f;
        this.a1[19] = -0.04f;
        this.a1[20] = 100.0f;
        this.a1[21] = 0.025f;
        this.a1[22] = 0.03f;
        this.a1[23] = -0.03f;
        this.a1[24] = 100.0f;
        this.a1[25] = 0.0f;
        this.a1[26] = 0.0f;
        this.a1[27] = 0.04f;
        this.a1[28] = -1.0f;
        f f2 = new f();
        this.bh = new c();
        this.bg = new d();
        this.bf = new e();
        this.b8 = 0.0f;
        this.b7 = -9.81f;
        this.b6 = 0.0f;
        this.b5 = 0.91f;
        this.b4 = 0.081f;
        this.bk = -1;
        int n = 0;
        while (n < (int)this.a1[12]) {
            byte cfr_ignored_2 = (byte)n;
            int n2 = 0;
            while (n2 < (int)(this.a1[12] / 3.0f)) {
                int n3 = 0;
                while (n3 < 10) {
                    byte cfr_ignored_3 = (byte)(n >> 2);
                    this.au = 1;
                    this.av[n3 + n2 / 30] = n2;
                    ++n3;
                }
                this.b3 = 73.0f;
                ++n2;
            }
            ++n;
        }
        this.b3 = 73.0f;
        this.b2 = -73.0f;
        this.b1 = 73.0f;
        this.b0 = -73.0f;
        this.b_ = 73.0f;
        this.bz = -73.0f;
        this.by = 0.0f;
        this.bx = 0.0f;
        this.bw = 0.0f;
        this.bv = 0.0f;
        this.bu = 0.0f;
        this.bt = 0.0f;
        this.bn = 0.0f;
        this.bm = 0.0f;
        this.bl = 0.0f;
        this.bf.c(this.be);
        this.a8 = new b[this.a9 + 8];
        n = 0;
        while (n < this.a9 + 8) {
            Math.cos((double)n / 100.0);
            this.a8[n] = new b();
            ++n;
        }
        this.a6 = new a();
        this.a7 = new a[this.a9 + 8];
        n = 0;
        while (n < this.a9 + 8) {
            this.a7[n] = new a();
            ++n;
        }
        n = 0;
        while (n < this.a9) {
            int cfr_ignored_4 = (int)this.a7[n].c;
            this.a7[n].l = 0.0f;
            this.a7[n].k = 0.0f;
            this.a7[n].j = 0.0f;
            this.a7[n].c = 0.0f;
            this.a7[n].b = 0.0f;
            this.a7[n].a = 0.0f;
            this.a7[n].f = (float)(Math.sin(((float)n + 0.01f) / 5.0f) * Math.cos(n) * 50.0);
            this.a7[n].e = (float)(Math.cos(((float)n + 0.01f) / 10.0f) * 50.0);
            this.a7[n].d = (float)(Math.sin(((float)n + 0.01f) / 30.0f) * 50.0);
            byte cfr_ignored_5 = (byte)this.a7[n].c;
            ++n;
        }
        this.cd = new byte[90000];
        this.bg.c(this.az, this.ay, this.ax);
        this.e();
        this.bd = System.currentTimeMillis();
        this.ba = 0L;
        this.showStatus("PopCorn Cube ready!");
    }

    public final void start() {
        if (this.b9 == null) {
            this.b9 = new Thread(this);
        }
        this.b9.start();
    }

    public final void stop() {
        if (this.b9 != null) {
            this.b9 = null;
            this.b9.stop();
        }
    }

    public final void run() {
        while (this.b9 != null) {
            this.ca.newPixels();
            this.b3 = (float)((Math.abs(Math.cos(this.bt)) * 0.7 + 0.3) * 73.0);
            this.b2 = (float)(-(Math.abs(Math.sin(this.bt)) * 0.7 + 0.3) * 73.0);
            this.b_ = (float)((Math.abs(Math.cos(this.bv)) * 0.7 + 0.3) * 73.0);
            this.bz = (float)(-(Math.abs(Math.sin(this.bv)) * 0.7 + 0.3) * 73.0);
            this.b1 = (float)((Math.abs(Math.sin(this.bu)) * 0.7 + 0.3) * 73.0);
            this.b0 = (float)(-(Math.abs(Math.sin(this.bu)) * 0.7 + 0.3) * 73.0);
            this.bv = (float)((double)this.bv + 0.01);
            this.bu = (float)((double)this.bu + 0.03);
            this.bt = (float)((double)this.bt + -0.02);
            this.a3 = 0;
            while (this.a3 < 90000) {
                this.cd[this.a3] = (byte)((this.cd[this.a3] & 0xFE) >> 1);
                ++this.a3;
            }
            this.a2[0] = 0.0f;
            this.a2[1] = -9.81f;
            this.a2[2] = 0.0f;
            this.bh.b(this.a7, this.a9, this.a2, -this.by, -this.bx, -this.bw);
            this.b8 = this.a2[0];
            this.b7 = this.a2[1];
            this.b6 = this.a2[2];
            this.a7[this.a9].i = this.bz - 5.0f;
            this.a7[this.a9].h = this.b1 + 5.0f;
            this.a7[this.a9].g = this.b3 + 5.0f;
            this.a7[this.a9 + 1].i = this.bz - 5.0f;
            this.a7[this.a9 + 1].h = this.b0 - 5.0f;
            this.a7[this.a9 + 1].g = this.b3 + 5.0f;
            this.a7[this.a9 + 2].i = this.b_ + 5.0f;
            this.a7[this.a9 + 2].h = this.b0 - 5.0f;
            this.a7[this.a9 + 2].g = this.b3 + 5.0f;
            this.a7[this.a9 + 3].i = this.b_ + 5.0f;
            this.a7[this.a9 + 3].h = this.b1 + 5.0f;
            this.a7[this.a9 + 3].g = this.b3 + 5.0f;
            this.a7[this.a9 + 4].i = this.bz - 5.0f;
            this.a7[this.a9 + 4].h = this.b1 + 5.0f;
            this.a7[this.a9 + 4].g = this.b2 - 5.0f;
            this.a7[this.a9 + 5].i = this.bz - 5.0f;
            this.a7[this.a9 + 5].h = this.b0 - 5.0f;
            this.a7[this.a9 + 5].g = this.b2 - 5.0f;
            this.a7[this.a9 + 6].i = this.b_ + 5.0f;
            this.a7[this.a9 + 6].h = this.b0 - 5.0f;
            this.a7[this.a9 + 6].g = this.b2 - 5.0f;
            this.a7[this.a9 + 7].i = this.b_ + 5.0f;
            this.a7[this.a9 + 7].h = this.b1 + 5.0f;
            this.a7[this.a9 + 7].g = this.b2 - 5.0f;
            this.a3 = 0;
            while (this.a3 < this.a9) {
                this.a6 = this.a7[this.a3];
                this.a6.c += this.b4;
                this.a6.b += this.b4;
                this.a6.a += this.b4;
                this.a6.i = (this.b8 * this.a6.c / 2.0f + this.a6.l) * this.a6.c + this.a6.f;
                this.a6.h = (this.b7 * this.a6.b / 2.0f + this.a6.k) * this.a6.b + this.a6.e;
                this.a6.g = (this.b6 * this.a6.a / 2.0f + this.a6.j) * this.a6.a + this.a6.d;
                byte cfr_ignored_0 = (byte)(this.a3 >> 1);
                if (this.a6.h >= this.b1) {
                    this.a6.h = 2.0f * this.b1 - this.a6.h;
                    this.a6.k = -((this.a6.b - this.b4) * this.b7 + this.a6.k) * this.b5;
                    this.a6.e = this.a6.h;
                    this.a6.b = 0.0f;
                }
                ++this.aw;
                if (this.a6.h <= this.b0) {
                    this.a6.h = 2.0f * this.b0 - this.a6.h;
                    this.a6.k = -((this.a6.b - this.b4) * this.b7 + this.a6.k) * this.b5;
                    this.a6.e = this.a6.h;
                    this.a6.b = 0.0f;
                }
                int cfr_ignored_1 = (int)this.bz & 0x3F;
                if (this.a6.i >= this.b_) {
                    int cfr_ignored_2 = (int)this.b_;
                    this.a6.i = 2.0f * this.b_ - this.a6.i;
                    this.a6.l = -((this.a6.c - this.b4) * this.b8 + this.a6.l) * this.b5;
                    this.a6.f = this.a6.i;
                    this.a6.c = 0.0f;
                }
                byte cfr_ignored_3 = (byte)((int)this.b_ & 0x7F);
                if (this.a6.i <= this.bz) {
                    int cfr_ignored_4 = (int)this.bz;
                    this.a6.i = 2.0f * this.bz - this.a6.i;
                    this.a6.l = -((this.a6.c - this.b4) * this.b8 + this.a6.l) * this.b5;
                    this.a6.f = this.a6.i;
                    this.a6.c = 0.0f;
                }
                if (this.a6.g >= this.b3) {
                    int cfr_ignored_5 = (int)this.b3;
                    this.a6.g = 2.0f * this.b3 - this.a6.g;
                    this.a6.j = -((this.a6.a - this.b4) * this.b6 + this.a6.j) * this.b5;
                    this.a6.d = this.a6.g;
                    this.a6.a = 0.0f;
                }
                if (this.a6.g <= this.b2) {
                    int cfr_ignored_6 = (int)(this.b3 / 2.0f);
                    this.a6.g = 2.0f * this.b2 - this.a6.g;
                    this.a6.j = -((this.a6.a - this.b4) * this.b6 + this.a6.j) * this.b5;
                    this.a6.d = this.a6.g;
                    this.a6.a = 0.0f;
                }
                ++this.a3;
            }
            this.bh.a(this.a7, this.a9, this.by, this.bx, this.bw);
            this.bh.c(this.a7, this.a9 + 8, this.by, this.bx, this.bw, 0.0f, 0.0f, 0.0f, this.a8, 300, 300);
            this.bs = -10000;
            this.br = 10000;
            this.a4 = this.a9;
            while (this.a4 < this.a9 + 8) {
                if (this.a8[this.a4].g > this.bs) {
                    this.bs = this.a8[this.a4].g;
                }
                if (this.a8[this.a4].g < this.br) {
                    this.br = this.a8[this.a4].g;
                }
                ++this.a4;
            }
            this.d(this.a8[this.a9].i, this.a8[this.a9].h, this.a8[this.a9].g, this.a8[this.a9 + 1].i, this.a8[this.a9 + 1].h, this.a8[this.a9 + 1].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 1].i, this.a8[this.a9 + 1].h, this.a8[this.a9 + 1].g, this.a8[this.a9 + 2].i, this.a8[this.a9 + 2].h, this.a8[this.a9 + 2].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 2].i, this.a8[this.a9 + 2].h, this.a8[this.a9 + 2].g, this.a8[this.a9 + 3].i, this.a8[this.a9 + 3].h, this.a8[this.a9 + 3].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 3].i, this.a8[this.a9 + 3].h, this.a8[this.a9 + 3].g, this.a8[this.a9].i, this.a8[this.a9].h, this.a8[this.a9].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 4].i, this.a8[this.a9 + 4].h, this.a8[this.a9 + 4].g, this.a8[this.a9 + 5].i, this.a8[this.a9 + 5].h, this.a8[this.a9 + 5].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 5].i, this.a8[this.a9 + 5].h, this.a8[this.a9 + 5].g, this.a8[this.a9 + 6].i, this.a8[this.a9 + 6].h, this.a8[this.a9 + 6].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 6].i, this.a8[this.a9 + 6].h, this.a8[this.a9 + 6].g, this.a8[this.a9 + 7].i, this.a8[this.a9 + 7].h, this.a8[this.a9 + 7].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 7].i, this.a8[this.a9 + 7].h, this.a8[this.a9 + 7].g, this.a8[this.a9 + 4].i, this.a8[this.a9 + 4].h, this.a8[this.a9 + 4].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9].i, this.a8[this.a9].h, this.a8[this.a9].g, this.a8[this.a9 + 4].i, this.a8[this.a9 + 4].h, this.a8[this.a9 + 4].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 1].i, this.a8[this.a9 + 1].h, this.a8[this.a9 + 1].g, this.a8[this.a9 + 5].i, this.a8[this.a9 + 5].h, this.a8[this.a9 + 5].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 2].i, this.a8[this.a9 + 2].h, this.a8[this.a9 + 2].g, this.a8[this.a9 + 6].i, this.a8[this.a9 + 6].h, this.a8[this.a9 + 6].g, this.br, this.bs, 40, 127);
            this.d(this.a8[this.a9 + 3].i, this.a8[this.a9 + 3].h, this.a8[this.a9 + 3].g, this.a8[this.a9 + 7].i, this.a8[this.a9 + 7].h, this.a8[this.a9 + 7].g, this.br, this.bs, 40, 127);
            this.a4 = 0;
            while (this.a4 < this.a9) {
                this.a5 = this.a8[this.a4].i + 300 * this.a8[this.a4].h;
                int n = this.a5 + 1;
                this.cd[n] = (byte)(this.cd[n] | 2);
                int n2 = this.a5 + 2;
                this.cd[n2] = (byte)(this.cd[n2] | 0xA);
                int n3 = this.a5 + 3;
                this.cd[n3] = (byte)(this.cd[n3] | 0xC);
                int n4 = this.a5 + 4;
                this.cd[n4] = (byte)(this.cd[n4] | 0xC);
                int n5 = this.a5 + 5;
                this.cd[n5] = (byte)(this.cd[n5] | 7);
                int n6 = this.a5 + 6;
                this.cd[n6] = (byte)(this.cd[n6] | 4);
                int n7 = this.a5 + 300;
                this.cd[n7] = (byte)(this.cd[n7] | 1);
                int n8 = this.a5 + 1 + 300;
                this.cd[n8] = (byte)(this.cd[n8] | 0xF);
                int n9 = this.a5 + 2 + 300;
                this.cd[n9] = (byte)(this.cd[n9] | 0x1D);
                int n10 = this.a5 + 3 + 300;
                this.cd[n10] = (byte)(this.cd[n10] | 0x28);
                int n11 = this.a5 + 4 + 300;
                this.cd[n11] = (byte)(this.cd[n11] | 0x27);
                int n12 = this.a5 + 5 + 300;
                this.cd[n12] = (byte)(this.cd[n12] | 0x1A);
                int n13 = this.a5 + 6 + 300;
                this.cd[n13] = (byte)(this.cd[n13] | 0xE);
                int n14 = this.a5 + 7 + 300;
                this.cd[n14] = (byte)(this.cd[n14] | 4);
                int n15 = this.a5 + 600;
                this.cd[n15] = (byte)(this.cd[n15] | 7);
                int n16 = this.a5 + 1 + 600;
                this.cd[n16] = (byte)(this.cd[n16] | 0x1D);
                int n17 = this.a5 + 2 + 600;
                this.cd[n17] = (byte)(this.cd[n17] | 0x3A);
                int n18 = this.a5 + 3 + 600;
                this.cd[n18] = (byte)(this.cd[n18] | 0x48);
                int n19 = this.a5 + 4 + 600;
                this.cd[n19] = (byte)(this.cd[n19] | 0x45);
                int n20 = this.a5 + 5 + 600;
                this.cd[n20] = (byte)(this.cd[n20] | 0x34);
                int n21 = this.a5 + 6 + 600;
                this.cd[n21] = (byte)(this.cd[n21] | 0x18);
                int n22 = this.a5 + 7 + 600;
                this.cd[n22] = (byte)(this.cd[n22] | 6);
                int n23 = this.a5 + 900;
                this.cd[n23] = (byte)(this.cd[n23] | 0xC);
                int n24 = this.a5 + 1 + 900;
                this.cd[n24] = (byte)(this.cd[n24] | 0x27);
                int n25 = this.a5 + 2 + 900;
                this.cd[n25] = (byte)(this.cd[n25] | 0x46);
                int n26 = this.a5 + 3 + 900;
                this.cd[n26] = (byte)(this.cd[n26] | 0x5A);
                int n27 = this.a5 + 4 + 900;
                this.cd[n27] = (byte)(this.cd[n27] | 0x58);
                int n28 = this.a5 + 5 + 900;
                this.cd[n28] = (byte)(this.cd[n28] | 0x41);
                int n29 = this.a5 + 6 + 900;
                this.cd[n29] = (byte)(this.cd[n29] | 0x1F);
                int n30 = this.a5 + 7 + 900;
                this.cd[n30] = (byte)(this.cd[n30] | 8);
                int n31 = this.a5 + 1200;
                this.cd[n31] = (byte)(this.cd[n31] | 0xC);
                int n32 = this.a5 + 1 + 1200;
                this.cd[n32] = (byte)(this.cd[n32] | 0x24);
                int n33 = this.a5 + 2 + 1200;
                this.cd[n33] = (byte)(this.cd[n33] | 0x43);
                int n34 = this.a5 + 3 + 1200;
                this.cd[n34] = (byte)(this.cd[n34] | 0x56);
                int n35 = this.a5 + 4 + 1200;
                this.cd[n35] = (byte)(this.cd[n35] | 0x52);
                int n36 = this.a5 + 5 + 1200;
                this.cd[n36] = (byte)(this.cd[n36] | 0x3E);
                int n37 = this.a5 + 6 + 1200;
                this.cd[n37] = (byte)(this.cd[n37] | 0x1D);
                int n38 = this.a5 + 7 + 1200;
                this.cd[n38] = (byte)(this.cd[n38] | 7);
                int n39 = this.a5 + 1500;
                this.cd[n39] = (byte)(this.cd[n39] | 7);
                int n40 = this.a5 + 1 + 1500;
                this.cd[n40] = (byte)(this.cd[n40] | 0x17);
                int n41 = this.a5 + 2 + 1500;
                this.cd[n41] = (byte)(this.cd[n41] | 0x2C);
                int n42 = this.a5 + 3 + 1500;
                this.cd[n42] = (byte)(this.cd[n42] | 0x3B);
                int n43 = this.a5 + 4 + 1500;
                this.cd[n43] = (byte)(this.cd[n43] | 0x39);
                int n44 = this.a5 + 5 + 1500;
                this.cd[n44] = (byte)(this.cd[n44] | 0x27);
                int n45 = this.a5 + 6 + 1500;
                this.cd[n45] = (byte)(this.cd[n45] | 0x13);
                int n46 = this.a5 + 7 + 1500;
                this.cd[n46] = (byte)(this.cd[n46] | 5);
                int n47 = this.a5 + 1800;
                this.cd[n47] = (byte)(this.cd[n47] | 4);
                int n48 = this.a5 + 1 + 1800;
                this.cd[n48] = (byte)(this.cd[n48] | 0xC);
                int n49 = this.a5 + 2 + 1800;
                this.cd[n49] = (byte)(this.cd[n49] | 0x14);
                int n50 = this.a5 + 3 + 1800;
                this.cd[n50] = (byte)(this.cd[n50] | 0x1B);
                int n51 = this.a5 + 4 + 1800;
                this.cd[n51] = (byte)(this.cd[n51] | 0x1A);
                int n52 = this.a5 + 5 + 1800;
                this.cd[n52] = (byte)(this.cd[n52] | 0x13);
                int n53 = this.a5 + 6 + 1800;
                this.cd[n53] = (byte)(this.cd[n53] | 0xA);
                int n54 = this.a5 + 7 + 1800;
                this.cd[n54] = (byte)(this.cd[n54] | 1);
                int n55 = this.a5 + 1 + 2100;
                this.cd[n55] = (byte)(this.cd[n55] | 1);
                int n56 = this.a5 + 2 + 2100;
                this.cd[n56] = (byte)(this.cd[n56] | 5);
                int n57 = this.a5 + 3 + 2100;
                this.cd[n57] = (byte)(this.cd[n57] | 7);
                int n58 = this.a5 + 4 + 2100;
                this.cd[n58] = (byte)(this.cd[n58] | 7);
                int n59 = this.a5 + 5 + 2100;
                this.cd[n59] = (byte)(this.cd[n59] | 5);
                int n60 = this.a5 + 6 + 2100;
                this.cd[n60] = (byte)(this.cd[n60] | 2);
                ++this.a4;
            }
            if (this.a_ == -1) {
                this.a0 += 4;
                if (this.a1[this.a0] == -1.0f) {
                    this.a0 = 0;
                }
                ++this.aw;
                this.a_ = (int)this.a1[this.a0];
                this.bk = 50;
            }
            if (this.bk >= 0) {
                this.by += ((float)this.bk + 0.001f) / 50.001f * this.bn + ((float)(50 - this.bk) + 0.001f) / 50.001f * this.a1[this.a0 + 1] / 2.0f;
                this.bx += ((float)this.bk + 0.001f) / 50.001f * this.bm + ((float)(50 - this.bk) + 0.001f) / 50.001f * this.a1[this.a0 + 2] / 2.0f;
                this.bw += ((float)this.bk + 0.001f) / 50.001f * this.bl + ((float)(50 - this.bk) + 0.001f) / 50.001f * this.a1[this.a0 + 3] / 2.0f;
                ++this.aw;
                --this.bk;
            } else {
                --this.aw;
                this.bn = this.a1[this.a0 + 1] / 2.0f;
                this.bm = this.a1[this.a0 + 2] / 2.0f;
                this.bl = this.a1[this.a0 + 3] / 2.0f;
                this.by += this.a1[this.a0 + 1] / 2.0f;
                this.bx += this.a1[this.a0 + 2] / 2.0f;
                this.bw += this.a1[this.a0 + 3] / 2.0f;
                --this.a_;
            }
            if ((double)this.by > Math.PI * 2) {
                this.by = (float)((double)this.by - Math.PI * 2);
            }
            if ((double)this.by < Math.PI * -2) {
                this.by = (float)((double)this.by + Math.PI * 2);
            }
            if ((double)this.bx > Math.PI * 2) {
                this.bx = (float)((double)this.bx - Math.PI * 2);
            }
            if ((double)this.bx < Math.PI * -2) {
                this.bx = (float)((double)this.bx + Math.PI * 2);
            }
            if ((double)this.bw > Math.PI * 2) {
                this.bw = (float)((double)this.bw - Math.PI * 2);
            }
            if ((double)this.bw < Math.PI * -2) {
                this.bw = (float)((double)this.bw + Math.PI * 2);
            }
            if ((double)this.bv > Math.PI * 2) {
                this.bv = (float)((double)this.bv - Math.PI * 2);
            }
            if ((double)this.bv < Math.PI * -2) {
                this.bv = (float)((double)this.bv + Math.PI * 2);
            }
            if ((double)this.bu > Math.PI * 2) {
                this.bu = (float)((double)this.bu - Math.PI * 2);
            }
            if ((double)this.bu < Math.PI * -2) {
                this.bu = (float)((double)this.bu + Math.PI * 2);
            }
            if ((double)this.bt > Math.PI * 2) {
                this.bt = (float)((double)this.bt - Math.PI * 2);
            }
            if ((double)this.bt < Math.PI * -2) {
                this.bt = (float)((double)this.bt + Math.PI * 2);
            }
            this.f();
            ++this.ba;
            this.bc = System.currentTimeMillis();
            if (this.bc - this.bd > 1000L) {
                System.gc();
                this.bd = this.bc;
                this.ba = 0L;
            }
            try {
                Thread.sleep(1L);
            }
            catch (InterruptedException interruptedException) {
                // empty catch block
            }
        }
    }

    public final synchronized void update(Graphics graphics) {
        this.paint(graphics);
    }

    public final synchronized void paint(Graphics graphics) {
        if (this.b9 != null) {
            graphics.drawImage(this.ce, 0, 0, this);
        }
    }

    private final void f() {
        int n = 0;
        while (n < 3300) {
            if (this.be[n] != 0) {
                this.cd[n + 600] = 127;
            }
            ++n;
        }
    }

    private final void e() {
        switch (this.au) {
            case 0: {
                IndexColorModel indexColorModel = new IndexColorModel(24, 65288, this.ay, this.ax, this.ay);
                this.ca = new MemoryImageSource(300, 300, (ColorModel)indexColorModel, this.cd, 0, 300);
                this.ca.setAnimated(false);
                break;
            }
            case 1: {
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.az, this.ay, this.ax);
                this.ca = new MemoryImageSource(300, 300, (ColorModel)indexColorModel, this.cd, 0, 300);
                this.ca.setAnimated(true);
                break;
            }
        }
        this.ca.setFullBufferUpdates(true);
        this.ce = this.createImage(this.ca);
        this.ca.newPixels();
    }

    public final void d(int n, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10) {
        this.bq = (int)((float)(n3 - n7) / (float)(n8 - n7) * (float)(n10 - n9) + (float)n9);
        this.bp = (int)((float)(n6 - n7) / (float)(n8 - n7) * (float)(n10 - n9) + (float)n9);
        if (Math.abs(n4 - n) > Math.abs(n5 - n2)) {
            if (n > n4) {
                int cfr_ignored_0 = this.bi >> 4;
                this.bi = n;
                n = n4;
                n4 = this.bi;
                this.bi = n2;
                n2 = n5;
                n5 = this.bi;
                this.bi = this.bq;
                this.bq = this.bp;
                this.bp = this.bi;
            }
            int cfr_ignored_1 = n4 >> 2;
            this.bi = n4 - n;
            n2 <<= 16;
            n5 <<= 16;
            this.bq <<= 16;
            this.bp <<= 16;
            int cfr_ignored_2 = this.bi >> 2;
            if (this.bi == 0) {
                this.bj = 0;
                this.bo = 0;
            } else {
                this.bj = (n5 - n2) / this.bi;
                this.bo = (this.bp - this.bq) / this.bi;
            }
            this.bi >>= 1;
            while (this.bi > 0) {
                this.cd[n++ + (n2 >> 16) * 300] = (byte)(this.bq >> 16);
                this.cd[n4-- + (n5 >> 16) * 300] = (byte)(this.bp >> 16);
                n2 += this.bj;
                n5 -= this.bj;
                this.bq += this.bo;
                this.bp -= this.bo;
                --this.bi;
            }
            this.cd[n + (n2 >> 16) * 300] = (byte)(this.bq >> 16);
            this.cd[n4 + (n5 >> 16) * 300] = (byte)(this.bp >> 16);
        } else {
            if (n2 > n5) {
                byte cfr_ignored_3 = (byte)n4;
                this.bi = n;
                n = n4;
                n4 = this.bi;
                this.bi = n2;
                n2 = n5;
                n5 = this.bi;
                this.bi = this.bq;
                this.bq = this.bp;
                this.bp = this.bi;
                int cfr_ignored_4 = this.bi >> 4;
            }
            this.bi = n5 - n2;
            int cfr_ignored_5 = (n4 <<= 16) + (n <<= 16);
            this.bq <<= 16;
            this.bp <<= 16;
            if (this.bi == 0) {
                this.bj = 0;
                this.bo = 0;
            } else {
                this.bj = (n4 - n) / this.bi;
                this.bo = (this.bp - this.bq) / this.bi;
            }
            n2 *= 300;
            n5 *= 300;
            this.bi >>= 1;
            int cfr_ignored_6 = this.bi + n;
            while (this.bi > 0) {
                int cfr_ignored_7 = -this.bi;
                this.cd[n2 + (n >> 16)] = (byte)(this.bq >> 16);
                this.cd[n5 + (n4 >> 16)] = (byte)(this.bp >> 16);
                n += this.bj;
                n4 -= this.bj;
                this.bq += this.bo;
                this.bp -= this.bo;
                n2 += 300;
                n5 -= 300;
                --this.bi;
            }
            this.cd[n2 + (n >> 16)] = (byte)(this.bq >> 16);
            this.cd[n5 + (n4 >> 16)] = (byte)(this.bp >> 16);
        }
    }

    public Particules() {
        double[][] cfr_ignored_0 = new double[4][10];
    }
}

