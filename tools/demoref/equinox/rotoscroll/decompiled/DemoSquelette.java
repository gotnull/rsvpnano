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

public class DemoSquelette
extends Applet
implements Runnable {
    public Thread r;
    public b q;
    public Image p;
    public Graphics o;
    public static Image n;
    public static int m;
    public static int l;

    public final void init() {
        this.q = new b();
        this.q.d();
        this.p = this.createImage(320, 200);
        this.r = null;
        this.setLayout(null);
        this.resize(320, 200);
        this.f(this.getDocumentBase(), "ballred.gif");
        int n = 0;
        int n2 = 0;
        while (n2 < 17) {
            n += "THE EQUINOX TOUCH".charAt(n2);
            ++n2;
        }
        while (n != 1229) {
        }
    }

    public final void start() {
        if (this.r == null) {
            this.r = new Thread(this);
        }
        this.r.start();
    }

    public final void stop() {
        if (this.r != null) {
            this.r = null;
        }
    }

    public final void run() {
        while (this.r != null) {
            try {
                Thread.sleep(20L);
            }
            catch (InterruptedException interruptedException) {}
            this.repaint();
        }
    }

    public final void update(Graphics graphics) {
        this.o = this.p.getGraphics();
        this.o.setColor(Color.black);
        this.o.fillRect(0, 0, 320, 200);
        this.q.e(this.o);
        this.o.setColor(Color.white);
        this.o.drawString("THE EQUINOX TOUCH", 100, 100);
        graphics.drawImage(this.p, 0, 0, 320, 200, this);
    }

    public final void paint(Graphics graphics) {
    }

    private final void f(URL uRL, String string) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {}
        int n = 6;
        int n2 = 6;
        m = image.getWidth(null);
        l = image.getHeight(null);
        int[] nArray = new int[m * l];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, m, l, nArray, 0, m);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {}
        DemoSquelette.n = this.createImage(new MemoryImageSource(m, l, nArray, 0, m));
    }
}

