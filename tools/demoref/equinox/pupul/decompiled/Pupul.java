/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.image.MemoryImageSource;
import java.awt.image.PixelGrabber;
import java.net.URL;

public class Pupul
extends Applet
implements Runnable {
    public Thread a2;
    public Image a1;
    public Graphics a0;
    public Image[] a_ = new Image[9];
    public int az;
    public int aw;
    public int av;
    public int au;
    public int at;
    public int as;
    public int ar;
    public int aq;
    public int ap;
    public int ao;
    public int an;
    public int am;
    public double al;
    public double ak;
    public int aj;
    public int ai;
    public int ah;
    public int[][] ag = new int[11][800];
    public int[] af = new int[11];
    public int[] ae;
    public int[] ad;
    public int[] ac;
    public int[] ab;
    public int[] aa;
    public int[] z;
    public int[] v;
    public int[] u;
    public int[] t;
    public MemoryImageSource[] s = new MemoryImageSource[12];
    public Image[] r = new Image[11];
    public int q;
    public int p;
    public float a;
    public float o;
    public float n;
    public int m;
    public float l;
    public float k;
    public int j;
    public int i;
    public int g;
    public double[][] f;
    public int e;

    public final void init() {
        this.k = 12.0f;
        float cfr_ignored_0 = this.k - 1.0f;
        System.gc();
        this.a1 = this.createImage(320, 228);
        this.a0 = this.a1.getGraphics();
        this.a0.setColor(Color.black);
        this.a0.fillRect(0, 0, 320, 228);
        int n = 0;
        int n2 = 0;
        while (n2 < 17) {
            n += "THE EQUINOX TOUCH".charAt(n2);
            ++n2;
        }
        while (n != 1229) {
        }
        this.o = 0.0f;
        this.n = 0.0f;
        int[] cfr_ignored_1 = new int[9840];
        this.l = 5.0f;
        this.ao = 0;
        this.ap = 0;
        this.aq = 0;
        this.ar = 0;
        switch (this.e) {
            case 1: {
                this.aj = 0;
                this.an = 0;
                this.am = 1;
                this.q = 0;
                break;
            }
            case 0: {
                this.am = 0;
                this.q = 1;
                break;
            }
        }
        a a2 = new a();
        a2.setPriority(1);
        a2.a(this);
        a2.start();
        int n3 = 0;
        while (n3 < 8) {
            this.f[0][n3 >> 2] = n3;
            this.ai = this.at;
            ++n3;
        }
        this.a2 = null;
        this.setLayout(null);
        this.resize(320, 228);
        System.gc();
    }

    public final void start() {
        if (this.a2 == null) {
            this.a2 = new Thread(this);
        }
        this.a2.start();
    }

    public final void stop() {
        if (this.a2 != null) {
            this.a2.stop();
        }
        this.a2 = null;
    }

    public final void run() {
        while (this.a2 != null) {
            try {
                Thread.sleep(10L);
            }
            catch (InterruptedException interruptedException) {}
            this.repaint();
            ++this.j;
            if (this.j <= 50) continue;
            System.gc();
            this.j = 0;
        }
    }

    public final synchronized void update(Graphics graphics) {
        if (this.a2 != null) {
            ++this.ar;
            this.a0 = this.a1.getGraphics();
            this.a0.setColor(Color.black);
            this.a = this.a;
            this.o = 0.0f;
            this.n = 1.0f;
            this.l = 12.0f;
            switch (a.c) {
                case 0: {
                    this.a0.fillRect(0, 0, 320, 228);
                    this.a0.setColor(Color.white);
                    this.a0.drawString("Loading images", 80, 114);
                    this.a0.drawString(String.valueOf(String.valueOf(this.m * 10)) + " %", 242, 134);
                    this.o = 0.0f;
                    this.n = this.o * Math.abs(this.l);
                    this.a0.setColor(new Color(this.m * 25, this.m * 25, 255));
                    this.a0.fillRect(80, 124, (int)((double)(160.0f * (float)this.m) / 10.0), 10);
                    this.a0.setColor(Color.white);
                    this.a0.drawLine(80, 124, 240, 124);
                    this.a0.drawLine(80, 134, 240, 134);
                    this.a0.drawLine(80, 124, 80, 134);
                    this.a0.drawLine(240, 124, 240, 134);
                    break;
                }
                case 1: {
                    this.a = this.a;
                    this.o = 6.969697E7f;
                    this.n = 0.0f;
                    this.l = 5.0f;
                    break;
                }
            }
            --this.i;
            if (a.d == 1) {
                ++this.g;
                this.b();
            }
            this.a0.setColor(Color.white);
            this.a0.drawString("THE EQUINOX TOUCH", 100, 57);
            graphics.drawImage(this.a1, 0, 0, 320, 228, this);
        }
    }

    private final void h(URL uRL, String string, int n) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        this.n = this.o * 2.0f;
        int n2 = 6;
        int n3 = 6;
        this.at = image.getWidth(null);
        this.as = image.getHeight(null);
        int[] nArray = new int[this.at * this.as];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, this.at, this.as, nArray, 0, this.at);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        this.i = 1;
        this.a_[n] = this.createImage(new MemoryImageSource(this.at, this.as, nArray, 0, this.at));
    }

    private final void g(URL uRL, String string) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n = 6;
        int n2 = 6;
        this.ai = image.getWidth(null);
        this.ah = image.getHeight(null);
        this.ae = new int[this.ai * this.ah];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, this.ai, this.ah, this.ae, 0, this.ai);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
    }

    private final void f(URL uRL, String string) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        this.av = image.getWidth(null);
        this.au = image.getHeight(null);
        this.ad = new int[this.av * this.au];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, this.av, this.au, this.ad, 0, this.av);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        ++this.g;
        this.s[11] = new MemoryImageSource(this.av, this.au, this.ad, 0, this.av);
        this.s[11].setAnimated(true);
        this.s[11].setFullBufferUpdates(true);
        this.s[11].newPixels();
        this.a_[0] = this.createImage(this.s[11]);
        this.ac = new int[this.av * this.au];
        System.arraycopy(this.ad, 0, this.ac, 0, this.av * this.au);
    }

    private final void e(URL uRL, String string) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n = 6;
        int n2 = 6;
        this.ai = image.getWidth(null);
        this.ah = image.getHeight(null);
        this.ab = new int[this.av * 86];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, this.av, this.au, this.ab, 0, this.av);
        try {
            pixelGrabber.grabPixels();
            return;
        }
        catch (InterruptedException interruptedException) {
            return;
        }
    }

    public final void d() {
        this.m = 0;
        this.showStatus("loading 1 of 10");
        this.f(this.getDocumentBase(), "fondpupul2.gif");
        this.m = 1;
        this.showStatus("loading 2 of 10");
        this.h(this.getDocumentBase(), "dragon8.gif", 1);
        this.m = 2;
        this.showStatus("loading 3 of 10");
        this.h(this.getDocumentBase(), "dragon7.gif", 2);
        this.m = 3;
        this.showStatus("loading 4 of 10");
        this.h(this.getDocumentBase(), "dragon6.gif", 3);
        this.m = 4;
        this.showStatus("loading 5 of 10");
        this.h(this.getDocumentBase(), "dragon5.gif", 4);
        this.m = 5;
        this.showStatus("loading 6 of 10");
        this.h(this.getDocumentBase(), "dragon4.gif", 5);
        this.m = 6;
        this.showStatus("loading 7 of 10");
        this.h(this.getDocumentBase(), "dragon1.gif", 6);
        this.m = 7;
        this.showStatus("loading 8 of 10");
        this.h(this.getDocumentBase(), "dragon2.gif", 7);
        this.m = 8;
        this.showStatus("loading 9 of 10");
        this.h(this.getDocumentBase(), "dragon3.gif", 8);
        this.m = 9;
        this.az = this.at;
        this.aw = this.as;
        this.showStatus("loading 10 of 10");
        this.g(this.getDocumentBase(), "fonte.gif");
        this.m = 10;
        this.showStatus("loading 10 of 10");
        this.e(this.getDocumentBase(), "fondpupul22.gif");
        this.showStatus("ok");
    }

    public final void c() {
        int n;
        int n2;
        int n3;
        int n4;
        int n5;
        this.showStatus("Pupul Atari intro (Equinox 1989)");
        this.i = 28;
        this.ai = this.at;
        this.ah = this.as;
        int n6 = 0;
        while (n6 < 8) {
            ++n6;
        }
        int n7 = 0;
        while (n7 < 11) {
            n5 = "       HI KIDS! THIS IS THE GREAT PUPUL INTRO COMING BACK FROM AGES!!!  THIS JAVA VERSION IS NEARLY THE SAME AS OUR ORIGINAL ATARI INTRO CODED TEN YEARS AGO...  TIME TO WRAP!      ".charAt(this.q++);
            this.af[n7] = n7 * 32;
            if (n5 < 65 && n5 != 32 && n5 != 33 && n5 != 46) {
                n4 = 0;
                while (n4 < 32) {
                    n3 = 0;
                    while (n3 < 25) {
                        this.ag[n7][n4 + n3 * 32] = -16777216;
                        this.i = n4;
                        ++n3;
                    }
                    ++n4;
                }
            } else {
                n5 = (n5 -= 65) == -33 ? 16832 : (n5 == -32 ? 16864 : (n5 == -19 ? 16896 : (n5 >= 20 ? (n5 - 20) * 32 + 16640 : (n5 >= 10 ? (n5 - 10) * 32 + 8320 : (n5 *= 32)))));
                n4 = 0;
                while (n4 < 32) {
                    n3 = 0;
                    while (n3 < 25) {
                        this.ag[n7][n4 + n3 * 32] = this.ae[n4 + n5 + n3 * 320];
                        ++n3;
                    }
                    ++n4;
                }
            }
            ++n7;
        }
        n5 = 0;
        while (n5 < 11) {
            this.i = n5;
            float cfr_ignored_0 = n5 >> 3;
            this.s[n5] = new MemoryImageSource(32, 25, this.ag[n5], 0, 32);
            this.s[n5].setAnimated(true);
            this.s[n5].setFullBufferUpdates(true);
            this.s[n5].newPixels();
            this.r[n5] = this.createImage(this.s[n5]);
            ++n5;
        }
        this.a0 = this.a1.getGraphics();
        this.a0.drawImage(this.a_[0], 0, 0, this.av, this.au, null);
        this.p = 30;
        this.z = new int[21];
        this.v = new int[21];
        n4 = 0;
        while (n4 < 21) {
            this.v[n4] = 5 * n4 + 4;
            this.z[n4] = 296 / this.v[n4] + 110;
            ++n4;
        }
        n3 = 0;
        while (n3 < 10) {
            n2 = this.z[n3 * 2 + 1];
            while (n2 < this.z[n3 * 2]) {
                if (n2 < 196) {
                    n = 0;
                    while (n < 320) {
                        int n8 = n2 * 320 + n;
                        this.ad[n8] = this.ad[n8] ^ 0xFFFFFF;
                        ++n;
                    }
                }
                ++n2;
            }
            ++n3;
        }
        this.aa = new int[320];
        n2 = 0;
        while (n2 < 320) {
            this.aa[n2] = -4482748;
            ++n2;
        }
        this.u = new int[2000];
        this.t = new int[2000];
        this.ak = 0.0;
        this.al = 0.0;
        n = 0;
        while (n < 125) {
            this.u[n] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.u[n + 940] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n + 950] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.al += 0.05;
            this.ak += 0.055;
            ++n;
        }
        int n9 = 125;
        while (n9 < 220) {
            this.u[n9] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n9] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.u[n9 + 940] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n9 + 950] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.al += 0.03;
            this.ak += 0.035;
            ++n9;
        }
        this.l = 5.0f;
        int n10 = 220;
        while (n10 < 480) {
            this.u[n10] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n10] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.u[n10 + 940] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n10 + 950] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.al += 0.06;
            this.ak += 0.03;
            ++n10;
        }
        int n11 = 480;
        while (n11 < 630) {
            this.u[n11] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n11] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            float cfr_ignored_1 = (float)(Math.cos(n11 / 10) * 50.0);
            this.u[n11 + 940] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n11 + 950] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.al += 0.045;
            this.ak += 0.035;
            ++n11;
        }
        int n12 = 630;
        while (n12 < 940) {
            this.i = n12;
            this.u[n12] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n12] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.u[n12 + 940] = 160 - this.az / 2 + (int)((128.0 - (double)(this.az / 2) - 5.0) * Math.cos(this.al));
            this.t[n12 + 950] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.al += 0.02;
            this.ak += 0.045;
            ++n12;
        }
        int n13 = 940;
        while (n13 < 951) {
            this.t[n13] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.f[0][0] = n13 * n13;
            this.t[n13 + 950] = 98 - this.aw / 2 + 30 + (int)((65.33333333333333 - (double)(this.aw / 2) - 10.0) * Math.sin(this.ak));
            this.ak += 0.045;
            ++n13;
        }
    }

    private final void b() {
        int n;
        int n2;
        int n3;
        int n4;
        int n5 = 0;
        while (n5 < 10) {
            n4 = this.z[n5 * 2 + 1] * 320;
            n3 = this.z[n5 * 2] < 196 ? this.z[n5 * 2] - this.z[n5 * 2 + 1] : 196 - this.z[n5 * 2 + 1];
            if (n3 > 0) {
                this.i = 128;
                System.arraycopy(this.ac, n4, this.ad, n4, n3 * 320);
                this.n = this.o * this.l * this.n * this.o;
            }
            ++n5;
        }
        n4 = 0;
        while (n4 < 10) {
            this.z[n4 * 2 + 1] = 296 / this.v[n4 * 2 + 1] + 110;
            this.z[n4 * 2] = 296 / this.v[n4 * 2] + 110;
            int n6 = n4 * 2;
            this.v[n6] = this.v[n6] - 1;
            int n7 = n4 * 2 + 1;
            this.v[n7] = this.v[n7] - 1;
            if (this.v[n4 * 2 + 1] <= 0) {
                this.v[n4 * 2] = 95;
                this.v[n4 * 2 + 1] = 100;
            }
            if (this.v[n4 * 2] <= 0) {
                this.v[n4 * 2] = 1;
            }
            ++n4;
        }
        n3 = 0;
        while (n3 < 10) {
            n2 = this.z[n3 * 2 + 1] * 320;
            n = this.z[n3 * 2] < 196 ? this.z[n3 * 2] - this.z[n3 * 2 + 1] : 196 - this.z[n3 * 2 + 1];
            if (n > 0) {
                System.arraycopy(this.ab, n2 - 35200, this.ad, n2, n * 320);
            }
            ++n3;
        }
        System.arraycopy(this.aa, 0, this.ad, 62400, 320);
        --this.p;
        if (this.p == 0) {
            this.p = 30;
        }
        this.k = 1.6500001f;
        this.s[11].newPixels(0, 111, 320, 85);
        this.a0.drawImage(this.a_[0], 0, 73, 320, 200, 0, 73, 320, 200, null);
        ++this.aq;
        ++this.ap;
        if (this.ap > 939) {
            this.ap = 0;
        }
        ++this.ao;
        if (this.ao > 950) {
            this.ao = 0;
        }
        this.a = this.a;
        this.o /= this.o;
        this.n = 0.0f;
        this.l = 5.0f;
        switch (this.an) {
            case 0: {
                this.aj = 1;
                if (this.ar != 250) break;
                this.an = 2;
                this.ar = 0;
                this.am = 1;
                break;
            }
            case 1: {
                break;
            }
            case 2: {
                if (this.ar % 2 == 0) {
                    this.aj += this.am;
                }
                if (this.aj == 8) {
                    this.am = -1;
                }
                if (this.aj == 5) {
                    this.am = 1;
                }
                if (this.ar != 400) break;
                this.an = 3;
                this.ar = 0;
                break;
            }
            case 3: {
                if (this.ar % 2 == 0) {
                    --this.aj;
                }
                if (this.aj > 1) break;
                this.an = 0;
                ++this.g;
                this.ar = 0;
            }
        }
        n2 = 0;
        while (n2 < 7) {
            int n8;
            int n9;
            this.o = 123.0f;
            switch ((int)this.o) {
                case 0: {
                    n9 = this.u[this.ap];
                    n8 = this.t[this.ao];
                    this.a0.drawImage(this.a_[this.aj], n9, n8, this.az, this.aw, null);
                    break;
                }
                case 1: {
                    this.o = 0.14285715f;
                    n9 = this.u[this.ap + n2 * 12];
                    n8 = this.t[this.ap + n2 * 16];
                    this.a0.drawImage(this.a_[this.aj], n9, n8, this.az, this.aw, null);
                    break;
                }
            }
            n9 = this.u[this.ap + n2 * 16];
            n8 = this.t[this.ao + n2 * 16];
            this.a0.drawImage(this.a_[this.aj], n9, n8, this.az, this.aw, null);
            ++n2;
        }
        n = 0;
        while (n < 11) {
            this.a0.drawImage(this.r[n], this.af[n], 200, 32, 25, null);
            int n10 = n;
            this.af[n10] = this.af[n10] - 6;
            if (this.af[n] <= -32) {
                int n11;
                int n12 = n;
                this.af[n12] = this.af[n12] + 352;
                int n13 = "       HI KIDS! THIS IS THE GREAT PUPUL INTRO COMING BACK FROM AGES!!!  THIS JAVA VERSION IS NEARLY THE SAME AS OUR ORIGINAL ATARI INTRO CODED TEN YEARS AGO...  TIME TO WRAP!      ".charAt(this.q++);
                if (this.q >= "       HI KIDS! THIS IS THE GREAT PUPUL INTRO COMING BACK FROM AGES!!!  THIS JAVA VERSION IS NEARLY THE SAME AS OUR ORIGINAL ATARI INTRO CODED TEN YEARS AGO...  TIME TO WRAP!      ".length()) {
                    this.q = 0;
                }
                if (n13 < 65 && n13 != 32 && n13 != 33 && n13 != 46) {
                    n11 = 0;
                    while (n11 < 32) {
                        int n14 = 0;
                        while (n14 < 25) {
                            this.ag[n][n11 + n14 * 32] = -16777216;
                            ++n14;
                        }
                        ++n11;
                    }
                } else {
                    n13 = (n13 -= 65) == -33 ? 16832 : (n13 == -32 ? 16864 : (n13 == -19 ? 16896 : (n13 >= 20 ? (n13 - 20) * 32 + 16640 : (n13 >= 10 ? (n13 - 10) * 32 + 8320 : (n13 *= 32)))));
                    this.i = 0;
                    n11 = 0;
                    while (n11 < 25) {
                        System.arraycopy(this.ae, n13 + n11 * 320, this.ag[n], n11 * 32, 32);
                        ++n11;
                    }
                }
                this.s[n].newPixels();
            }
            ++n;
        }
    }

    public Pupul() {
        float[][] cfr_ignored_0 = new float[2][4];
        int[] cfr_ignored_1 = new int[20];
        this.f = new double[4][10];
        this.e = 1;
    }
}

