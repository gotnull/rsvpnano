/*
 * Decompiled with CFR 0.152.
 */
public class j {
    private int a;
    public int[] b;
    public int[] c;
    public int[] d;
    public int[] e;
    public int[] f;
    public int[] g;
    public int[] h;
    public int i;
    public int j;
    public int k;
    public int[] l;
    private static int m;
    private int n;
    private int o;
    private int p = 0;
    private static final int q = 280;
    private static final int r = 280;
    private float s = 0.0f;
    private byte[] t = new byte[78400];
    private byte[] u = new byte[78400];
    private byte[] v = new byte[78400];
    private float[] w = new float[256];
    private int x;
    private String y;
    private byte z;
    private Thread A;

    public void a(int[] nArray, int n2) {
        this.z = 0;
        this.x = 0;
        while (this.x < 256) {
            this.w[this.x] = ((float)this.x + 0.0f) / (float)n2;
            ++this.x;
        }
        this.x = 78399;
        while (this.x >= 0) {
            this.t[this.x] = (byte)(nArray[this.x] >> 16 & 0xFF);
            this.u[this.x] = (byte)(nArray[this.x] >> 8 & 0xFF);
            this.v[this.x] = (byte)(nArray[this.x] & 0xFF);
            --this.x;
        }
    }

    public void b(int[] nArray, int n2) {
        this.A = null;
        this.x = 78399;
        while (this.x >= 0) {
            nArray[this.x] = ((int)(this.w[this.t[this.x] & 0xFF] * (float)n2) & 0xFF) << 16 | ((int)(this.w[this.u[this.x] & 0xFF] * (float)n2) & 0xFF) << 8 | (int)(this.w[this.v[this.x] & 0xFF] * (float)n2) & 0xFF;
            --this.x;
        }
    }

    public void a(int n2, int n3, int n4) {
        m = n2;
        this.b = new int[m];
        this.c = new int[m];
        this.d = new int[m];
        this.e = new int[m];
        this.f = new int[m];
        this.g = new int[m];
        this.h = new int[m * 2 + 1];
        for (int i2 = 0; i2 < m; ++i2) {
            this.b[i2] = (int)(1100.0 * Math.random()) - 550;
            this.c[i2] = (int)(1100.0 * Math.random()) - 550;
            this.d[i2] = -2000;
        }
        this.i = 5000;
        this.j = 3000;
        this.k = 7000;
        this.a();
    }

    public void a(byte[] byArray, int n2, int[] nArray, int[] nArray2, int[] nArray3) {
        int n3;
        int n4;
        int n5 = 1;
        int n6 = 0;
        int n7 = this.h[0];
        if (n7 != 0) {
            int n8 = 1;
            for (n4 = 0; n4 < n7; ++n4) {
                if ((byArray[n3 = this.h[n8++]] & 0xFF) == 0) {
                    byArray[n3] = (byte)this.h[n8];
                }
                ++n8;
            }
        }
        n5 = 1;
        for (n4 = 0; n4 < n2; ++n4) {
            int n9 = nArray3[n4];
            float f2 = 300.0f / (300.0f - (float)n9);
            n3 = (int)((float)nArray[n4] * f2) + 140;
            int n10 = (int)((float)nArray2[n4] * f2) + 140;
            if (n3 >= 0 && n3 < 280 && n10 >= 0 && n10 < 280) {
                n7 = n3 + n10 * 280;
                int n11 = (2046 + n9 >> 4) + 128;
                if ((byArray[n7] & 0xFF) == 0) {
                    byArray[n7] = (byte)n11;
                }
                this.h[n5++] = n7;
                this.h[n5++] = n11;
                ++n6;
            }
            if (n9 < -100) {
                int n12 = n4;
                nArray3[n12] = nArray3[n12] + 16;
                continue;
            }
            int n13 = n4;
            nArray3[n13] = nArray3[n13] - 1936;
        }
        this.h[0] = n6;
    }

    public void a(byte[] byArray) {
        this.a(byArray, this.l);
    }

    public void a(byte[] byArray, byte[] byArray2) {
        this.z = 0;
        for (int i2 = 0; i2 < 544; ++i2) {
            byArray2[i2] = (byArray[i2 / 8] >> 7 - i2 % 8 & 1) == 1 ? -1 : 0;
        }
    }

