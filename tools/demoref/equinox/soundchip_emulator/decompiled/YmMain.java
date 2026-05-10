/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Color;
import java.awt.Event;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.PixelGrabber;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

public class YmMain
extends Applet
implements Runnable {
    public Thread bd;
    public Image bc;
    public Graphics ba;
    public static int a9;
    public static int a8;
    public d a7;
    public a a6;
    public static int[] a5;
    public int a4;
    public long a3;
    public long a2;
    public long a1;
    public int a0;
    public boolean a_ = true;
    public long az;
    public int ay;
    public long aw;
    public Image av;
    public Image au;
    public Image at;
    public Image as;
    public Image ar;
    public Image ap;
    public int ao;
    public int an;
    public int am;
    public int al;
    public URL ak;
    public String aj;
    public String[] ai;
    public Font ah;
    public Font ag;
    public int af;
    public URL ad;
    public String ac;
    public e ab;
    public f aa;
    public int z;

    public final void init() {
        this.a7 = new d();
        a9 = 320;
        this.a4 = 400;
        a5 = new int[1350];
        a8 = 196;
        this.bc = this.createImage(a9, a8);
        this.ba = this.bc.getGraphics();
        this.ba.setColor(Color.black);
        switch (a8) {
            case 0: {
                PixelGrabber pixelGrabber = new PixelGrabber(this.bc, 0, 0, 15, 15, a5, 0, 15);
                try {
                    pixelGrabber.grabPixels();
                }
                catch (Exception exception) {
                    exception.printStackTrace();
                }
                break;
            }
            case 1: {
            }
        }
        this.ab = new e();
        this.aa = new f();
        this.ba.clearRect(0, 0, a9, a8);
        this.ba.setColor(Color.white);
        this.ba.drawString("Loading and initializing", a9 / 2 - a9 / 4, a8 / 2);
        this.a6 = new a();
        this.av = this.getImage(this.getCodeBase(), "LogoYM.jpg");
        this.au = this.getImage(this.getCodeBase(), "ball14x14.gif");
        this.at = this.getImage(this.getCodeBase(), "raster16.gif");
        this.ab.aj(this.ai);
        this.av = this.getImage(this.getCodeBase(), "eqxlogo.gif");
        this.ad = this.getCodeBase();
        this.ac = this.ad.getHost();
        YmMain ymMain = this;
        this.getCodeBase().toString();
        this.j();
        this.bd = null;
        this.setLayout(null);
        this.resize(a9, a8);
        int n = 0;
        int n2 = 0;
        while (n2 < 17) {
            n += "THE EQUINOX TOUCH".charAt(n2);
            ++n2;
        }
        while (n != 1229) {
        }
        this.a7.f(this.a4, a9, a8);
        this.a3 = System.currentTimeMillis();
        this.az = 0L;
        if (!this.aa.ak().equals(this.ac)) {
            this.a6 = null;
        }
        this.aw = 500L;
        this.a1 = 13L;
        this.ba = this.bc.getGraphics();
        this.ba.setFont(new Font("Arial", 1, 10));
        this.ba.drawString("Yamaha 2149 soundchip emulator", 85, 12);
        this.as = this.getImage(this.getCodeBase(), "fonte_e.gif");
        this.ar = this.getImage(this.getCodeBase(), "fonte_q.gif");
        this.ap = this.getImage(this.getCodeBase(), "fonte_x.gif");
    }

    public final void start() {
        if (this.bd == null) {
            this.bd = new Thread(this);
            this.bd.start();
            this.a6.h();
        }
    }

    public final void stop() {
        if (this.bd != null) {
            this.a6.g();
            this.bd = null;
        }
    }

    public final void destroy() {
        this.a6.g();
        this.a6 = null;
        this.bd = null;
    }

    public final void run() {
        while (Thread.currentThread() == this.bd) {
            this.repaint();
            try {
                Thread.sleep(this.aw);
            }
            catch (InterruptedException interruptedException) {}
        }
    }

    private final void k() {
        ++this.az;
        this.a2 = System.currentTimeMillis();
        if (this.a2 - this.a3 > 1000L) {
            this.a3 = this.a2;
            if (this.a0 > 0) {
                if (!this.a6.e()) {
                    if (this.az < 20L && this.a1 > 3L) {
                        this.a1 -= 2L;
                    } else if (this.az < 45L && this.a1 > 2L) {
                        --this.a1;
                    } else if (this.az > 90L) {
                        ++this.a1;
                    }
                }
            }
            if (!this.a6.e()) {
                ++this.a0;
            }
            this.aw = this.a1;
            this.az = 0L;
        }
    }

    public final void update(Graphics graphics) {
        this.paint(graphics);
    }

    public final void paint(Graphics graphics) {
        this.ba = this.bc.getGraphics();
        this.ba.setFont(this.ah);
        switch (a9) {
            case 0: {
                break;
            }
            case 1: {
                YmMain ymMain = this;
            }
        }
        if (this.a6.e() || this.a_) {
            this.ay = this.a6.i();
            this.ba.setColor(new Color(80, 0, 90));
            this.ba.fillRect(80, 0, a9 - 80, a8);
            this.ba.drawImage(this.av, 0, 0, null);
            this.ba.setColor(new Color(this.ay * 2 + 55, 0, 255));
            this.ba.fillRect(a9 / 4 + 30, a8 / 2 + 10, (int)((double)((float)(a9 / 2) * (float)this.ay) / 100.0), 10);
            this.ba.setColor(Color.white);
            this.ba.drawString("Loading and buffering music", a9 / 4 + 30, a8 / 2);
            this.ba.drawString(String.valueOf(String.valueOf(this.ay)) + " %", a9 * 3 / 4 + 4 + 30, a8 / 2 + 20);
            if (this.aa.al(this.ac) != this.z) {
                this.a6 = null;
            }
            this.ba.drawLine(a9 / 4 + 30, a8 / 2 + 10, a9 * 3 / 4 + 30, a8 / 2 + 10);
            this.ba.drawLine(a9 / 4 + 30, a8 / 2 + 20, a9 * 3 / 4 + 30, a8 / 2 + 20);
            this.ba.drawLine(a9 / 4 + 30, a8 / 2 + 10, a9 / 4 + 30, a8 / 2 + 20);
            this.ba.drawLine(a9 / 4 * 3 + 30, a8 / 2 + 10, a9 / 4 * 3 + 30, a8 / 2 + 20);
            this.aw = 200L;
        } else {
            this.aw = this.a1;
            this.ba.setColor(new Color(80, 0, 90));
            this.ba.fillRect(80, 0, a9 - 80, a8);
            this.ba.drawImage(this.av, 0, 0, null);
            if (this.aa.al(this.ac) != this.z) {
                return;
            }
            this.a7.ah(this.ba, a9, a8, this.af, this.au, this.at, this.as, this.ar, this.ap);
            this.ba.setColor(Color.white);
            int n = 0;
            while (n < 16) {
                if (n != this.ao) {
                    this.ba.drawString(this.ai[n * 2], a9 / 4 + 20, 30 + n * 10);
                }
                ++n;
            }
            this.al += 5;
            if (this.al > 510) {
                this.al = 0;
            }
            this.am = this.al < 256 ? this.al : 510 - this.al;
            this.ba.setColor(new Color(255, this.am / 2 + 127, this.am / 2 + 127));
            this.ba.drawString(this.ai[this.ao * 2], a9 / 4 + 20, 30 + this.ao * 10);
            this.ba.setColor(Color.white);
            this.ba.fillRect(a9 / 4 + 20 - 8, 30 + this.an * 10 - 5, 4, 3);
        }
        this.ba.setColor(Color.white);
        this.ba.setFont(this.ag);
        this.ba.drawString("Yamaha 2149 soundchip emulator v0.22", 85, 12);
        graphics.drawImage(this.bc, 0, 0, a9, a8, null);
        this.k();
    }

    public final boolean mouseDown(Event event, int n, int n2) {
        if (this.a6.e()) {
            return true;
        }
        this.ao = (n2 + 10 - 30) / 10;
        if (this.ao < 0) {
            this.ao = 0;
        }
        if (this.ao > 15) {
            this.ao = 15;
        }
        this.an = this.ao;
        this.aw = 500L;
        this.j();
        return true;
    }

    public final boolean mouseMove(Event event, int n, int n2) {
        this.ao = (n2 + 10 - 30) / 10;
        if (this.ao < 0) {
            this.ao = 0;
        }
        if (this.ao > 15) {
            this.ao = 15;
        }
        return true;
    }

    public final void j() {
        System.gc();
        if (!this.aa.ak().equals(this.ac)) {
            return;
        }
        this.a_ = true;
        try {
            this.aj = this.getCodeBase().toString();
            this.aj = String.valueOf(this.aj) + this.ai[this.an * 2 + 1];
            this.ak = new URL(this.aj);
        }
        catch (MalformedURLException malformedURLException) {
            malformedURLException.printStackTrace();
            System.out.println("MalformedURL...");
            return;
        }
        try {
            this.a6.f(this.ak, true);
        }
        catch (IOException iOException) {
            iOException.printStackTrace();
            System.out.println("YMPlayer: Init failed");
        }
        System.gc();
        this.showStatus("Yamaha2149 Emulator by Equinox");
        this.aw = this.a1;
        this.a_ = false;
        this.a0 = 0;
        ++this.af;
        if (this.af > 4) {
            this.af = 1;
        }
        if (this.af == 1) {
            this.a7.dl = 0;
            this.a7.de = 1272;
        }
    }

    public YmMain() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
        this.ao = 3;
        this.an = 3;
        this.aj = new String();
        this.ai = new String[34];
        this.ah = new Font("Arial", 1, 10);
        this.ag = new Font("Arial", 1, 11);
        this.z = 1985;
    }
}

