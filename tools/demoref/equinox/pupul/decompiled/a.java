/*
 * Decompiled with CFR 0.152.
 */
public class a
extends Thread {
    public static int d;
    public static int c;
    public Pupul b;
    public float a;

    public final void a(Pupul pupul) {
        this.b = pupul;
        this.a = this.a;
        d = 0;
        c = 0;
    }

    public final void run() {
        try {
            Thread.sleep(500L);
        }
        catch (InterruptedException interruptedException) {}
        this.b.d();
        c = 1;
        this.b.c();
        d = 1;
        this.stop();
    }
}

