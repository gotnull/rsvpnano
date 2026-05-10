/*
 * Decompiled with CFR 0.152.
 */
public class a
extends Thread {
    public static int e;
    public static int d;
    public VertRasters c;
    public float a;

    public final void a(VertRasters vertRasters) {
        this.c = vertRasters;
        this.a = this.a;
        e = 0;
        d = 0;
    }

    public final void run() {
        try {
            Thread.sleep(100L);
        }
        catch (InterruptedException interruptedException) {}
        this.c.j();
        d = 1;
        e = 1;
        this.c.af = 0;
        this.stop();
    }
}

