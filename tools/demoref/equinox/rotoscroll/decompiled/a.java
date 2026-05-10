/*
 * Decompiled with CFR 0.152.
 */
public class a {
    public float[] a = new float[9];

    public a() {
        this.a[8] = 1.0f;
        this.a[4] = 1.0f;
        this.a[0] = 1.0f;
    }

    public final void a(float f) {
        this.a[0] = (float)Math.cos(f);
        this.a[1] = (float)Math.sin(f);
        this.a[2] = 0.0f;
        this.a[3] = (float)(-Math.sin(f));
        this.a[4] = (float)Math.cos(f);
        this.a[5] = 0.0f;
        this.a[6] = 0.0f;
        this.a[7] = 0.0f;
        this.a[8] = 1.0f;
    }
}

