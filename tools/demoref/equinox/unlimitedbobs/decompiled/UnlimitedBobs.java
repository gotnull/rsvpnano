/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Color;
import java.awt.Event;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.image.MemoryImageSource;
import java.awt.image.PixelGrabber;
import java.net.URL;

public class UnlimitedBobs
extends Applet
implements Runnable {
    public Thread ba;
    public Image[] a9 = new Image[8];
    public a a8;
    public Graphics a7;
    public static Image a6;
    public static int a5;
    public static int a4;
    public static int a3;
    public static int a2;
    public int a1;
    public int a0 = 1;
    public int a_;
    public int az;
    public int b;
    public int ay;
    public int e;
    public double[][] c;

    public final void init() {
        int n;
        this.a8 = new a();
        this.b = 1;
        String string = this.getParameter("eqx");
        if (string.equalsIgnoreCase("1")) {
            this.ay = 1;
        } else {
            this.ay = 2;
            this.c[1][1] = -1.0;
        }
        switch (this.ay) {
            case 1: {
                a3 = 320;
                a2 = 200;
                break;
            }
            case 2: {
                a3 = 400;
                a2 = 300;
                break;
            }
        }
        this.a8.h();
        this.resize(a3, a2);
        int n2 = 0;
        while (n2 < 8) {
            n = 0;
            while (n < 28) {
                this.az += "http://equinox.planet-d.net/".charAt(n);
                ++n;
            }
            while (this.az != 2632) {
            }
            this.az = 0;
            this.a9[n2] = this.createImage(a3, a2);
            this.a7 = this.a9[n2].getGraphics();
            switch (this.ay) {
                case 1: {
                    int n3 = 0;
                    while (n3 < 3) {
                        this.e = 2;
                        int n4 = 0;
                        while (n4 < 3) {
                            int n5 = 0;
                            while (n5 < 3) {
                                this.c[n3][n4 + n5] = n3 + n4 - n5;
                                ++this.e;
                                ++n5;
                            }
                            ++n4;
                        }
                        ++n3;
                    }
                    this.a7.setColor(new Color(0, 0, 0));
                    break;
                }
                case 2: {
                    this.a7.setColor(new Color(170, 150, 200));
                    break;
                }
            }
            this.a7.fillRect(0, 0, a3, a2);
            this.a7.setColor(Color.white);
            this.a7.drawString("Click mouse to change the curves", 2, a2 - 2);
            this.a7.setColor(Color.white);
            this.a7.drawString("http://equinox.planet-d.net/", 3, 12);
            ++n2;
        }
        this.ba = null;
        this.setLayout(null);
        this.resize(a3, a2);
        this.i(this.getDocumentBase(), "sprites2.gif");
        n = 0;
        while (n < 17) {
            this.az += "THE EQUINOX TOUCH".charAt(n);
            ++n;
        }
        while (this.az != 1229) {
        }
    }

    public final void start() {
        if (this.ba == null) {
            this.ba = new Thread(this);
        }
        this.ba.start();
    }

    public final void stop() {
        if (this.ba != null) {
            this.ba = null;
        }
    }

    public final void run() {
        while (this.ba != null) {
            try {
                Thread.sleep(17L);
            }
            catch (InterruptedException interruptedException) {}
            this.repaint();
        }
    }

    public final void update(Graphics graphics) {
        int n;
        int n2;
        int n3;
        int n4 = 0;
        while (n4 < 8) {
            this.a7 = this.a9[n4].getGraphics();
            this.a7.setColor(Color.black);
            n3 = 0;
            while (n3 < 3) {
                this.e = 2;
                n2 = 0;
                while (n2 < 3) {
                    n = 0;
                    while (n < 3) {
                        this.c[n3][n2 + n] = n3 - n2 + n;
                        ++this.e;
                        ++n;
                    }
                    ++n2;
                }
                ++n3;
            }
            this.a8.g(this.a7, this.a_, this.ay);
            ++n4;
        }
        this.a7 = this.a9[this.a1].getGraphics();
        switch (this.ay) {
            case 1: {
                this.a7.setColor(new Color(0, 0, 0));
                break;
            }
            case 2: {
                this.c[1][2] = 1.0;
                this.a7.setColor(new Color(170, 150, 200));
                break;
            }
        }
        this.a7.fillRect(a3 - 100, a2 - 10, 100, 10);
        this.a7.setColor(Color.black);
        this.a7.drawString("THE EQUINOX TOUCH", a3 / 2 - 60 + 1, a2 / 2 + 1);
        this.a7.setColor(Color.white);
        this.a7.drawString("THE EQUINOX TOUCH", a3 / 2 - 60, a2 / 2);
        this.a7.drawString("SPRITES = " + String.valueOf(this.a0), a3 - 100, a2 - 1);
        graphics.drawImage(this.a9[this.a1], 0, 0, a3, a2, this);
        ++this.a1;
        n3 = 0;
        while (n3 < 3) {
            this.e = 2;
            n2 = 0;
            while (n2 < 3) {
                n = 0;
                while (n < 3) {
                    this.c[n3][n2 + n] = n3 - n2 + n;
                    ++this.e;
                    ++n;
                }
                ++n2;
            }
            ++n3;
        }
        if (this.a1 == 8) {
            this.a1 = 0;
        }
        ++this.a0;
    }

    public final void paint(Graphics graphics) {
    }

    public final boolean mouseDown(Event event, int n, int n2) {
        ++this.a_;
        if (this.a_ > 5) {
            this.a_ = 0;
        }
        int n3 = 0;
        while (n3 < 8) {
            int n4 = 0;
            while (n4 < 1) {
                this.e = -2;
                int n5 = 0;
                while (n5 < 2) {
                    int n6 = 0;
                    while (n6 < 1) {
                        this.c[n4 + n5][n6] = n4 + n5 + n6;
                        ++this.e;
                        ++n6;
                    }
                    ++n5;
                }
                ++n4;
            }
            this.a7 = this.a9[n3].getGraphics();
            this.a7.setColor(new Color(170 * (this.ay - 1), 150 * (this.ay - 1), 200 * (this.ay - 1)));
            this.a7.fillRect(0, 0, a3, a2);
            this.a7.setColor(Color.white);
            this.a7.drawString("Click mouse to change the curves", 2, a2 - 2);
            this.a7.setColor(Color.white);
            this.a7.drawString("http://equinox.planet-d.net/", 3, 12);
            ++n3;
        }
        this.a0 = 1;
        this.a8.h();
        return true;
    }

    private final void i(URL uRL, String string) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        if (this.b == 2) {
            try {
                mediaTracker.waitForAll();
            }
            catch (InterruptedException interruptedException) {
            }
        } else {
            try {
                mediaTracker.waitForAll();
            }
            catch (InterruptedException interruptedException) {}
        }
        switch (this.b) {
            case 1: {
                int n = 6;
                int n2 = 6;
                a5 = image.getWidth(null);
                a4 = image.getHeight(null);
                int[] nArray = new int[a5 * a4];
                PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, a5, a4, nArray, 0, a5);
                try {
                    pixelGrabber.grabPixels();
                }
                catch (InterruptedException interruptedException) {}
                a6 = this.createImage(new MemoryImageSource(a5, a4, nArray, 0, a5));
                break;
            }
            case 2: {
                int n = 8;
                int n3 = 8;
                a5 = image.getWidth(null);
                a4 = image.getHeight(null);
                int[] nArray = new int[a5 * a4];
                PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, a5, a4, nArray, 0, a5);
                try {
                    pixelGrabber.grabPixels();
                }
                catch (InterruptedException interruptedException) {}
                break;
            }
        }
    }

    public UnlimitedBobs() {
        int[] cfr_ignored_0 = new int[20];
        this.c = new double[4][10];
    }

    static {
        a3 = 400;
        a2 = 300;
    }
}

