/*
 * Decompiled with CFR 0.152.
 */
import b.e.a;
import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.zip.ZipInputStream;

public class f {
    private InputStream a;
    private DataInputStream b;
    private int c;
    private float d;
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
    private float p;
    private float q;
    private float r;
    private String s;
    private boolean t;
    private int u;
    private int v;
    private float w;

    public int a(URL uRL, String string, int n2, a a2, int n3, int n4, int n5) {
        int n6;
        block52: {
            block44: {
                boolean bl = Troisd.gq;
                this.s = f.a("zWZ-\u000bLM^yRBJ\u001b5DBTR7L\r^Of\u000byMB0EJ\u001fO6\u000b_VKyDCZ\u001b4D_Z\u001b-B@Z\u0004");
                this.a = null;
                this.b = null;
                this.c = 0;
                this.t = false;
                try {
                    block45: {
                        this.a = uRL.openStream();
                        if (this.a == null) break block44;
                        this.u = 1;
                        try {
                            int n7;
                            int n8;
                            block50: {
                                int n9;
                                block49: {
                                    int n10;
                                    float f2;
                                    block48: {
                                        int n11;
                                        block47: {
                                            float f3;
                                            float f4;
                                            float f5;
                                            float f6;
                                            float f7;
                                            float f8;
                                            block46: {
                                                int n12;
                                                block56: {
                                                    block55: {
                                                        if (!string.toLowerCase().endsWith(f.a("WVK"))) break block55;
                                                        ZipInputStream zipInputStream = null;
                                                        zipInputStream = new ZipInputStream(this.a);
                                                        zipInputStream.getNextEntry();
                                                        this.b = new DataInputStream(zipInputStream);
                                                        if (!bl) break block56;
                                                    }
                                                    this.b = new DataInputStream(this.a);
                                                }
                                                a2.e = n5;
                                                char c2 = this.b.readChar();
                                                char c3 = this.b.readChar();
                                                char c4 = this.b.readChar();
                                                char c5 = this.b.readChar();
                                                n9 = this.b.readInt();
                                                a2.b = this.b.readInt();
                                                a2.g = new float[a2.b];
                                                a2.h = new float[a2.b];
                                                a2.f = new float[a2.b];
                                                this.c += 12 * a2.b;
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                this.s = ";";
                                                for (int i2 = 0; i2 < a2.b; ++i2) {
                                                    a2.g[i2] = this.b.readFloat();
                                                    a2.h[i2] = this.b.readFloat();
                                                    a2.f[i2] = this.b.readFloat();
                                                    if (!bl) {
                                                        if (!bl) continue;
                                                    }
                                                    break block45;
                                                }
                                                f2 = 0.0f;
                                                this.u = 0;
                                                f8 = -100000.0f;
                                                f7 = 100000.0f;
                                                f6 = -100000.0f;
                                                f5 = 100000.0f;
                                                f4 = -100000.0f;
                                                f3 = 100000.0f;
                                                for (n10 = 0; n10 < a2.b; ++n10) {
                                                    this.t = true;
                                                    float f9 = a2.g[n10] - f8;
                                                    n12 = f9 == 0.0f ? 0 : (f9 > 0.0f ? 1 : -1);
                                                    if (!bl) {
                                                        if (n12 > 0) {
                                                            f8 = a2.g[n10];
                                                        }
                                                        if (a2.g[n10] < f7) {
                                                            f7 = a2.g[n10];
                                                        }
                                                        if (a2.h[n10] > f6) {
                                                            f6 = a2.h[n10];
                                                        }
                                                        if (a2.h[n10] < f5) {
                                                            f5 = a2.h[n10];
                                                        }
                                                        if (a2.f[n10] > f4) {
                                                            f4 = a2.f[n10];
                                                        }
                                                        if (!(a2.f[n10] < f3)) continue;
                                                        f3 = a2.f[n10];
                                                        if (!bl) continue;
                                                    }
                                                    break block46;
                                                }
                                                n12 = n10 = 0;
                                            }
                                            while (n10 < a2.b) {
                                                this.t = true;
                                                a2.g[n10] = a2.g[n10] - (f8 + f7) / 2.0f;
                                                a2.h[n10] = a2.h[n10] - (f6 + f5) / 2.0f;
                                                a2.f[n10] = a2.f[n10] - (f4 + f3) / 2.0f;
                                                ++n10;
                                                if (!bl) {
                                                    if (!bl) continue;
                                                }
                                                break block47;
                                            }
                                            n10 = 0;
                                        }
                                        while (n10 < a2.b) {
                                            this.t = true;
                                            ++this.v;
                                            float f10 = Math.abs(a2.g[n10]) - f2;
                                            n11 = f10 == 0.0f ? 0 : (f10 > 0.0f ? 1 : -1);
                                            if (!bl) {
                                                if (n11 > 0) {
                                                    f2 = Math.abs(a2.g[n10]);
                                                }
                                                if (Math.abs(a2.h[n10]) > f2) {
                                                    f2 = Math.abs(a2.h[n10]);
                                                }
                                                if (Math.abs(a2.f[n10]) > f2) {
                                                    f2 = Math.abs(a2.f[n10]);
                                                }
                                                ++n10;
                                                if (!bl) continue;
                                            }
                                            break block48;
                                        }
                                        n11 = n10 = 0;
                                    }
                                    while (n10 < a2.b) {
                                        ++this.v;
                                        a2.g[n10] = a2.g[n10] / f2 * (float)n2;
                                        a2.h[n10] = a2.h[n10] / f2 * (float)n2;
                                        ++this.v;
                                        a2.f[n10] = a2.f[n10] / f2 * (float)n2;
                                        ++n10;
                                        if (!bl) {
                                            if (!bl) continue;
                                        }
                                        break block49;
                                    }
                                    n10 = this.b.readChar();
                                }
                                char c6 = this.b.readChar();
                                char c7 = this.b.readChar();
                                char c8 = this.b.readChar();
                                ++this.v;
                                a2.c = this.b.readInt();
                                a2.i = new int[n9 + a2.c * 2];
                                this.c += 4 * (n9 + a2.c * 2);
                                this.s = f.a("hNN0EBG\u001b+^AZHx");
                                int n13 = this.b.readByte();
                                short s = this.b.readShort();
                                int n14 = 0;
                                int n15 = 0;
                                int n16 = 0;
                                this.t = true;
                                n8 = 0;
                                while (n8 < a2.c) {
                                    block51: {
                                        block58: {
                                            block57: {
                                                --this.u;
                                                n7 = n13;
                                                if (bl) break block50;
                                                if (n7 <= 3) break block57;
                                                n15 = this.b.readByte() - n8 % 57;
                                                if (!bl) break block58;
                                            }
                                            n15 = 3;
                                        }
                                        a2.i[n14++] = n15;
                                        --this.u;
                                        while (n15 > 0) {
                                            a2.i[n14++] = this.b.readShort() + n16;
                                            n16 = a2.i[n14 - 1];
                                            --n15;
                                            if (!bl) {
                                                if (!bl) continue;
                                            }
                                            break block51;
                                        }
                                        a2.i[n14++] = n3;
                                        a2.i[n14++] = n4;
                                        this.s = f.a("cjw\u0015");
                                        ++n8;
                                    }
                                    if (!bl) continue;
                                }
                                n8 = 0;
                                n7 = 0;
                            }
                            int n17 = n7;
                            boolean bl2 = false;
                            int n18 = 0;
                            int n19 = 0;
                            int n20 = 0;
                            this.t = false;
                            int[] nArray = new int[256];
                            float[] fArray = new float[256];
                            for (int i3 = 0; i3 < a2.c; ++i3) {
                                int n21;
                                int n22;
                                block54: {
                                    block53: {
                                        n17 = a2.i[n8++];
                                        n6 = 0;
                                        if (bl) break block52;
                                        for (n18 = v1214; n18 < n17; ++n18) {
                                            this.g = a2.g[a2.i[n8 + n18 % n17]];
                                            this.h = a2.h[a2.i[n8 + n18 % n17]];
                                            this.i = a2.f[a2.i[n8 + n18 % n17]];
                                            this.j = a2.g[a2.i[n8 + (1 + n18) % n17]];
                                            this.k = a2.h[a2.i[n8 + (1 + n18) % n17]];
                                            this.l = a2.f[a2.i[n8 + (1 + n18) % n17]];
                                            this.m = a2.g[a2.i[n8 + (2 + n18) % n17]];
                                            this.n = a2.h[a2.i[n8 + (2 + n18) % n17]];
                                            this.o = a2.f[a2.i[n8 + (2 + n18) % n17]];
                                            this.t = false;
                                            this.r = (this.j - this.g) * (this.n - this.h) - (this.m - this.g) * (this.k - this.h);
                                            this.p = (this.k - this.h) * (this.o - this.i) - (this.n - this.h) * (this.l - this.i);
                                            this.q = (this.l - this.i) * (this.m - this.g) - (this.o - this.i) * (this.j - this.g);
                                            fArray[n18] = Math.abs(this.p) + Math.abs(this.q) + Math.abs(this.r);
                                            if (!bl) {
                                                if (!bl) continue;
                                            }
                                            break block53;
                                        }
                                        n19 = 0;
                                        n20 = 0;
                                    }
                                    for (n18 = 0; n18 < n17; ++n18) {
                                        n22 = nArray[n18];
                                        n21 = n20;
                                        if (!bl) {
                                            if (n22 <= n21) continue;
                                            n19 = n18;
                                            n20 = nArray[n18];
                                            if (!bl) continue;
                                        }
                                        break block54;
                                    }
                                    System.arraycopy(a2.i, 0, nArray, 0, n19);
                                    System.arraycopy(a2.i, n19, a2.i, 0, n17 - n19);
                                    this.t = false;
                                    System.arraycopy(nArray, 0, a2.i, n17 - n19, n19);
                                    n22 = n8;
                                    n21 = 2 + n17;
                                }
                                n8 = n22 + n21;
                                if (!bl) continue;
                            }
                            this.t = false;
                            a2.a = true;
                        }
                        catch (Exception exception) {
                            // empty catch block
                        }
                    }
                    try {
                        if (this.b != null) {
                            this.b.close();
                        }
                    }
                    catch (Exception exception) {}
                }
                catch (Exception exception) {
                    // empty catch block
                }
            }
            try {
                if (this.a != null) {
                    this.a.close();
                }
            }
            catch (Exception exception) {
                // empty catch block
            }
            System.gc();
            n6 = this.c;
        }
        return n6;
    }

    /*
     * Handled impossible loop by adding 'first' condition
     * Enabled aggressive block sorting
     */
    private static String a(String string) {
        char[] cArray = string.toCharArray();
        int n2 = cArray.length;
        int n3 = 0;
        boolean bl = true;
        block6: do {
            if (bl && !(bl = false) && n2 > 1) continue;
            char[] cArray2 = cArray;
            int n4 = n3;
            while (true) {
                int n5;
                char c2 = cArray2[n4];
                switch (n3 % 5) {
                    case 0: {
                        n5 = 45;
                        break;
                    }
                    case 1: {
                        n5 = 63;
                        break;
                    }
                    case 2: {
                        n5 = 59;
                        break;
                    }
                    case 3: {
                        n5 = 89;
                        break;
                    }
                    default: {
                        n5 = 43;
                    }
                }
                cArray2[n4] = (char)(c2 ^ n5);
                ++n3;
                if (n2 != 0) continue block6;
                cArray2 = cArray;
                n4 = n2;
            }
        } while (n3 < n2);
        return new String(cArray);
    }
}

