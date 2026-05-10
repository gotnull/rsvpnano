/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.Toolkit;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;
import java.awt.image.PixelGrabber;
import java.net.URL;

public class Scrolls88
extends Applet
implements Runnable {
    public String br = "bllsap57*dge";
    public String bq = "bllsap/4-4,fec";
    public String bp = ",/.^.+ehb";
    public String bo = "anvkkdm-cfd";
    public byte[] bn;
    public int bm;
    public int bl;
    public int bk;
    public int bj;
    public int bi;
    public int bh;
    public int bg;
    public int bf;
    public int be;
    public int bd;
    public int bc;
    public int ba;
    public int a9;
    public int a8;
    public int a7;
    public int[] a6;
    public byte[] a5;
    public int a4;
    public byte[] a3;
    public Thread a2;
    public Image a1;
    public Image a0;
    public static int a_;
    public static int az;
    public byte[] ay;
    public int[] ax;
    public MemoryImageSource aw;
    public static int[] av;
    public long au;
    public long at;
    public long as;
    public long ar;
    public int aq;
    public byte[] ap;
    public byte[] ao;
    public byte[] an;
    public int am = 1;
    public long al;
    public byte[] ak;
    public int aj;
    public int ai;
    public int ah;
    public byte[] ag;
    public byte[] af;
    public int ae;
    public int[] ad;
    public byte[] ac;
    public float ab;
    public float aa;
    public float z;
    public float v;
    public byte[] u;
    public int t;
    public int[] s;
    public byte[] q;
    public byte[] p;
    public byte[] o;
    public int[] n;
    public int[] m;
    public int[] l;
    public int[] k;
    public int j;
    public a i;
    public int f;
    public int e;
    public byte[] d;
    public byte[] c;
    public int[] a;

    /*
     * Unable to fully structure code
     */
    public final void init() {
        block50: {
            this.i = new a();
            new byte[4160];
            Scrolls88.av = new int[1350];
            this.a8 = Integer.parseInt(this.getParameter("eqx"));
            this.d = new byte[3300];
            this.c("\u00eb\u00f6_/\u00eb/#");
            if (this.a7 == 2) {
                this.c(8, 100);
                this.bo = this.c(-1, this.bo);
            }
            var1_1 = 48960;
            this.ak = new byte[64000];
            this.bd = this.d(this.i.bt);
            this.bc = this.d(this.i.bu);
            this.ba = this.d(this.i.bs);
            this.a9 = this.d(this.i.bv);
            System.gc();
            switch (200) {
                case 0: {
                    var2_2 = new PixelGrabber(this.a1, 0, 0, 15, 15, Scrolls88.av, 0, 15);
                    try {
                        var2_2.grabPixels();
                    }
                    catch (Exception var3_3) {
                        var3_3.printStackTrace();
                    }
                    break;
                }
                case 1: {
                    break;
                }
            }
            var2_2 = new int[2];
            var2_2[0] = 1;
            this.a2 = null;
            this.setLayout(null);
            this.addNotify();
            this.getToolkit();
            var3_4 = 0;
            var4_5 = 0;
            while (var4_5 < 17) {
                var3_4 += "THE EQUINOX TOUCH".charAt(var4_5);
                ++var4_5;
            }
            while (var3_4 != 1229) {
            }
            this.ay = new byte[64000];
            this.p = new byte[64000];
            var5_6 = 0;
            while (var5_6 < 64000) {
                (byte)var5_6;
                this.p[var5_6] = 0;
                this.ay[var5_6] = 0;
                this.ak[var5_6] = 0;
                ++var5_6;
            }
            this.getToolkit();
            System.gc();
            this.ap = new byte[256];
            this.ao = new byte[256];
            this.an = new byte[256];
            this.n = new int[256];
            this.m = new int[256];
            this.l = new int[256];
            if (this.a7 == 1) {
                this.aj = 0;
                while (this.aj < 8) {
                    (byte)(this.aj >> 1);
                    this.n[this.aj] = 255;
                    this.m[this.aj] = 255;
                    this.l[this.aj] = this.aj * 32;
                    ++this.aj;
                }
                this.aj = 8;
                while (this.aj < 16) {
                    this.n[this.aj] = (15 - this.aj) * 32;
                    this.m[this.aj] = 255;
                    this.l[this.aj] = 255;
                    ++this.aj;
                }
                this.aj = 16;
                while (this.aj < 24) {
                    this.n[this.aj] = (this.aj - 16) * 32;
                    this.m[this.aj] = (23 - this.aj) * 32;
                    this.l[this.aj] = 255;
                    ++this.aj;
                }
                this.aj = 24;
                while (this.aj < 32) {
                    this.n[this.aj] = 255;
                    this.m[this.aj] = (this.aj - 24) * 32;
                    this.l[this.aj] = (31 - this.aj) * 32;
                    ++this.aj;
                }
                var6_7 = 20;
                var7_8 = 1;
                while (var7_8 < 7) {
                    this.aj = 0;
                    while (this.aj < 32) {
                        (byte)(var7_8 * this.aj);
                        this.n[this.aj + 32 * var7_8] = this.n[this.aj + 32 * (var7_8 - 1)] > var6_7 ? this.n[this.aj + 32 * (var7_8 - 1)] - var6_7 : 0;
                        this.m[this.aj + 32 * var7_8] = this.m[this.aj + 32 * (var7_8 - 1)] > var6_7 ? this.m[this.aj + 32 * (var7_8 - 1)] - var6_7 : 0;
                        this.l[this.aj + 32 * var7_8] = this.l[this.aj + 32 * (var7_8 - 1)] > var6_7 ? this.l[this.aj + 32 * (var7_8 - 1)] - var6_7 : 0;
                        ++this.aj;
                    }
                    ++var7_8;
                }
                this.aj = 0;
                while (this.aj < 256) {
                    if (this.n[this.aj] < 235) {
                        v0 = this.aj;
                        this.n[v0] = this.n[v0] + 20;
                    } else {
                        this.n[this.aj] = 255;
                    }
                    if (this.m[this.aj] < 250) {
                        v1 = this.aj;
                        this.m[v1] = this.m[v1] + 5;
                    } else {
                        this.m[this.aj] = 255;
                    }
                    if (this.l[this.aj] > 20) {
                        v2 = this.aj;
                        this.l[v2] = this.l[v2] - 20;
                    } else {
                        this.l[this.aj] = 0;
                    }
                    ++this.aj;
                }
                this.aj = 0;
                while (this.aj < 256) {
                    this.ap[this.aj] = (byte)this.n[this.aj];
                    this.ao[this.aj] = (byte)this.m[this.aj];
                    this.an[this.aj] = (byte)this.l[this.aj];
                    ++this.aj;
                }
                this.ap[0] = 0;
                this.ao[0] = 0;
                this.an[0] = 0;
                this.aj = 0;
                while (this.aj < 8) {
                    (byte)(this.aj - 1);
                    this.ap[this.aj + 1] = 48;
                    this.ao[this.aj + 1] = (byte)(255 - this.aj * 16 - 48 + 16);
                    this.an[this.aj + 1] = (byte)(255 - this.aj * 12);
                    ++this.aj;
                }
                this.u = new byte[1000];
                this.j(this.u);
                this.ap[128] = 0;
                this.ao[128] = 0;
                this.an[128] = 0;
                this.aj = 129;
                while (this.aj < 137) {
                    this.ap[this.aj] = 0;
                    this.ao[this.aj] = (byte)((this.aj - 129) * 31);
                    this.an[this.aj] = (byte)((this.aj - 129) * 31);
                    this.ap[this.aj + 8] = (byte)((this.aj - 129) * 31);
                    this.ao[this.aj + 8] = (byte)((this.aj - 129) * 31);
                    this.an[this.aj + 8] = 0;
                    this.ap[this.aj + 16] = (byte)((this.aj - 129) * 31);
                    this.ao[this.aj + 16] = 0;
                    this.an[this.aj + 16] = (byte)((this.aj - 129) * 31);
                    ++this.aj;
                }
                System.gc();
                var1_1 = 0;
                this.aj = 64;
                while (this.aj < 124) {
                    this.aj * 3;
                    this.ap[this.aj] = (byte)(this.a[var1_1++] / 2 + 128);
                    this.ao[this.aj] = (byte)(this.a[var1_1++] * 6 / 7);
                    this.an[this.aj] = (byte)(this.a[var1_1++] * 6 / 7);
                    ++this.aj;
                }
                this.ao[64] = -1;
                this.an[64] = -1;
                var8_9 = 10;
                while (var8_9 < 20) {
                    var8_9 * 2;
                    this.ap[var8_9] = (byte)((var8_9 - 10) * 27);
                    this.ao[var8_9] = -1;
                    this.an[var8_9] = (byte)((var8_9 - 10) * 27);
                    ++var8_9;
                }
                this.aj = 130;
                while (this.aj < 140) {
                    this.aj * 2;
                    this.ap[this.aj] = (byte)((this.aj - 130) * 25);
                    this.ao[this.aj] = -1;
                    this.an[this.aj] = -1;
                    this.ap[this.aj + 10] = -1;
                    this.ao[this.aj + 10] = (byte)(255 - (this.aj - 130) * 12);
                    this.an[this.aj + 10] = -1;
                    this.ap[this.aj + 20] = -1;
                    this.ao[this.aj + 20] = -128;
                    this.an[this.aj + 20] = (byte)(255 - (this.aj - 130) * 12);
                    ++this.aj;
                }
                this.br = this.c(-1, this.br);
                this.bq = this.c(-1, this.bq);
            } else {
                this.c(3);
            }
            this.t();
            this.al = 0L;
            this.as = 6L;
            this.ar = 1L;
            this.f = 0;
            this.e = 0;
            if (this.a7 == 1) {
                this.b(this.k);
                System.gc();
                this.m();
                this.s = new int[24000];
                this.t = 0;
                this.aj = 0;
                while (this.aj < 24000) {
                    (float)(30 + this.aj / 2 % 12200 + (int)(Math.sin((double)this.aj / 30.0) * 15.0));
                    this.s[this.aj] = 30 + this.aj / 2 % 12200 + (int)(Math.sin((double)this.aj / 30.0) * 30.0);
                    (float)this.aj * 0.9f;
                    ++this.aj;
                }
                this.q = new byte[134400];
                this.h();
                this.ad = new int[1002];
                this.ae = 0;
                this.aj = 0;
                while (this.aj < 1000) {
                    this.ad[this.aj] = (int)(Math.sin(0.012566f * (float)this.aj * 5.0f) * 40.0 + 100.0 - 8.0);
                    ++this.aj;
                }
            }
            this.s(this.c, this.d);
            var1_1 = 0;
            this.aj = 3298;
            while (this.aj >= 0) {
                (int)(Math.sin(0.012566f * (float)this.aj * 5.0f) * 10.0);
                if (this.d[this.aj] != 0) {
                    if (this.a7 == 1) {
                        this.d[this.aj + 1] = 64;
                    }
                    this.d[this.aj] = 64;
                }
                this.aj >> 2;
                --this.aj;
            }
            if (this.a7 == 2) {
                this.aj = 2999;
                while (this.aj >= 0) {
                    (int)(Math.cos(0.012566f * (float)this.aj * 5.0f) * 10.0);
                    if (this.d[this.aj] != 0 && this.d[this.aj + 301] == 0) {
                        this.d[this.aj + 301] = 1;
                    }
                    this.aj >> 2;
                    --this.aj;
                }
            }
            if (this.bi != 57362) break block50;
            if (this.bh != 58708 || this.bg != 68531 || this.bf != 32668) break block50;
            switch (this.a8) {
                case 1: {
                    (byte)this.a8;
                    this.af = new byte[200];
                    this.aj = 0;
                    while (this.aj < 96) {
                        this.af[this.aj + 100 - 8 - 40] = (byte)(Math.sin(3.1415f * (float)this.aj / 96.0f) * 5.0 + 15.0);
                        ++this.aj;
                    }
                    this.ag = new byte[200];
                    this.aj = 0;
                    while (this.aj < 200) {
                        this.ag[this.aj] = (byte)(this.aj / 8 + 25);
                        ++this.aj;
                    }
                    System.gc();
                    return;
                }
                case 2: {
                    this.a5 = new byte[10752];
                    this.ad = new int[1002];
                    this.a6 = new int[1002];
                    this.ae = 0;
                    this.a4 = 0;
                    this.aj = 0;
                    while (this.aj < 1000) {
                        this.ad[this.aj] = (int)(Math.sin(0.01256600022315979 * (double)this.aj * 2.0) * 60.0 + 100.0 - 8.0);
                        this.a6[this.aj] = (int)(Math.sin(0.012566f * (float)this.aj) * 31.0 + 32.0);
                        ++this.aj;
                    }
                    this.u();
                    this.bp = this.c(-1, this.bp);
                    this.a(this.ay, 0);
                    this.aj = 0;
                    while (this.aj < 8) {
                        this.bk = 0;
                        if (true) ** GOTO lbl427
                        do {
                            this.bj = 0;
                            while (this.bj < 7) {
                                System.arraycopy(this.ay, this.bj * 320, this.ak, 7 * this.aj * 384 + this.bk * 8 + this.bj * 384, 8);
                                ++this.bj;
                            }
                            ++this.bk;
lbl427:
                            // 2 sources

                        } while (this.bk < 48);
                        ++this.aj;
                    }
                    this.m(2);
                    System.gc();
                    return;
                }
            }
        }
    }

    public final void destroy() {
        this.a0.flush();
        this.ay = null;
        this.ak = null;
    }

    public final void start() {
        if (this.a2 == null) {
            this.a2 = new Thread(this);
            this.a2.start();
        }
    }

    public final void stop() {
        if (this.a2 != null) {
            this.a2.stop();
            this.a2 = null;
        }
    }

    public final void run() {
        while (this.a2 != null) {
            this.aw.newPixels(0, 0, 320, 200, true);
            this.v();
            this.repaint();
            try {
                Thread.sleep(this.as);
            }
            catch (InterruptedException interruptedException) {}
            this.w();
        }
    }

    public final void update(Graphics graphics) {
        if (this.a0 != null) {
            graphics.drawImage(this.a0, 0, 0, 320, 200, this);
            Toolkit.getDefaultToolkit().sync();
        }
    }

    private final void w() {
        ++this.al;
        this.at = System.currentTimeMillis();
        if (this.at - this.au > 1000L) {
            this.al = (int)(1000L * this.al / (this.at - this.au));
            this.au = this.at;
            if (this.aq > 1 && this.ar < 0L && this.aq < 12) {
                if (this.al <= 10L) {
                    this.as += -4L;
                }
                if (this.al <= 20L && this.al > 10L) {
                    this.as += -3L;
                }
                if (this.al <= 30L && this.al > 20L) {
                    this.as += -2L;
                }
                if (this.al <= 60L && this.al >= 30L) {
                    this.as += -1L;
                }
                if (this.al >= 80L) {
                    this.as += (long)((int)((this.al - 75L) / 10L));
                }
                if (this.as > 14L) {
                    this.as = 14L;
                }
            }
            if (this.aq == 14) {
                if (this.al > 80L) {
                    this.as += -1L;
                } else if (this.al < 60L) {
                    this.as += -1L;
                }
            }
            if (this.as < 1L) {
                this.as = 1L;
            }
            System.gc();
            ++this.aq;
            this.al = 0L;
            this.ar = -this.ar;
        }
    }

    private final void v() {
        int n;
        int n2 = 0;
        if (this.bi == 57362 && this.bh == 58708 && this.bg == 68531 && this.bf == 32668) {
            switch (this.a7) {
                case 1: {
                    n2 = 0;
                    while (n2 < 22) {
                        n = 0;
                        while (n < 8) {
                            int cfr_ignored_0 = 320 * (n + 9 * n2 + 1);
                            int cfr_ignored_1 = 4200 * 3;
                            System.arraycopy(this.q, n * 8400 + this.s[this.t++], this.ay, 320 * (n + 9 * n2 + 1), 320);
                            ++n;
                        }
                        System.arraycopy(this.ak, 0, this.ay, 320 * (n + 9 * n2 + 1), 320);
                        ++n2;
                    }
                    System.arraycopy(this.ak, 0, this.ay, 0, 320);
                    break;
                }
                case 2: {
                    this.v(32);
                    return;
                }
                case 3: {
                    n2 = 0;
                    while (n2 < 5) {
                        n = 0;
                        while (n < 8) {
                            int cfr_ignored_2 = 320 * (n + 9 * n2 + 1);
                            System.arraycopy(this.q, n * 8400 + this.s[this.t++], this.ay, 320 * (n + 9 * n2 + 1), 320);
                            ++n;
                        }
                        System.arraycopy(this.ak, 0, this.ay, 320 * (n + 9 * n2 + 1), 320);
                        ++n2;
                    }
                    break;
                }
            }
        }
        this.t -= 174;
        if (this.t > 12000) {
            this.t = 0;
        }
        n2 = 0;
        while (n2 < 60) {
            n = (int)(Math.cos((double)this.ab + (double)((float)n2 / 60.0f) * 12.566370964050293) * Math.sin(this.v) * 20.0 + Math.cos((double)(2.0f * this.aa) / 2.0 - (double)((float)n2 / 60.0f * 6.28f)) * 40.0 - Math.sin((double)(this.aa / 2.0f + this.z) + (double)((float)n2 / 60.0f) * 6.283185005187988) * 60.0) + 160 - 4;
            int n3 = (int)(Math.cos((double)(this.aa * 1.5f) + (double)((float)n2 / 60.0f) * 6.283185005187988) * Math.sin(this.z) * 20.0 - Math.sin((double)(4.0f * this.ab) / 3.0 + (double)((float)n2 / 60.0f * 6.28f * 2.0f)) * 40.0 + Math.sin((double)(this.ab / 2.0f + this.v) + (double)((float)n2 / 60.0f) * 6.283185005187988) * 20.0) + 100;
            this.a(this.ay, n + n3 * 320);
            byte cfr_ignored_3 = (byte)this.aa;
            ++n2;
        }
        this.ab = (float)((double)this.ab + (double)0.02f);
        this.aa = (float)((double)this.aa + (double)0.0146f);
        this.z = (float)((double)this.z + (double)0.0182f);
        this.v = (float)((double)this.v + (double)0.0206f);
        this.i();
        this.k();
        n2 = 0;
        int n4 = 642;
    }

    private final void v(int n) {
        if (this.bi == 57362 && this.bh == 58708 && this.bg == 68531) {
            if (this.bf == 32668) {
                this.k(2);
            }
        }
        this.q(this.ay, this.d);
        if (this.bm < 196) {
            ++this.bm;
        }
    }

    private final void u() {
        this.ax = new int[64000];
        int n = 0;
        float f = 0.0f;
        float f2 = 0.07854f;
        f2 = 0.0f;
        int n2 = -100;
        while (n2 < 100) {
            int n3 = -160;
            while (n3 < 160) {
                int n4;
                int n5;
                f = 0.0f;
                float f3 = (float)Math.sqrt(n3 * n3 + n2 * n2);
                if (f3 <= 99.0f) {
                    f = (float)(Math.cos(3.14159f * (f3 - 0.0f) / 100.0f) * 50.0 + 50.0);
                    float f4 = 1.0f / (400.0f - f);
                    n5 = (int)(400.0 * (double)n3 * (double)f4);
                    n4 = (int)(400.0 * (double)n2 * (double)f4);
                    f2 = (float)((-Math.cos(3.14159f * (f3 - 0.0f) / 100.0f) - 1.0) / 6.0);
                } else {
                    n5 = n3;
                    n4 = n2;
                    f2 = 0.0f;
                }
                int n6 = (int)((double)n5 * Math.cos(f2) - (double)n4 * Math.sin(f2));
                int n7 = (int)((double)n5 * Math.sin(f2) + (double)n4 * Math.cos(f2));
                this.ax[n] = n6 + 160 + 320 * (n7 + 100);
                ++n;
                ++n3;
            }
            ++n2;
        }
    }

    private final void t() {
        int n;
        switch (this.am) {
            case 0: {
                IndexColorModel indexColorModel = new IndexColorModel(24, 65288, this.an, this.ap, this.an);
                this.aw = new MemoryImageSource(320, 200, (ColorModel)indexColorModel, this.ay, 0, 320);
                this.aw.setAnimated(false);
                break;
            }
            case 1: {
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.ap, this.ao, this.an);
                n = 0;
                while (n < this.bd) {
                    this.bi += this.c(this.i.bt, n);
                    ++n;
                }
                this.aw = new MemoryImageSource(320, 200, (ColorModel)indexColorModel, this.ay, 0, 320);
                this.aw.setAnimated(true);
                break;
            }
        }
        n = 0;
        while (n < this.bc) {
            this.bh += this.c(this.i.bu, n);
            ++n;
        }
        n = 0;
        while (n < this.ba) {
            this.bg += this.c(this.i.bs, n);
            ++n;
        }
        this.aw.setFullBufferUpdates(true);
        n = 0;
        while (n < this.a9) {
            this.bf += this.c(this.i.bv, n);
            ++n;
        }
        this.a0 = this.createImage(this.aw);
    }

    public final void s(byte[] byArray, byte[] byArray2) {
        int n = 0;
        while (n < 3300) {
            byArray2[n] = (byArray[n / 8] >> 7 - n % 8 & 1) == 1 ? -1 : 0;
            ++n;
        }
    }

    private final void r(URL uRL, String string, int n) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        a_ = image.getWidth(null);
        az = image.getHeight(null);
        int[] nArray = new int[a_ * az];
        n = 0;
        this.o = new byte[1792];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, a_, az, nArray, 0, a_);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        n = 0;
        while (n < 32) {
            int n2 = 0;
            while (n2 < 56) {
                int n3 = nArray[n2 + n * 56];
                if (n3 == -16777216) {
                    this.o[n2 + n * 56] = 0;
                }
                if (n3 != -16777216) {
                    this.o[n2 + n * 56] = (byte)(1 + n2 / 7);
                }
                ++n2;
            }
            ++n;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void q(byte[] byArray, byte[] byArray2) {
        int n = 322;
        int n2 = 0;
        int n3 = 0;
        while (n3 < 11) {
            int n4 = 0;
            while (++n4 < 75) {
                byte by;
                if ((by = byArray2[n2++]) != 0) {
                    byArray[n] = by;
                }
                ++n;
                if ((by = byArray2[n2++]) != 0) {
                    byArray[n] = by;
                }
                ++n;
                if ((by = byArray2[n2++]) != 0) {
                    byArray[n] = by;
                }
                ++n;
                if ((by = byArray2[n2++]) != 0) {
                    byArray[n] = by;
                }
                ++n;
            }
            n += 20;
            ++n3;
        }
    }

    private final void p(URL uRL, String string, int n) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        a_ = image.getWidth(null);
        az = image.getHeight(null);
        int[] nArray = new int[a_ * az];
        n = 0;
        this.ac = new byte[7905];
        int cfr_ignored_0 = 468 * 0;
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, a_, az, nArray, 0, a_);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        n = 0;
        while (n < 31) {
            int n2 = 0;
            while (n2 < 255) {
                int n3 = nArray[n2 + n * 255];
                if (n3 == -16777216) {
                    byte cfr_ignored_1 = (byte)(n3 - 1);
                    this.ac[n2 + n * 255] = 0;
                }
                if (n3 != -16777216) {
                    byte cfr_ignored_2 = (byte)n3;
                    this.ac[n2 + n * 255] = 1;
                }
                ++n2;
            }
            ++n;
        }
    }

    /*
     * Handled impossible loop by duplicating code
     * Enabled aggressive block sorting
     * Enabled unnecessary exception pruning
     * Enabled aggressive exception aggregation
     */
    private final void o(URL uRL, String string, int n) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        a_ = image.getWidth(null);
        az = image.getHeight(null);
        int[] nArray = new int[a_ * az];
        n = 0;
        this.a3 = new byte[55552];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, a_, az, nArray, 0, a_);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        n = 0;
        while (n < 31) {
            int n2 = 0;
            while (n2 < 32) {
                block11: {
                    int n3;
                    block10: {
                        n3 = 0;
                        if (!true) break block10;
                        int cfr_ignored_0 = 28 * 488;
                        if (n3 >= 28) break block11;
                    }
                    do {
                        int n4 = nArray[n * 32 + n2 + n3 * a_];
                        if (n4 == -16777216) {
                            byte cfr_ignored_1 = (byte)(n4 - 1);
                            this.a3[n2 + n3 * 32 + n * 1792] = 0;
                            this.a3[n2 + n3 * 32 + (n * 2 + 1) * 896] = 0;
                        }
                        if (n4 != -16777216) {
                            byte cfr_ignored_2 = (byte)n4;
                            this.a3[n2 + n3 * 32 + n * 1792] = (byte)((n4 & 0xFF) / 4);
                            this.a3[n2 + n3 * 32 + (n * 2 + 1) * 896] = (byte)((n4 & 0xFF) / 4);
                        }
                        ++n3;
                        int cfr_ignored_3 = 28 * 488;
                    } while (n3 < 28);
                }
                ++n2;
            }
            ++n;
        }
    }

    private final void n(URL uRL, String string, int n) {
        int n2;
        int n3;
        boolean bl;
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n4 = 6;
        int n5 = 6;
        a_ = image.getWidth(null);
        az = image.getHeight(null);
        int[] nArray = new int[a_ * az];
        n = 0;
        this.bn = new byte[15680];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, a_, az, nArray, 0, a_);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        int[] nArray2 = new int[54];
        int n6 = 1;
        while (n6 < 18) {
            nArray2[n6] = -1;
            ++n6;
        }
        nArray2[0] = 0;
        n6 = 0;
        while (n6 < 15680) {
            if (nArray[n6] != -16777216) {
                bl = false;
                n3 = nArray[n6];
                n2 = 1;
                while (n2 < 18) {
                    if (!bl) {
                        if (nArray2[n2] == -1) {
                            nArray2[n2] = n3;
                            bl = true;
                        }
                        if (nArray2[n2] == n3) {
                            this.bn[n6] = (byte)(n2 + 127);
                            bl = true;
                        }
                    }
                    ++n2;
                }
            } else {
                this.bn[n6] = 0;
            }
            ++n6;
        }
        n6 = 0;
        while (n6 < 15680) {
            if (nArray[n6] != -16777216) {
                bl = false;
                n3 = nArray[n6];
                n2 = 1;
                while (n2 < 18) {
                    if (!bl) {
                        if (nArray2[n2] == -1) {
                            nArray2[n2] = n3;
                            bl = true;
                        }
                        if (nArray2[n2] == n3) {
                            this.bn[n6] = (byte)(n2 + 127);
                            bl = true;
                        }
                    }
                    ++n2;
                }
            } else {
                this.bn[n6] = 0;
            }
            ++n6;
        }
        n6 = 1;
        while (n6 < 18) {
            this.ap[128 + n6 - 1] = (byte)((nArray2[n6] & 0xFF0000) >> 16);
            this.ao[128 + n6 - 1] = (byte)((nArray2[n6] & 0xFF00) >> 8);
            this.an[128 + n6 - 1] = (byte)(nArray2[n6] & 0xFF);
            ++n6;
        }
        System.gc();
    }

    public final void m() {
        this.r(this.getDocumentBase(), this.br, 1);
        this.p(this.getDocumentBase(), this.bq, 1);
        this.showStatus("Scrollers applet");
    }

    public final void m(int n) {
        this.o(this.getDocumentBase(), this.bp, n);
        this.showStatus("MegaDist");
    }

    /*
     * Handled impossible loop by duplicating code
     * Enabled aggressive block sorting
     */
    private final void k() {
        int n = 0;
        if (this.f < 0) {
            this.f = 0;
        }
        this.e -= 2;
        if (this.e <= -15) {
            this.e = 0;
            ++this.f;
            ++this.ae;
        }
        int n2 = 0;
        while (n2 < 21) {
            int n3;
            block15: {
                block17: {
                    int n4;
                    block16: {
                        byte cfr_ignored_0 = (byte)n2;
                        n3 = this.i.bu.charAt(this.f++);
                        if (this.f < this.i.bu.length()) break block15;
                        n4 = 0;
                        if (!true) break block16;
                        int cfr_ignored_1 = 7 * 748;
                        if (n4 >= 7) break block17;
                    }
                    do {
                        ++n4;
                        int cfr_ignored_2 = 7 * 748;
                    } while (n4 < 7);
                }
                this.f = 0;
                n = 0;
            }
            ++n;
            if (n3 == 32) {
                n3 = 91;
            } else if (n3 == 46) {
                n3 = 92;
            } else if (n3 == 33) {
                n3 = 93;
            } else if (n3 == 39) {
                n3 = 94;
            } else if (n3 == 44) {
                n3 = 95;
            }
            this.g(n2, this.e, (n3 -= 65) * 255, this.ad[this.ae++], this.ay, this.ac);
            ++n2;
        }
        this.ae -= 20;
        if (this.ae >= 500) {
            this.ae -= 500;
        }
        this.f -= n;
        this.q(this.ay, this.d);
    }

    private final void l(byte[] byArray, byte[] byArray2) {
        int n = 0;
        int n2 = (196 - this.bm) * 320;
        int n3 = 0;
        while (n3 < this.bm) {
            int n4 = 0;
            while (n4 < 20) {
                byte by = byArray2[n++];
                if (by != 0) {
                    byArray[n2] = by;
                }
                ++n2;
                if ((by = byArray2[n++]) != 0) {
                    byArray[n2] = by;
                }
                ++n2;
                if ((by = byArray2[n++]) != 0) {
                    byArray[n2] = by;
                }
                ++n2;
                if ((by = byArray2[n++]) != 0) {
                    byArray[n2] = by;
                }
                ++n2;
                ++n4;
            }
            n2 += 240;
            ++n3;
        }
    }

    private final void k(int n) {
        int n2;
        int n3 = 0;
        if (this.f < 0) {
            this.f = 0;
        }
        this.e -= 2;
        int cfr_ignored_0 = 578 * 2;
        if (this.e <= -32) {
            this.e = 0;
            ++this.f;
            this.ae += 7;
        }
        int n4 = 0;
        while (n4 < 13) {
            byte cfr_ignored_1 = (byte)n4;
            int n5 = this.i.bv.charAt(this.f++);
            if (this.f >= this.i.bv.length()) {
                n2 = 0;
                while (n2 < 7) {
                    ++n2;
                }
                this.f = 0;
                n3 = 0;
            }
            ++n3;
            if (n5 == 32) {
                n5 = 91;
            } else if (n5 == 46) {
                n5 = 92;
            } else if (n5 == 33) {
                n5 = 93;
            } else if (n5 == 39) {
                n5 = 94;
            } else if (n5 == 44) {
                n5 = 95;
            }
            this.f(n4, this.e, (n5 -= 65) * 1792, this.ad[this.ae], this.a5, this.a3);
            this.ae += 7;
            ++n4;
        }
        this.f -= n3;
        n2 = 0;
        while (n2 < 7) {
            n4 = 0;
            while (n4 < 28) {
                System.arraycopy(this.a5, n4 * 384 + this.a6[this.a4], this.ay, n4 * 320 + n2 * 8960, 320);
                ++this.a4;
                ++n4;
            }
            ++n2;
        }
        System.arraycopy(this.ak, this.ad[this.bl] / 2 % 28 * 384 + this.be, this.a5, 0, 10752);
        ++this.be;
        if (this.be >= 7) {
            this.be = 0;
        }
        ++this.bl;
        if (this.bl >= 500) {
            this.bl = 0;
        }
        this.a4 += -194;
        if (this.a4 >= 500) {
            this.a4 -= 500;
        }
        this.l(this.ay, this.bn);
        this.ae += -90;
        if (this.ae >= 500) {
            this.ae -= 500;
        }
    }

    private final void j(byte[] byArray) {
        int n = 0;
        int n2 = 0;
        while (n2 < 50) {
            byArray[n++] = -128;
            ++n2;
        }
        n2 = 129;
        while (n2 < 137) {
            byArray[n++] = (byte)n2;
            byArray[n++] = (byte)n2;
            ++n2;
        }
        n2 = 136;
        while (n2 > 128) {
            byArray[n++] = (byte)n2;
            byArray[n++] = (byte)n2;
            --n2;
        }
        n2 = 0;
        while (n2 < 100) {
            byArray[n++] = -128;
            ++n2;
        }
        n2 = 137;
        while (n2 < 145) {
            byArray[n++] = (byte)n2;
            byArray[n++] = (byte)n2;
            ++n2;
        }
        n2 = 144;
        while (n2 > 136) {
            byArray[n++] = (byte)n2;
            byArray[n++] = (byte)n2;
            --n2;
        }
        n2 = 0;
        while (n2 < 100) {
            byArray[n++] = -128;
            ++n2;
        }
        n2 = 145;
        while (n2 < 153) {
            byArray[n++] = (byte)n2;
            byArray[n++] = (byte)n2;
            ++n2;
        }
        n2 = 152;
        while (n2 > 144) {
            byArray[n++] = (byte)n2;
            byArray[n++] = (byte)n2;
            --n2;
        }
        n2 = 0;
        while (n2 < 50) {
            byArray[n++] = -128;
            ++n2;
        }
    }

    private final void i() {
        int n = 0;
        if (this.ai < 0) {
            this.ai = 0;
        }
        this.ah -= 2;
        if (this.ah <= -16) {
            this.ah = 0;
            ++this.ai;
        }
        int n2 = 0;
        while (n2 < 12) {
            float cfr_ignored_0 = (float)n2 / 3.0f;
            int n3 = this.i.bs.charAt(this.ai++);
            if (this.ai >= this.i.bs.length()) {
                int n4 = 0;
                while (n4 < 7) {
                    ++n4;
                }
                this.ai = 0;
                n = 0;
            }
            ++n;
            if (n3 == 32) {
                n3 = 91;
            } else if (n3 == 46) {
                n3 = 92;
            } else if (n3 == 33) {
                n3 = 93;
            } else if (n3 == 39) {
                n3 = 94;
            }
            this.e(n2, this.ah, (n3 -= 65) * 255 + 15, this.ay, this.ac);
            ++n2;
        }
        this.ai -= n;
    }

    private final void h() {
        this.f = 0;
        this.e = 0;
        int n = 0;
        while (n < 850) {
            int cfr_ignored_0 = n >> 1;
            int n2 = this.i.bt.charAt(this.f++);
            if (n2 == 32) {
                n2 = 91;
            } else if (n2 == 46) {
                n2 = 92;
            } else if (n2 == 33) {
                n2 = 93;
            } else if (n2 == 58) {
                n2 = 94;
            } else if (n2 == 44) {
                n2 = 95;
            } else if (n2 == 45) {
                n2 = 96;
            }
            n2 -= 65;
            int n3 = 0;
            while (n3 < 8) {
                byte cfr_ignored_1 = (byte)(n3 >> 1);
                int n4 = 0;
                while (n4 < 7) {
                    this.q[(this.f + 80) * 7 + n3 * 8400 + n4] = this.o[n2 * 56 + n4 + n3 * 7];
                    ++n4;
                }
                ++n3;
            }
            ++n;
        }
        this.f = 0;
        this.e = 0;
    }

    private final void g(int n, int n2, int n3, int n4, byte[] byArray, byte[] byArray2) {
        int n5 = n * 16 + n2;
        int n6 = 0;
        int n7 = n4;
        byte by = this.af[n7];
        if (n == 0) {
            int n8 = 0;
            while (n8 < 17) {
                int cfr_ignored_0 = n8 >> 2;
                n6 = (n8 + n4) * 320 + n5;
                int n9 = n8 * 15 + n3;
                int n10 = -n2;
                while (n10 < 15) {
                    byte by2 = byArray2[n9 + n10];
                    if (by2 != 0) {
                        byArray[n6 + n10] = by;
                    }
                    ++n10;
                }
                by = this.af[n7++];
                ++n8;
            }
            return;
        }
        if (n != 0 && n != 20) {
            int n11 = n3;
            int n12 = 0;
            while (n12 < 17) {
                int cfr_ignored_1 = n12 >> 3;
                n6 = (n12 + n4) * 320 + n5;
                int n13 = 0;
                while (n13 < 15) {
                    byte by3;
                    if ((by3 = byArray2[n11++]) != 0) {
                        byArray[n6 + n13] = by;
                    }
                    ++n13;
                }
                by = this.af[n7++];
                ++n12;
            }
            return;
        }
        if (n == 20 && n2 != 0) {
            int n14 = 0;
            while (n14 < 17) {
                n6 = (n14 + n4) * 320 + n5;
                int n15 = n14 * 15 + n3;
                int n16 = 0;
                while (n16 < -n2) {
                    byte by4 = byArray2[n15 + n16];
                    if (by4 != 0) {
                        byArray[n6 + n16] = by;
                    }
                    ++n16;
                }
                by = this.af[n7++];
                ++n14;
            }
        }
    }

    private final void f(int n, int n2, int n3, int n4, byte[] byArray, byte[] byArray2) {
        int n5 = n * 32 + n2;
        int n6 = 0;
        n3 += n4 % 28 * 32;
        if (n == 0) {
            int n7 = 0;
            while (n7 < 28) {
                int cfr_ignored_0 = n7 >> 2;
                n6 = n7 * 384 + n5;
                int n8 = n7 * 32 + n3;
                int n9 = -n2;
                while (n9 < 32) {
                    byte by = byArray2[n8 + n9];
                    if (by != 0) {
                        byArray[n6 + n9] = by;
                    }
                    ++n9;
                }
                ++n7;
            }
            return;
        }
        if (n != 0 && n != 12) {
            int n10 = n3;
            int n11 = 0;
            while (n11 < 28) {
                int cfr_ignored_1 = n11 >> 3;
                n6 = n11 * 384 + n5;
                int n12 = 0;
                while (n12 < 32) {
                    byte by;
                    if ((by = byArray2[n10++]) != 0) {
                        byArray[n6 + n12] = by;
                    }
                    ++n12;
                }
                ++n11;
            }
            return;
        }
        if (n == 12 && n2 != 0) {
            int n13 = 0;
            while (n13 < 28) {
                n6 = n13 * 384 + n5;
                int n14 = n13 * 32 + n3;
                int n15 = 0;
                while (n15 < -n2) {
                    byte by = byArray2[n14 + n15];
                    if (by != 0) {
                        byArray[n6 + n15] = by;
                    }
                    ++n15;
                }
                ++n13;
            }
        }
    }

    private final void e(int n, int n2, int n3, byte[] byArray, byte[] byArray2) {
        if (n == 0 && n2 > -15) {
            int n4 = 6;
            int n5 = 1936;
            int n6 = n3 - n2 * 15 + 15;
            int n7 = -n2;
            while (n7 < 16) {
                byte by = this.ag[n4++];
                int cfr_ignored_0 = n7 - n2;
                int n8 = 0;
                while (n8 < 15) {
                    if (byArray2[n6++] != 0) {
                        byArray[n5] = by;
                        byArray[n5 + 272] = by;
                    }
                    ++n5;
                    ++n8;
                }
                n5 += 305;
                ++n7;
            }
            return;
        }
        if (n != 0 && n < 11) {
            int n9 = 6 + n * 17 + n2;
            int n10 = 1936 + (n * 17 + n2) * 320;
            int n11 = n3;
            byte cfr_ignored_1 = (byte)n3;
            int n12 = 0;
            while (n12 < 16) {
                byte by = this.ag[n9++];
                int n13 = 0;
                while (n13 < 15) {
                    if (byArray2[n11++] != 0) {
                        byArray[n10] = by;
                        byArray[n10 + 272] = by;
                    }
                    ++n10;
                    ++n13;
                }
                n10 += 305;
                ++n12;
            }
            return;
        }
        if (n >= 11) {
            int n14 = 6 + n * 17 + n2;
            int n15 = 1936 + (n * 17 + n2) * 320;
            int n16 = n3 + 15;
            byte cfr_ignored_2 = (byte)(n3 + n16);
            int n17 = 0;
            while (n17 < -n2) {
                byte by = this.ag[n14++];
                int n18 = 0;
                while (n18 < 15) {
                    if (byArray2[n16++] != 0) {
                        byArray[n15] = by;
                        byArray[n15 + 272] = by;
                    }
                    ++n15;
                    ++n18;
                }
                n15 += 305;
                ++n17;
            }
        }
    }

    private final void c(String string) {
        this.c();
    }

    private final void c() {
        this.a7 = this.a8;
    }

    private final void c(int n, int n2) {
    }

    private final String c(int n, String string) {
        String string2 = " ";
        int n2 = string.length();
        char[] cArray = new char[n2];
        string.getChars(0, n2, cArray, 0);
        int n3 = 0;
        while (n3 < string.length()) {
            cArray[n3] = n == 1 ? (char)(cArray[n3] - 4 + n3 % 4) : (char)(cArray[n3] + 4 - n3 % 4);
            ++n3;
        }
        string2 = new String(cArray);
        return string2;
    }

    private final void c(int n) {
        this.aj = 0;
        while (this.aj < 64) {
            this.ap[this.aj] = (byte)(32.0 + (double)this.aj * 3.5 * (double)(1.0f - 0.1f * (1.0f - (float)this.aj / 63.0f)));
            this.ao[this.aj] = (byte)(32.0 + (double)this.aj * 3.5 * (double)(1.0f - 0.95f * (1.0f - (float)this.aj / 63.0f)));
            this.an[this.aj] = (byte)(32.0 + (double)this.aj * 3.5 * (double)(1.0f - 0.2f * (1.0f - (float)this.aj / 63.0f)));
            this.ap[127 - this.aj] = (byte)((double)this.aj * (double)2.8f);
            this.ao[127 - this.aj] = (byte)((double)this.aj * 3.0);
            this.an[127 - this.aj] = (byte)(this.aj * 4);
            ++this.aj;
        }
        this.ap[64] = -1;
        this.ao[64] = -1;
        this.an[64] = -1;
        this.ap[255] = -1;
        this.ao[255] = -1;
        this.an[255] = -1;
        this.n(this.getDocumentBase(), this.bo, n);
    }

    private final int d(String string) {
        return string.length();
    }

    private final int c(String string, int n) {
        return string.charAt(n);
    }

    private final void b(int[] nArray) {
        nArray = new int[this.j * 15];
        int n = 0;
        while (n < this.j) {
            int cfr_ignored_0 = nArray[n];
            nArray[n] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 2] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 3] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 4] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 5] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 6] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 7] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 8] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 9] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 10] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 11] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 12] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 13] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            nArray[n + this.j * 14] = (int)(152.0 + (Math.cos((double)6.28318f / (double)this.j * (double)n) + Math.sin((double)6.28318f / (double)this.j * (double)n * 2.0) * (double)1.24f) / 2.0 * 150.0);
            ++n;
        }
    }

    public Scrolls88() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
        int[] nArray = new int[]{123, 564, 89, 64, 679, 46, 41, 87};
        Image[] cfr_ignored_2 = new Image[30];
        this.j = 1000;
        byte[] byArray = new byte[413];
        byArray[3] = -128;
        byArray[5] = 64;
        byArray[17] = 1;
        byArray[21] = 8;
        byArray[22] = 64;
        byArray[24] = 1;
        byArray[29] = -128;
        byArray[33] = 8;
        byArray[37] = -128;
        byArray[38] = -64;
        byArray[40] = 8;
        byArray[41] = 4;
        byArray[42] = 4;
        byArray[44] = 1;
        byArray[45] = -15;
        byArray[46] = -62;
        byArray[47] = 19;
        byArray[48] = -104;
        byArray[49] = -121;
        byArray[50] = 8;
        byArray[51] = 64;
        byArray[55] = 16;
        byArray[56] = 68;
        byArray[59] = -124;
        byArray[66] = 8;
        byArray[69] = 16;
        byArray[71] = -128;
        byArray[73] = 16;
        byArray[74] = 8;
        byArray[75] = 12;
        byArray[78] = -128;
        byArray[79] = 64;
        byArray[80] = 64;
        byArray[82] = 16;
        byArray[83] = 34;
        byArray[84] = 33;
        byArray[85] = 17;
        byArray[86] = -120;
        byArray[87] = -120;
        byArray[88] = -124;
        byArray[92] = 1;
        byArray[93] = 4;
        byArray[94] = 64;
        byArray[96] = 16;
        byArray[97] = -128;
        byArray[104] = -128;
        byArray[106] = 1;
        byArray[108] = 8;
        byArray[110] = 1;
        byArray[111] = 1;
        byArray[112] = 1;
        byArray[113] = 35;
        byArray[114] = -57;
        byArray[115] = -120;
        byArray[116] = -17;
        byArray[117] = 7;
        byArray[118] = -120;
        byArray[119] = -127;
        byArray[120] = 4;
        byArray[121] = 18;
        byArray[122] = 17;
        byArray[123] = 20;
        byArray[124] = -112;
        byArray[125] = 68;
        byArray[126] = -128;
        byArray[130] = 30;
        byArray[131] = -1;
        byArray[132] = 60;
        byArray[133] = 65;
        byArray[134] = 8;
        byArray[135] = -29;
        byArray[136] = -56;
        byArray[137] = -109;
        byArray[138] = -57;
        byArray[139] = 17;
        byArray[140] = 7;
        byArray[141] = -119;
        byArray[142] = -57;
        byArray[143] = -121;
        byArray[144] = 60;
        byArray[145] = 7;
        byArray[146] = -125;
        byArray[147] = -61;
        byArray[148] = -68;
        byArray[149] = 16;
        byArray[150] = 18;
        byArray[151] = 34;
        byArray[152] = 68;
        byArray[153] = -111;
        byArray[154] = 64;
        byArray[155] = 68;
        byArray[156] = 80;
        byArray[157] = 31;
        byArray[158] = 65;
        byArray[159] = 33;
        byArray[160] = 17;
        byArray[161] = 73;
        byArray[162] = 4;
        byArray[163] = 48;
        byArray[167] = 1;
        byArray[168] = 20;
        byArray[169] = 66;
        byArray[170] = 36;
        byArray[171] = 33;
        byArray[172] = 17;
        byArray[173] = 68;
        byArray[174] = -119;
        byArray[175] = 34;
        byArray[176] = -120;
        byArray[177] = -96;
        byArray[178] = 68;
        byArray[179] = -126;
        byArray[180] = 68;
        byArray[181] = -119;
        byArray[183] = -120;
        byArray[184] = 34;
        byArray[185] = 69;
        byArray[186] = 2;
        byArray[187] = 1;
        byArray[188] = 34;
        byArray[189] = 36;
        byArray[190] = 73;
        byArray[191] = -12;
        byArray[192] = 4;
        byArray[193] = 69;
        byArray[194] = 1;
        byArray[195] = 4;
        byArray[196] = 18;
        byArray[197] = 17;
        byArray[198] = 18;
        byArray[199] = -112;
        byArray[200] = 67;
        byArray[205] = 17;
        byArray[206] = 68;
        byArray[207] = 34;
        byArray[208] = 2;
        byArray[209] = 17;
        byArray[210] = -12;
        byArray[211] = 72;
        byArray[212] = -110;
        byArray[213] = 40;
        byArray[214] = -124;
        byArray[215] = 4;
        byArray[216] = 73;
        byArray[217] = -28;
        byArray[218] = 79;
        byArray[219] = -111;
        byArray[220] = -56;
        byArray[221] = -126;
        byArray[222] = 39;
        byArray[223] = -48;
        byArray[224] = 32;
        byArray[225] = 63;
        byArray[226] = 34;
        byArray[227] = 68;
        byArray[228] = -112;
        byArray[229] = 64;
        byArray[230] = 68;
        byArray[231] = 80;
        byArray[232] = 16;
        byArray[233] = 65;
        byArray[234] = 33;
        byArray[235] = 17;
        byArray[236] = 41;
        byArray[237] = 4;
        byArray[238] = 72;
        byArray[242] = 1;
        byArray[243] = 20;
        byArray[244] = 66;
        byArray[245] = 32;
        byArray[246] = 66;
        byArray[247] = 16;
        byArray[248] = 68;
        byArray[249] = -119;
        byArray[250] = 34;
        byArray[251] = -120;
        byArray[252] = 64;
        byArray[253] = 68;
        byArray[254] = -94;
        byArray[255] = 68;
        byArray[256] = -127;
        byArray[258] = -120;
        byArray[259] = 34;
        byArray[260] = 65;
        byArray[261] = 4;
        byArray[262] = 2;
        byArray[263] = 18;
        byArray[264] = 36;
        byArray[265] = 73;
        byArray[266] = 20;
        byArray[267] = 4;
        byArray[268] = 66;
        byArray[269] = 1;
        byArray[270] = 2;
        byArray[271] = 34;
        byArray[272] = 17;
        byArray[273] = 17;
        byArray[274] = -120;
        byArray[275] = -120;
        byArray[276] = 64;
        byArray[280] = 17;
        byArray[281] = 68;
        byArray[282] = 34;
        byArray[283] = 68;
        byArray[284] = 33;
        byArray[285] = 20;
        byArray[286] = 72;
        byArray[287] = -110;
        byArray[288] = 40;
        byArray[289] = -118;
        byArray[290] = 36;
        byArray[291] = 74;
        byArray[292] = 36;
        byArray[293] = 72;
        byArray[294] = -112;
        byArray[295] = 8;
        byArray[296] = -110;
        byArray[297] = 36;
        byArray[298] = 80;
        byArray[299] = 64;
        byArray[300] = 33;
        byArray[301] = 60;
        byArray[302] = 120;
        byArray[303] = -114;
        byArray[304] = 48;
        byArray[305] = 120;
        byArray[306] = 32;
        byArray[307] = 31;
        byArray[308] = 28;
        byArray[309] = 30;
        byArray[310] = 57;
        byArray[311] = 24;
        byArray[312] = 112;
        byArray[313] = -124;
        byArray[317] = 1;
        byArray[318] = 19;
        byArray[319] = 51;
        byArray[320] = -60;
        byArray[321] = -124;
        byArray[322] = 14;
        byArray[323] = 60;
        byArray[324] = 121;
        byArray[325] = 34;
        byArray[326] = 113;
        byArray[327] = 18;
        byArray[328] = 120;
        byArray[329] = -98;
        byArray[330] = 68;
        byArray[331] = 112;
        byArray[332] = -64;
        byArray[333] = 121;
        byArray[334] = 34;
        byArray[335] = 56;
        byArray[336] = -56;
        byArray[338] = 2;
        byArray[339] = 4;
        byArray[343] = 2;
        byArray[346] = 64;
        byArray[357] = 32;
        byArray[358] = 8;
        byArray[359] = 64;
        byArray[361] = 64;
        byArray[365] = 4;
        byArray[374] = -128;
        byArray[376] = 32;
        byArray[377] = 64;
        byArray[381] = 64;
        byArray[383] = 3;
        byArray[394] = 2;
        byArray[398] = 4;
        byArray[403] = 64;
        this.c = byArray;
        float[] fArray = new float[]{123.0f, 564.0f, 89.0f, 64.2f, 679.0f, 46.0f, 41.0f, 87.0f};
        int[] nArray2 = new int[186];
        nArray2[0] = 255;
        nArray2[1] = 255;
        nArray2[2] = 255;
        nArray2[3] = 255;
        nArray2[4] = 222;
        nArray2[5] = 247;
        nArray2[6] = 255;
        nArray2[7] = 214;
        nArray2[8] = 231;
        nArray2[9] = 247;
        nArray2[10] = 181;
        nArray2[11] = 222;
        nArray2[12] = 239;
        nArray2[13] = 198;
        nArray2[14] = 222;
        nArray2[15] = 239;
        nArray2[16] = 165;
        nArray2[17] = 206;
        nArray2[18] = 231;
        nArray2[19] = 181;
        nArray2[20] = 206;
        nArray2[21] = 231;
        nArray2[22] = 148;
        nArray2[23] = 206;
        nArray2[24] = 222;
        nArray2[25] = 148;
        nArray2[26] = 189;
        nArray2[27] = 214;
        nArray2[28] = 165;
        nArray2[29] = 189;
        nArray2[30] = 214;
        nArray2[31] = 132;
        nArray2[32] = 189;
        nArray2[33] = 214;
        nArray2[34] = 123;
        nArray2[35] = 189;
        nArray2[36] = 206;
        nArray2[37] = 132;
        nArray2[38] = 181;
        nArray2[39] = 206;
        nArray2[40] = 107;
        nArray2[41] = 181;
        nArray2[42] = 198;
        nArray2[43] = 198;
        nArray2[44] = 198;
        nArray2[45] = 198;
        nArray2[46] = 148;
        nArray2[47] = 173;
        nArray2[48] = 198;
        nArray2[49] = 115;
        nArray2[50] = 173;
        nArray2[51] = 198;
        nArray2[52] = 107;
        nArray2[53] = 173;
        nArray2[54] = 189;
        nArray2[55] = 115;
        nArray2[56] = 165;
        nArray2[57] = 189;
        nArray2[58] = 99;
        nArray2[59] = 156;
        nArray2[60] = 189;
        nArray2[61] = 90;
        nArray2[62] = 165;
        nArray2[63] = 173;
        nArray2[64] = 99;
        nArray2[65] = 148;
        nArray2[66] = 173;
        nArray2[67] = 82;
        nArray2[68] = 140;
        nArray2[69] = 173;
        nArray2[70] = 74;
        nArray2[71] = 148;
        nArray2[72] = 165;
        nArray2[73] = 82;
        nArray2[74] = 140;
        nArray2[75] = 156;
        nArray2[76] = 57;
        nArray2[77] = 132;
        nArray2[78] = 148;
        nArray2[79] = 66;
        nArray2[80] = 123;
        nArray2[81] = 148;
        nArray2[82] = 49;
        nArray2[83] = 115;
        nArray2[84] = 140;
        nArray2[85] = 49;
        nArray2[86] = 115;
        nArray2[87] = 140;
        nArray2[88] = 41;
        nArray2[89] = 115;
        nArray2[90] = 132;
        nArray2[91] = 41;
        nArray2[92] = 115;
        nArray2[93] = 132;
        nArray2[94] = 33;
        nArray2[95] = 107;
        nArray2[96] = 123;
        nArray2[97] = 33;
        nArray2[98] = 107;
        nArray2[99] = 115;
        nArray2[100] = 24;
        nArray2[101] = 99;
        nArray2[102] = 115;
        nArray2[103] = 16;
        nArray2[104] = 90;
        nArray2[105] = 107;
        nArray2[106] = 24;
        nArray2[107] = 90;
        nArray2[108] = 99;
        nArray2[109] = 8;
        nArray2[110] = 82;
        nArray2[111] = 99;
        nArray2[113] = 74;
        nArray2[114] = 90;
        nArray2[115] = 24;
        nArray2[116] = 82;
        nArray2[117] = 90;
        nArray2[118] = 8;
        nArray2[119] = 74;
        nArray2[120] = 90;
        nArray2[122] = 74;
        nArray2[123] = 90;
        nArray2[125] = 66;
        nArray2[126] = 82;
        nArray2[128] = 57;
        nArray2[129] = 74;
        nArray2[130] = 8;
        nArray2[131] = 74;
        nArray2[132] = 74;
        nArray2[134] = 49;
        nArray2[135] = 66;
        nArray2[136] = 16;
        nArray2[137] = 74;
        nArray2[138] = 66;
        nArray2[140] = 49;
        nArray2[141] = 57;
        nArray2[143] = 57;
        nArray2[144] = 57;
        nArray2[146] = 41;
        nArray2[147] = 57;
        nArray2[149] = 33;
        nArray2[150] = 49;
        nArray2[152] = 57;
        nArray2[153] = 49;
        nArray2[155] = 41;
        nArray2[156] = 49;
        nArray2[158] = 33;
        nArray2[159] = 41;
        nArray2[161] = 24;
        nArray2[162] = 33;
        nArray2[164] = 41;
        nArray2[165] = 33;
        nArray2[167] = 24;
        nArray2[168] = 33;
        nArray2[170] = 16;
        nArray2[171] = 16;
        nArray2[172] = 16;
        nArray2[173] = 57;
        nArray2[174] = 16;
        nArray2[176] = 33;
        nArray2[177] = 16;
        nArray2[179] = 8;
        nArray2[182] = 49;
        nArray2[185] = 33;
        this.a = nArray2;
    }

    private final void a(byte[] byArray, int n) {
        byte cfr_ignored_0 = (byte)n;
        byArray[5 + n] = 121;
        byArray[323 + n] = 71;
        byArray[324 + n] = 71;
        byArray[325 + n] = 89;
        byArray[326 + n] = 90;
        byArray[327 + n] = 111;
        byArray[642 + n] = 65;
        byArray[643 + n] = 65;
        byArray[644 + n] = 89;
        byArray[645 + n] = 96;
        byArray[646 + n] = 96;
        byArray[647 + n] = 92;
        byArray[961 + n] = 67;
        byArray[962 + n] = 65;
        byArray[963 + n] = 67;
        byArray[964 + n] = 92;
        byArray[965 + n] = 94;
        byArray[966 + n] = 90;
        byArray[967 + n] = 94;
        byArray[968 + n] = 121;
        byArray[1280 + n] = 86;
        byArray[1281 + n] = 65;
        byArray[1282 + n] = 67;
        byArray[1283 + n] = 77;
        byArray[1284 + n] = 96;
        byArray[1285 + n] = 87;
        byArray[1286 + n] = 89;
        byArray[1287 + n] = 83;
        byArray[1288 + n] = 96;
        byArray[1600 + n] = 102;
        byArray[1601 + n] = 75;
        byArray[1602 + n] = 81;
        byArray[1603 + n] = 90;
        byArray[1604 + n] = 84;
        byArray[1605 + n] = 94;
        byArray[1606 + n] = 90;
        byArray[1607 + n] = 84;
        byArray[1608 + n] = 121;
        byArray[1920 + n] = 121;
        byArray[1921 + n] = 87;
        byArray[1922 + n] = 91;
        byArray[1923 + n] = 96;
        byArray[1924 + n] = 84;
        byArray[1925 + n] = 89;
        byArray[1926 + n] = 84;
        byArray[1927 + n] = 84;
        byArray[2241 + n] = 109;
        byArray[2242 + n] = 94;
        byArray[2243 + n] = 94;
        byArray[2244 + n] = 89;
        byArray[2245 + n] = 75;
        byArray[2246 + n] = 77;
        byArray[2247 + n] = 109;
        byArray[2563 + n] = 102;
        byArray[2564 + n] = 94;
        byArray[2565 + n] = 102;
    }
}