    public void a(byte[] byArray, int[] nArray) {
        j j2;
        block8: {
            boolean bl;
            block11: {
                block13: {
                    block12: {
                        block10: {
                            bl = Troisd.gq;
                            if (this.n != 0 || this.o != 0) break block10;
                            this.n = 1;
                            if (!bl) break block11;
                        }
                        if (this.n != 1 || this.o != 0) break block12;
                        this.o = 1;
                        if (!bl) break block11;
                    }
                    if (this.n != 1 || this.o != 1) break block13;
                    this.n = 0;
                    if (!bl) break block11;
                }
                if (this.n == 0 && this.o == 1) {
                    this.o = 0;
                }
            }
            int n2 = (int)(Math.sin((double)this.s / 2.0 + 1.0) * Math.cos(this.s * 3.0f) * 280.0 / 2.0 + 140.0);
            int n3 = (int)(Math.cos((double)(-this.s) * 2.0 + 1.0) * 280.0 / 2.0 + 140.0);
            int n4 = (int)(Math.cos((double)this.s / 4.0 + 2.0) * 280.0 / 2.0 + 140.0);
            int n5 = (int)(Math.sin((double)(-this.s) * 3.0) * 280.0 / 2.0 + 140.0);
            int n6 = (int)(Math.sin((double)this.s * 2.0 + 1.0) * 280.0 / 2.0 + 140.0);
            int n7 = (int)(Math.sin((double)(-this.s) / 4.0 * 3.0 + 2.0) * 280.0 / 2.0 + 140.0);
            this.A = null;
            int n8 = this.n;
            while (n8 < 280) {
                block9: {
                    int n9 = n8 + this.o * 280;
                    this.z = 1;
                    int n10 = Math.abs(n8 - n2) * 600;
                    int n11 = Math.abs(n8 - n3) * 600;
                    int n12 = Math.abs(n8 - n4) * 600;
                    j2 = this;
                    if (bl) break block8;
                    for (int i2 = (v43672).o; i2 < 280; i2 += 2) {
                        int n13 = Math.abs(i2 - n5);
                        int n14 = Math.abs(i2 - n6);
                        int n15 = Math.abs(i2 - n7);
                        byArray[n9] = (byte)(nArray[n10 + n13] + nArray[n11 + n14] + nArray[n12 + n15]);
                        n9 += 560;
                        if (!bl) {
                            if (!bl) continue;
                        }
                        break block9;
                    }
                    n8 += 2;
                }
                if (!bl) continue;
            }
            ++this.p;
            j2 = this;
        }
        j2.s = (float)((double)j2.s + 0.007);
    }

    public void a(byte[] byArray, int n2) {
        this.a(byArray, n2, this.b, this.c, this.d);
    }

    public void a(int n2, float f2) {
        int n3 = n2;
        int n4 = 0;
        for (int i2 = n3 * 10; i2 < n3 * 10 + 10; ++i2) {
            n4 = i2 * 600 + 600 - 1;
            for (int i3 = 599; i3 >= 0; --i3) {
                this.l[n4--] = (int)(Math.sin(Math.sqrt(i2 * i2 + i3 * i3) / (double)f2) * 21.0 + 21.0);
            }
        }
    }

    public void b(int n2, float f2) {
        int n3 = n2;
        int n4 = n3 / 10;
        for (int i2 = (n3 + 3) % 10 * 60; i2 < (n3 + 3) % 10 * 60 + 60; ++i2) {
            int n5 = i2 * 600 + n4;
            for (int i3 = n4; i3 < 600; i3 += 10) {
                this.l[n5] = (int)(Math.sin(Math.sqrt(i2 * i3 + i2 * i3) / 30.0) * 21.0 + 21.0);
            }
            n5 += 10;
        }
    }

    private void a() {
        this.l = new int[360000];
        for (int i2 = 599; i2 >= 0; --i2) {
            int n2 = i2 * 600 + 600 - 1;
            for (int i3 = 599; i3 >= 0; --i3) {
                this.l[n2--] = (int)(Math.sin(Math.sqrt(i2 * i2 + i3 * i3) / 20.0) * 21.0 + 21.0);
            }
        }
    }
}

