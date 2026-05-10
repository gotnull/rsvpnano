/*
 * Decompiled with CFR 0.152.
 */
public class c {
    public a aq;
    public float ap;
    public float ao;
    public float an;
    public float am;
    public float al;
    public float ak;
    public float aj;
    public float ai;
    public float ah;
    public float ag;
    public float af;
    public float ae;
    public float ad;
    public float ac;
    public float ab;
    public float w;
    public float v;
    public float u;
    public float t;
    public float s;
    public float r;
    public float q;
    public float p;
    public float o;
    public float n;
    public int m;

    public final void c(a[] aArray, int n, float f2, float f3, float f4, float f5, float f6, float f7, b[] bArray, int n2, int n3) {
        this.ap = (float)Math.cos(f4);
        this.ao = (float)Math.sin(f4);
        this.an = (float)Math.cos(f3);
        this.am = (float)Math.sin(f3);
        this.al = (float)Math.cos(f2);
        this.ak = (float)Math.sin(f2);
        this.ac = this.ap * this.an;
        this.ab = -this.ap * this.am * this.ak + this.ao * this.al;
        this.w = this.ap * this.am * this.al + this.ao * this.ak;
        this.v = -this.ao * this.an;
        this.u = this.ao * this.am * this.ak + this.ap * this.al;
        this.t = -this.ao * this.am * this.al + this.ap * this.ak;
        this.s = -this.am;
        this.r = -this.an * this.ak;
        this.q = this.an * this.al;
        this.m = 0;
        while (this.m < n) {
            this.aq = aArray[this.m];
            this.af = this.aq.i;
            this.ae = this.aq.h;
            this.ad = this.aq.g;
            this.ai = this.ac * this.af + this.ab * this.ae + this.w * this.ad + f5;
            this.ag = this.v * this.af + this.u * this.ae + this.t * this.ad + f6;
            this.ah = this.s * this.af + this.r * this.ae + this.q * this.ad + f7;
            this.aj = 400.0f / (400.0f - this.ah);
            bArray[this.m].i = (int)(this.ai * this.aj + (float)(n2 / 2));
            bArray[this.m].h = (int)(-this.ag * this.aj + (float)(n3 / 2));
            bArray[this.m].g = (int)this.ah;
            ++this.m;
        }
    }

    public final void b(a[] aArray, int n, float[] fArray, float f2, float f3, float f4) {
        this.ap = (float)Math.cos(f4);
        this.ao = (float)Math.sin(f4);
        this.an = (float)Math.cos(f3);
        this.am = (float)Math.sin(f3);
        this.al = (float)Math.cos(f2);
        this.ak = (float)Math.sin(f2);
        this.ac = this.an * this.ap;
        this.ab = this.an * this.ao;
        this.w = this.am;
        this.v = -this.al * this.ao - this.ap * this.am * this.ak;
        this.u = this.al * this.ap - this.ak * this.am * this.ao;
        this.t = this.ak * this.an;
        this.s = this.ao * this.ak - this.al * this.am * this.ap;
        this.r = -this.ak * this.ap - this.al * this.am * this.ao;
        this.q = this.al * this.an;
        this.p = fArray[0];
        this.o = fArray[1];
        this.n = fArray[2];
        this.ai = this.ac * this.p + this.ab * this.o + this.w * this.n;
        this.ag = this.v * this.p + this.u * this.o + this.t * this.n;
        this.ah = this.s * this.p + this.r * this.o + this.q * this.n;
        fArray[0] = this.ai;
        fArray[1] = this.ag;
        fArray[2] = this.ah;
        this.m = 0;
        while (this.m < n) {
            byte cfr_ignored_0 = (byte)this.p;
            this.aq = aArray[this.m];
            this.p = this.aq.f;
            this.o = this.aq.e;
            this.n = this.aq.d;
            this.aq.f = this.ac * this.p + this.ab * this.o + this.w * this.n;
            this.aq.e = this.v * this.p + this.u * this.o + this.t * this.n;
            this.aq.d = this.s * this.p + this.r * this.o + this.q * this.n;
            this.p = this.aq.l;
            this.o = this.aq.k;
            this.n = this.aq.j;
            this.aq.l = this.ac * this.p + this.ab * this.o + this.w * this.n;
            this.aq.k = this.v * this.p + this.u * this.o + this.t * this.n;
            this.aq.j = this.s * this.p + this.r * this.o + this.q * this.n;
            ++this.m;
        }
    }

    public final void a(a[] aArray, int n, float f2, float f3, float f4) {
        this.ap = (float)Math.cos(f4);
        this.ao = (float)Math.sin(f4);
        this.an = (float)Math.cos(f3);
        this.am = (float)Math.sin(f3);
        this.al = (float)Math.cos(f2);
        this.ak = (float)Math.sin(f2);
        this.ac = this.ap * this.an;
        this.ab = -this.ap * this.am * this.ak + this.ao * this.al;
        this.w = this.ap * this.am * this.al + this.ao * this.ak;
        this.v = -this.ao * this.an;
        this.u = this.ao * this.am * this.ak + this.ap * this.al;
        this.t = -this.ao * this.am * this.al + this.ap * this.ak;
        this.s = -this.am;
        this.r = -this.an * this.ak;
        this.q = this.an * this.al;
        this.m = 0;
        while (this.m < n) {
            int cfr_ignored_0 = this.m >> 1;
            this.aq = aArray[this.m];
            this.p = this.aq.f;
            this.o = this.aq.e;
            this.n = this.aq.d;
            this.aq.f = this.ac * this.p + this.ab * this.o + this.w * this.n;
            this.aq.e = this.v * this.p + this.u * this.o + this.t * this.n;
            this.aq.d = this.s * this.p + this.r * this.o + this.q * this.n;
            this.p = this.aq.l;
            this.o = this.aq.k;
            this.n = this.aq.j;
            this.aq.l = this.ac * this.p + this.ab * this.o + this.w * this.n;
            this.aq.k = this.v * this.p + this.u * this.o + this.t * this.n;
            this.aq.j = this.s * this.p + this.r * this.o + this.q * this.n;
            ++this.m;
        }
    }

    public c() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
        this.aq = new a();
    }
}

