/*
 * Decompiled with CFR 0.152.
 */
class o
extends Thread {
    private static int a;
    public static int b;
    public static int c;
    Troisd d;
    private float e;
    private float f;
    private float g;
    private float h;
    private float i;
    private float j;
    private float k;
    private float l;
    private float m;
    private float n;
    private float o;

    o() {
        a = 0;
    }

    public void a(Troisd troisd) {
        this.d = troisd;
        this.e = this.e;
        this.k = 0.0f;
        this.o = 0.9f;
        b = 0;
        c = 0;
    }

    public void run() {
        boolean bl = false;
        try {
            Thread.sleep(200L);
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        this.d.x();
        this.m = 128.0f;
        this.n = 0.025f;
        this.o = 0.2469f;
        this.stop();
    }
}

