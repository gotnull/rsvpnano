/*
 * Decompiled with CFR 0.152.
 */
public class g {
    public long ev;
    public long eu;
    public int et;
    public long es;
    public long er;
    public boolean eq = false;
    public boolean ep = false;
    public int[] eo = new int[10000];

    public final void am(byte by, byte by2) {
        double d2 = (double)((by & 0xF) << 8) + (double)(by2 & 0xFF);
        if (d2 == 0.0) {
            this.et = 0;
            this.ev = 0;
            return;
        }
        this.ev = (long)(2000000.0 / (d2 * 16.0) / 44100.0 * 2.0 * 65536.0 * 256.0);
        this.et = this.et == 0 ? 16 : this.et;
    }
}

