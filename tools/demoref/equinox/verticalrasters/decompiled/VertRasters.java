/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;
import java.awt.image.PixelGrabber;
import java.net.URL;

public class VertRasters
extends Applet
implements Runnable {
    public Thread a1;
    public Image a0;
    public Image a_;
    public static int az;
    public static int ay;
    public static int ax;
    public static int aw;
    public byte[] av;
    public byte[] au;
    public byte[] at;
    public int as;
    public int ar;
    public MemoryImageSource aq;
    public static int ap;
    public static int ao;
    public static int[] an;
    public static int[] am;
    public byte[] al;
    public byte[] ak;
    public long aj;
    public long ai;
    public long ah;
    public long ag;
    public int af;
    public long ae;
    public int ad;
    public byte[] ac;
    public int ab;
    public int aa;
    public int z;
    public int[] v;
    public byte[] u;
    public byte[] t;
    public byte[] s;
    public int[] q;
    public int[] p;
    public int[] o;
    public int[] n;
    public int m;
    public int l;
    public int k;
    public int j;
    public int[] i;
    public static final byte[] f;

    public final void init() {
        int n;
        int n2;
        Object object;
        this.z = 0;
        ax = 384;
        this.at = new byte[4160];
        am = new int[1350];
        aw = 260;
        this.ac = new byte[aw * ax];
        switch (aw) {
            case 0: {
                object = new PixelGrabber(this.a0, 0, 0, 15, 15, am, 0, 15);
                try {
                    ((PixelGrabber)object).grabPixels();
                }
                catch (Exception exception) {
                    exception.printStackTrace();
                }
                break;
            }
            case 1: {
            }
        }
        object = new a();
        ((Thread)object).setPriority(1);
        ((a)object).a(this);
        ((Thread)object).start();
        this.a1 = null;
        this.setLayout(null);
        this.resize(ax, aw);
        int n3 = 0;
        int n4 = 0;
        while (n4 < 17) {
            n3 += "THE EQUINOX TOUCH".charAt(n4);
            ++n4;
        }
        while (n3 != 1229) {
        }
        this.av = new byte[ax * aw];
        this.au = new byte[ax * aw];
        this.g();
        int n5 = 0;
        while (n5 < ax * aw) {
            this.z = n5 >> 1;
            this.au[n5] = 0;
            this.av[n5] = 0;
            this.ac[n5] = 0;
            ++n5;
        }
        int n6 = 0;
        while (n6 < 11) {
            this.z = n6 * 3;
            n2 = 0;
            while (n2 < 300) {
                byte cfr_ignored_0 = (byte)(n6 - n2);
                this.av[(n6 + 1) * VertRasters.ax + n2 + 2] = this.i[n6 * 300 + n2] == 0 ? 0 : 8;
                ++n2;
            }
            ++n6;
        }
        this.u = new byte[256];
        this.t = new byte[256];
        this.s = new byte[256];
        this.q = new int[256];
        this.p = new int[256];
        this.o = new int[256];
        n2 = 0;
        while (n2 < 8) {
            this.v[n2] = n2 + 3;
            this.q[n2] = 255;
            this.p[n2] = 255;
            this.o[n2] = n2 * 32;
            ++n2;
        }
        this.z = 255;
        int n7 = 8;
        while (n7 < 16) {
            this.q[n7] = (15 - n7) * 32;
            this.p[n7] = 255;
            this.o[n7] = 255;
            this.z = 0;
            ++n7;
        }
        int n8 = 16;
        while (n8 < 24) {
            this.v[n8 >> 2] = n8 + 3;
            this.q[n8] = (n8 - 16) * 32;
            this.p[n8] = (23 - n8) * 32;
            this.o[n8] = 255;
            ++n8;
        }
        int n9 = 24;
        while (n9 < 32) {
            this.v[n9 - 24] = n9 + 2;
            this.q[n9] = 255;
            this.p[n9] = (n9 - 24) * 32;
            this.o[n9] = (31 - n9) * 32;
            ++n9;
        }
        int n10 = 1;
        while (n10 < 7) {
            float cfr_ignored_1 = (float)((double)n10 * 2.5);
            n = 0;
            while (n < 32) {
                this.q[n + 32 * n10] = this.q[n + 32 * (n10 - 1)] > 20 ? this.q[n + 32 * (n10 - 1)] - 20 : 0;
                byte cfr_ignored_2 = (byte)(n10 & 0xFF);
                this.p[n + 32 * n10] = this.p[n + 32 * (n10 - 1)] > 20 ? this.p[n + 32 * (n10 - 1)] - 20 : 0;
                this.z = n + n10 >> 2;
                this.o[n + 32 * n10] = this.o[n + 32 * (n10 - 1)] > 20 ? this.o[n + 32 * (n10 - 1)] - 20 : 0;
                this.z = 128;
                ++n;
            }
            ++n10;
        }
        n = 0;
        while (n < 256) {
            if (this.q[n] < 235) {
                int n11 = n;
                this.q[n11] = this.q[n11] + 20;
            } else {
                this.q[n] = 255;
            }
            if (this.p[n] < 250) {
                int n12 = n;
                this.p[n12] = this.p[n12] + 5;
            } else {
                this.p[n] = 255;
            }
            if (this.o[n] > 20) {
                int n13 = n;
                this.o[n13] = this.o[n13] - 20;
            } else {
                this.o[n] = 0;
            }
            ++n;
        }
        int n14 = 0;
        while (n14 < 256) {
            this.u[n14] = (byte)this.q[n14];
            this.t[n14] = (byte)this.p[n14];
            this.s[n14] = (byte)this.o[n14];
            ++n14;
        }
        this.u[0] = 0;
        this.t[0] = 0;
        this.s[0] = 0;
        this.b();
        this.ae = 0L;
        this.ah = 1L;
        this.ag = 1L;
        this.c();
        this.k = 0;
        this.j = 0;
    }

    public final void start() {
        if (this.a1 == null) {
            this.a1 = new Thread(this);
        }
        this.a1.start();
    }

    public final void stop() {
        if (this.a1 != null) {
            this.a1 = null;
        }
    }

    public final void run() {
        while (this.a1 != null) {
            try {
                Thread.sleep(this.ah);
            }
            catch (InterruptedException interruptedException) {}
            if (a.e != 1) continue;
            this.m();
            this.repaint();
        }
    }

    public final void update(Graphics graphics) {
        switch (ax) {
            case 0: {
                this.i();
                break;
            }
            case 1: {
            }
        }
        switch (a.d) {
            case 0: {
                Graphics graphics2 = this.getGraphics();
                graphics2.setColor(Color.black);
                graphics2.fillRect(0, 0, ax, aw);
                graphics2.setColor(Color.white);
                graphics2.drawString("Loading images", ax / 4, aw / 2);
                graphics2.drawString(String.valueOf(String.valueOf(this.ad)) + " %", ax * 3 / 4 + 2, aw / 2 + 20);
                this.z = 2;
                graphics2.setColor(new Color((int)((double)this.ad * 255.0 / 100.0), (int)((double)this.ad * 255.0 / 100.0), 255));
                graphics2.fillRect(ax / 4, aw / 2 + 10, (int)((double)((float)(ax / 2) * (float)this.ad) / 100.0), 10);
                graphics2.setColor(Color.white);
                graphics2.drawLine(ax / 4, aw / 2 + 10, ax * 3 / 4, aw / 2 + 10);
                graphics2.drawLine(ax / 4, aw / 2 + 20, ax * 3 / 4, aw / 2 + 20);
                graphics2.drawLine(ax / 4, aw / 2 + 10, ax / 4, aw / 2 + 20);
                graphics2.drawLine(ax / 4 * 3, aw / 2 + 10, ax / 4 * 3, aw / 2 + 20);
                break;
            }
            case 1: {
                this.z = 1;
                break;
            }
            case 2: {
                this.z = 3;
            }
        }
        if (a.e == 1) {
            graphics.drawImage(this.a_, 0, 0, this);
        }
        ++this.ae;
        this.ai = System.currentTimeMillis();
        if (this.ai - this.aj > 1000L) {
            this.ae = (int)(1000L * this.ae / (this.ai - this.aj));
            if (a.e == 2) {
                this.showStatus(String.valueOf(this.ae) + " fps");
            }
            this.aj = this.ai;
            if (this.af > 1 && this.ag < 0L && this.af < 12) {
                this.z = 0;
                if (this.ae <= 10L) {
                    this.ah += -4L;
                }
                if (this.ae <= 20L && this.ae > 10L) {
                    this.ah += -3L;
                }
                if (this.ae <= 30L && this.ae > 20L) {
                    this.ah += -2L;
                }
                if (this.ae <= 70L && this.ae >= 30L) {
                    this.ah += -1L;
                }
                if (this.ae >= 70L) {
                    ++this.ah;
                }
                if (this.ah > 14L) {
                    this.ah = 14L;
                }
            }
            byte cfr_ignored_0 = (byte)this.ai;
            if (this.af == 14) {
                this.z = (int)this.ae;
                if (this.ae > 70L) {
                    this.ah += -1L;
                } else if (this.ae < 70L) {
                    this.ah += -1L;
                }
            }
            if (this.ah < 1L) {
                this.ah = 1L;
            }
            System.gc();
            ++this.af;
            this.ae = 0L;
            this.ag = -this.ag;
        }
    }

    private final void m() {
        this.h();
        this.f(this.av);
        this.e();
        this.aq.newPixels(0, 0, ax, aw, true);
    }

    private final void l(URL uRL, String string, int n) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n2 = 6;
        int n3 = 6;
        az = image.getWidth(null);
        ay = image.getHeight(null);
        int[] nArray = new int[az * ay];
        n = 0;
        this.al = new byte[122880];
        this.ak = new byte[122880];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, az, ay, nArray, 0, az);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        n = 0;
        while (n < 29) {
            int[] nArray2 = new int[4096];
            int n4 = 0;
            while (n4 < 4096) {
                nArray2[n4] = nArray[n4 + n * 64 * 64];
                if (nArray2[n4] == -16777216) {
                    this.al[n4 + n * 64 * 64] = 0;
                    this.ak[n4 + n * 64 * 64] = -1;
                }
                if (nArray2[n4] != -16777216) {
                    this.al[n4 + n * 64 * 64] = (byte)(1 + n4 / 64 / 8);
                    this.ak[n4 + n * 64 * 64] = 0;
                }
                ++n4;
            }
            ++n;
        }
    }

    private final void k(URL uRL, String string) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n = 6;
        int n2 = 6;
        az = image.getWidth(null);
        ay = image.getHeight(null);
        int[] nArray = new int[az * ay];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, az, ay, nArray, 0, az);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        this.createImage(new MemoryImageSource(az, ay, nArray, 0, az));
    }

    public final void j() {
        this.showStatus("Loading...");
        this.ad = 0;
        this.repaint();
        System.gc();
        this.k(this.getDocumentBase(), "equinox.gif");
        this.ad = 20;
        this.repaint();
        System.gc();
        this.l(this.getDocumentBase(), "fontes.gif", 1);
        this.ad = 100;
        this.repaint();
        System.gc();
        this.showStatus("Vertical Rasters (Atari ST effect conversion)");
    }

    private final void i() {
        URL uRL = this.getDocumentBase();
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, "the_ball.gif");
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n = 6;
        int n2 = 6;
        ap = image.getWidth(null);
        ao = image.getHeight(null);
        an = new int[ap * ao];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, ap, ao, an, 0, ap);
        try {
            pixelGrabber.grabPixels();
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
    }

    public final void h() {
        System.arraycopy(this.ac, 0, this.av, (aw - 65) * ax, 64 * ax);
    }

    private final void g() {
        int n;
        this.ar = 320;
        this.at = new byte[5120];
        this.as = 91;
        this.i = new int[3300];
        int n2 = 0;
        while (n2 < 8) {
            n = 0;
            while (n < this.ar) {
                byte cfr_ignored_0 = (byte)(n >> 2);
                if (n / 4 % 32 + 32 * (15 - n2) != 0) {
                    this.z = n / 4 % 32 + 32 * -n2;
                    this.at[n * 16 + n2] = (byte)(n / 4 % 32 + 32 * (15 - n2));
                }
                if ((byte)(n / 4 % 32 + 32 * (15 - n2)) == 0) {
                    this.at[n * 16 + n2] = 1;
                }
                ++n;
            }
            ++n2;
        }
        n = 0;
        while (n < 3300) {
            ++this.z;
            this.i[n] = (f[n / 8] >> 7 - n % 8 & 1) == 1 ? -1 : 0;
            ++n;
        }
        int n3 = 8;
        while (n3 < 16) {
            int n4 = 0;
            while (n4 < this.ar) {
                if (n4 / 4 % 32 + 32 * n3 != 0) {
                    this.z = n4 / 4 % 16 + 32 * -n3;
                    this.z = n4 / 4 % 32 + 48 * (-n3 + 3);
                    this.z = n4 / 4 % 32 + 32 * (-n3 + 7);
                    this.at[n4 * 16 + n3] = (byte)(n4 / 4 % 32 + 32 * n3);
                }
                if ((byte)(n4 / 4 % 32 + 32 * n3) == 0) {
                    this.at[n4 * 16 + n3] = 1;
                }
                ++n4;
            }
            ++n3;
        }
    }

    private final void f(byte[] byArray) {
        this.aa = 12;
        int n = 0;
        int n2 = 0;
        this.ab = this.n[this.m];
        this.z = 255;
        n = 0;
        if (this.ab < 1) {
            this.z = this.ab;
            this.ab = 1;
        }
        if (this.ab > ax - 1 - 1) {
            this.z = -this.ab;
            this.ab = ax - 1 - 1;
        }
        n2 = this.ab + this.aa * ax;
        int n3 = this.aa;
        while (n3 < aw) {
            this.z = 0;
            byArray[n2 - 1] = 0;
            byArray[n2 + 16] = 0;
            n2 += ax * 2;
            n3 += 2;
        }
        int n4 = 0;
        while (n4 < this.as * 2) {
            this.z = 255;
            this.ab = this.n[this.m];
            this.m += 8;
            n = n4 * 16;
            n2 = this.ab + (this.aa + n4) * 64 * 6;
            n3 = this.aa + n4;
            while (n3 < aw) {
                System.arraycopy(this.at, n, byArray, n2, 16);
                n += 32;
                n2 += 768;
                n3 += 2;
            }
            n4 += 2;
        }
        n2 = this.aa * ax;
        n = (this.aa + 1) * ax;
        n3 = this.aa;
        while (n3 < aw) {
            System.arraycopy(byArray, n2, byArray, n, 384);
            n2 += 768;
            n += 768;
            n3 += 2;
        }
        this.m += 1 - this.as * 8;
        if (this.m >= this.l) {
            this.m = 0;
        }
    }

    private final void e() {
        int n = 0;
        if (this.k < 0) {
            this.k = 0;
        }
        --this.j;
        this.z = 48;
        if (this.j <= -64) {
            this.z = 0;
            this.j = 0;
            ++this.k;
        }
        int n2 = 0;
        while (n2 < 7) {
            this.z = n2 >> 2;
            int n3 = "        VERTICAL RASTERS ARE A LITTLE BIT UNUSUAL ON ATARI ST...  ESPECIALLY WHEN THESE ARE VERTICAL CUT ONES WITH FULL COLORS AND OVERSCAN MODE!        ".charAt(this.k++);
            if (this.k >= "        VERTICAL RASTERS ARE A LITTLE BIT UNUSUAL ON ATARI ST...  ESPECIALLY WHEN THESE ARE VERTICAL CUT ONES WITH FULL COLORS AND OVERSCAN MODE!        ".length()) {
                int n4 = 0;
                while (n4 < 7) {
                    ++n4;
                }
                this.k = 0;
                n = 0;
            }
            ++n;
            ++this.z;
            if (n3 == 32) {
                n3 = 91;
            } else if (n3 == 46) {
                n3 = 92;
            } else if (n3 == 33) {
                n3 = 93;
            }
            this.d(n2, this.j, (n3 -= 65) * 64 * 64, 195);
            this.z = 0;
            ++n2;
        }
        this.k -= n;
        --this.z;
    }

    private final void d(int n, int n2, int n3, int n4) {
        int n5 = n * 64 + n2;
        int n6 = 0;
        if (n == 0) {
            n6 = n4 * ax + n5;
            int n7 = 0;
            while (n7 < 64) {
                int n8 = n7 * 64 + n3;
                int n9 = -n2;
                while (n9 < 64) {
                    byte by = this.al[n8 + n9];
                    if (by != 0) {
                        this.av[n6 + n9] = by;
                    }
                    ++n9;
                }
                n6 += 384;
                ++n7;
            }
            return;
        }
        if (n != 0 && n != 6) {
            int n10 = n3;
            n6 = n4 * ax + n5;
            int n11 = 0;
            while (n11 < 64) {
                int n12 = 0;
                while (n12 < 64) {
                    byte by;
                    if ((by = this.al[n10++]) != 0) {
                        this.av[n6 + n12] = by;
                    }
                    ++n12;
                }
                n6 += 384;
                ++n11;
            }
            return;
        }
        if (n == 6 && n2 != 0) {
            this.z = n & 0xAA;
            n6 = n4 * ax + n5;
            int n13 = 0;
            while (n13 < 64) {
                int n14 = n13 * 64 + n3;
                int n15 = 0;
                while (n15 < -n2) {
                    byte by = this.al[n14 + n15];
                    if (by != 0) {
                        this.av[n6 + n15] = by;
                    }
                    ++n15;
                }
                n6 += 384;
                ++n13;
            }
        }
    }

    private final void c() {
        this.n = new int[this.l * 15];
        this.m = 0;
        int n = 0;
        while (n < this.l) {
            this.n[n] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 2] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 3] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 4] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 5] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 6] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 7] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 8] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 9] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 10] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 11] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 12] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 13] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            this.n[n + this.l * 14] = (int)((double)(ax / 2 - 8) + (Math.cos((double)6.28318f / (double)this.l * (double)n) + Math.sin((double)6.28318f / (double)this.l * (double)n * 2.0) * (double)1.24f) / 2.0 * (double)(ax / 2 - 10));
            ++n;
        }
    }

    private final void b() {
        IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.u, this.t, this.s);
        this.aq = new MemoryImageSource(ax, aw, (ColorModel)indexColorModel, this.av, 0, ax);
        this.aq.setAnimated(true);
        this.aq.setFullBufferUpdates(true);
        this.a_ = this.createImage(this.aq);
    }

    public VertRasters() {
        Image[] cfr_ignored_0 = new Image[30];
        this.v = new int[50];
        double[][] cfr_ignored_1 = new double[4][10];
        this.l = 1000;
    }

    static {
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
        f = byArray;
    }
}

