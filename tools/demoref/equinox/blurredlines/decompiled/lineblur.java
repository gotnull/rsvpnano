/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;

public class lineblur
extends Applet
implements Runnable {
    public Thread ae;
    public Image ad;
    public byte[] ac;
    public byte[] ab;
    public int[] aa;
    public MemoryImageSource z;
    public a w;
    public int v;
    public long u;
    public long t;
    public long s;
    public long q;
    public int p;
    public byte[] o;
    public byte[] n;
    public byte[] m;
    public int l;
    public long k;
    public byte[] j;
    public int i;
    public int h;
    public int f;
    public byte[] e;
    public byte[] d;

    public final void init() {
        this.w = new a();
        this.h = 1;
        byte[] cfr_ignored_0 = new byte[1350];
        this.e = new byte[2750];
        switch (this.h) {
            case 0: {
                lineblur lineblur2 = this;
                break;
            }
            case 1: {
                this.v = 400;
                int[] cfr_ignored_1 = new int[1350];
                this.b(this.d, this.e);
                break;
            }
        }
        this.getToolkit().getScreenSize();
        int n = -62250;
        this.i = 0;
        this.l = this.v - 399;
        this.j = new byte[62500];
        int n2 = 0;
        while (n2 < n) {
            this.j[n2] = -1;
            ++n2;
        }
        this.ae = null;
        this.setLayout(null);
        this.ac = new byte[62500];
        this.ab = new byte[62500];
        this.d();
        this.c();
        this.f();
        this.w.u(this.v, 250, 250);
        this.i = 2;
        this.z.newPixels(0, 0, 250, 250, true);
        this.s = 1L;
        this.k = 0L;
        this.q = -1L;
    }

    public final void destroy() {
        this.ad.flush();
        this.ac = null;
        this.j = null;
    }

    public final void start() {
        if (this.ae == null) {
            this.ae = new Thread(this);
            this.ae.start();
        }
    }

    public final void stop() {
        if (this.ae != null) {
            this.ae.stop();
            this.ae = null;
        }
    }

    public final void run() {
        while (this.ae != null) {
            try {
                Thread.sleep(this.s);
            }
            catch (InterruptedException interruptedException) {}
            this.a(this.ac, this.ab, this.aa);
            this.w.s(this.ac);
            this.e(this.ac, this.e);
            this.g();
            this.z.newPixels(0, 0, 250, 250, true);
            this.repaint();
        }
    }

    private final void g() {
        ++this.k;
        this.t = System.currentTimeMillis();
        if (this.t - this.u > 1000L) {
            this.k = (int)(1000L * this.k / (this.t - this.u));
            this.u = this.t;
            this.q = -this.q;
            if (this.p >= 1 && this.q < 0L && this.p < 12) {
                if (this.k <= 10L) {
                    this.s += -4L;
                }
                if (this.k <= 20L && this.k > 10L) {
                    this.s += -3L;
                    this.i = 0;
                }
                if (this.k <= 30L && this.k > 20L) {
                    this.s += -2L;
                }
                if (this.k <= 60L && this.k >= 30L) {
                    this.s += -1L;
                }
                if (this.k > 80L) {
                    ++this.s;
                }
                if (this.s > 14L) {
                    this.s = 14L;
                }
            }
            if (this.p == 14) {
                if (this.k > 80L) {
                    ++this.s;
                }
                if (this.k < 70L) {
                    this.s += -1L;
                }
            }
            if (this.s < 1L) {
                this.s = 1L;
            }
            ++this.p;
            this.k = 0L;
        }
    }

    public final void update(Graphics graphics) {
        this.i = 1;
        if (this.ad != null) {
            graphics.drawImage(this.ad, 0, 0, 250, 250, this);
        }
    }

    private final void f() {
        switch (this.l) {
            case 0: {
                lineblur lineblur2 = this;
                IndexColorModel indexColorModel = new IndexColorModel(24, 65288, this.m, this.o, this.m);
                this.z = new MemoryImageSource(250, 250, (ColorModel)indexColorModel, this.ac, 0, 250);
                this.z.setAnimated(false);
                break;
            }
            case 1: {
                this.v = 400;
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.o, this.n, this.m);
                this.z = new MemoryImageSource(250, 250, (ColorModel)indexColorModel, this.ac, 0, 250);
                this.i = 0;
                this.z.setAnimated(true);
                break;
            }
        }
        this.z.setFullBufferUpdates(true);
        this.ad = this.createImage(this.z);
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void e(byte[] byArray, byte[] byArray2) {
        this.f = 2749;
        while (this.f >= 0) {
            if (byArray2[this.f] != 0) {
                byArray[this.f + 500] = -1;
            }
            --this.f;
        }
    }

    private final void d() {
        this.o = new byte[256];
        this.n = new byte[256];
        this.m = new byte[256];
        int n = 0;
        while (n < 127) {
            this.o[n] = (byte)((double)n / 1.5);
            this.n[n] = (byte)((double)n / 1.5);
            this.m[n] = (byte)n;
            float cfr_ignored_0 = this.m[n];
            ++n;
        }
        int n2 = 127;
        while (n2 < 240) {
            this.o[n2] = (byte)((double)n2 / 1.5);
            this.n[n2] = (byte)((double)n2 / 1.5);
            this.m[n2] = (byte)n2;
            ++n2;
        }
        int n3 = 127;
        while (n3 < 200) {
            this.o[n3] = (byte)((double)n3 / 1.5);
            this.n[n3] = (byte)((double)n3 / 1.5);
            this.m[n3] = (byte)n3;
            ++n3;
        }
        if (this.h == 2) {
            this.i = 0;
        }
        int n4 = 220;
        while (n4 < 240) {
            this.o[n4] = (byte)((double)n4 / 1.5);
            this.n[n4] = (byte)((double)n4 / 1.5);
            this.m[n4] = (byte)n4;
            ++n4;
        }
        ++this.i;
        int n5 = 240;
        while (n5 < 245) {
            this.o[n5] = (byte)((double)n5 / (double)1.4f);
            this.n[n5] = (byte)((double)n5 / (double)1.4f);
            this.m[n5] = (byte)n5;
            ++n5;
        }
        int n6 = 245;
        while (n6 < 250) {
            this.o[n6] = (byte)((double)n6 / (double)1.3f);
            this.n[n6] = (byte)((double)n6 / (double)1.3f);
            this.m[n6] = (byte)n6;
            ++n6;
        }
        int n7 = 250;
        while (n7 < 256) {
            this.o[n7] = (byte)((double)n7 / (double)1.2f);
            this.n[n7] = (byte)((double)n7 / (double)1.2f);
            this.m[n7] = (byte)n7;
            ++n7;
        }
        this.o[255] = -44;
        this.n[255] = -44;
        this.m[255] = -1;
    }

    /*
     * Enabled aggressive block sorting
     */
    private final void c() {
        this.aa = new int[62500];
        int n = 0;
        float f = -10.0f;
        float f2 = 0.017453f;
        float f3 = (float)(1.0 / (400.0 - (double)f));
        switch (this.l) {
            case 0: {
                int n2 = -125;
                block4: while (true) {
                    if (++n2 >= 125) {
                        return;
                    }
                    int n3 = -125;
                    while (true) {
                        if (n3 >= 125) continue block4;
                        int n4 = (int)(400.0 * (double)n2 * (double)f3);
                        int n5 = (int)(400.0 * (double)n3 * (double)f3);
                        int n6 = (int)((double)n4 * Math.sin(f2) - (double)n5 * Math.sin(f2));
                        int n7 = (int)((double)n4 * Math.cos(f2) + (double)n5 * Math.cos(f2));
                        this.aa[n] = 31375;
                        ++n;
                        ++n3;
                    }
                    break;
                }
            }
            case 1: {
                int n8 = -125;
                block6: while (++n8 < 125) {
                    int n9 = -125;
                    while (true) {
                        if (n9 >= 125) continue block6;
                        int n10 = (int)(400.0 * (double)n9 * (double)f3);
                        int n11 = (int)(400.0 * (double)n8 * (double)f3);
                        int n12 = (int)((double)n10 * Math.cos(f2) - (double)n11 * Math.sin(f2));
                        int n13 = (int)((double)n10 * Math.sin(f2) + (double)n11 * Math.cos(f2));
                        this.aa[n] = n12 + 125 + 250 * (n13 + 125);
                        ++n;
                        ++n9;
                    }
                    break;
                }
                return;
            }
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void b(byte[] byArray, byte[] byArray2) {
        int n = 0;
        while (++n < 2742) {
            if ((byArray[n / 8] >> 7 - n % 8 & 1) == 1) {
                byArray2[n] = -1;
                continue;
            }
            byArray2[n] = 0;
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void a(byte[] byArray, byte[] byArray2, int[] nArray) {
        int n = 62500;
        int n2 = 0;
        int n3 = 0;
        int n4 = 1;
        int n5 = 2;
        n2 = n - 1;
        while (n2 >= 0) {
            int n6 = nArray[n2];
            byArray2[n2] = byArray[n6] > -123 ? (int)(byArray[n6] - 4) : -127;
            --n2;
        }
        ++this.i;
        switch (this.l) {
            case 0: {
                this.i = 0;
                n3 = byArray2[250];
                n4 = byArray2[251];
                n5 = byArray2[252];
                break;
            }
            case 1: {
                n3 = byArray2[250];
                n4 = byArray2[251];
                n5 = byArray2[252];
                break;
            }
        }
        n2 = 251;
        while (n2 < 62249) {
            n5 = byArray2[n2 + 1];
            byArray[n2] = (byte)(n3 + n5 + byArray2[n2 - 250] + byArray2[n2 + 250] >> 2);
            n3 = n4;
            n4 = n5;
            ++n2;
        }
        n2 = 249;
        while (n2 >= 0) {
            byArray[n2] = -128;
            --n2;
        }
        n2 = 62250;
        while (n2 < 62500) {
            byArray[n2] = -128;
            ++n2;
        }
    }

    public lineblur() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
        this.h = 1;
        byte[] byArray = new byte[343];
        byArray[12] = -128;
        byArray[15] = 4;
        byArray[16] = 32;
        byArray[19] = -128;
        byArray[23] = 64;
        byArray[27] = 4;
        byArray[31] = 79;
        byArray[32] = -114;
        byArray[33] = 16;
        byArray[34] = -100;
        byArray[35] = -60;
        byArray[36] = 56;
        byArray[37] = 66;
        byArray[43] = 32;
        byArray[44] = -120;
        byArray[46] = 1;
        byArray[47] = 8;
        byArray[54] = 16;
        byArray[57] = 32;
        byArray[58] = 1;
        byArray[61] = 32;
        byArray[62] = 18;
        byArray[63] = 4;
        byArray[64] = 68;
        byArray[65] = 34;
        byArray[66] = 49;
        byArray[67] = 17;
        byArray[68] = 16;
        byArray[69] = -128;
        byArray[74] = 8;
        byArray[75] = 34;
        byArray[78] = -124;
        byArray[85] = 4;
        byArray[88] = 8;
        byArray[90] = 64;
        byArray[92] = 8;
        byArray[93] = 8;
        byArray[94] = -126;
        byArray[95] = 9;
        byArray[96] = 8;
        byArray[97] = -118;
        byArray[98] = 72;
        byArray[99] = 34;
        byArray[100] = 64;
        byArray[105] = 3;
        byArray[106] = -33;
        byArray[107] = -25;
        byArray[108] = -120;
        byArray[109] = 33;
        byArray[110] = 28;
        byArray[111] = 121;
        byArray[112] = 18;
        byArray[113] = 120;
        byArray[114] = -30;
        byArray[115] = 32;
        byArray[116] = -15;
        byArray[117] = 56;
        byArray[118] = -16;
        byArray[119] = -25;
        byArray[120] = -128;
        byArray[121] = -16;
        byArray[122] = 120;
        byArray[123] = 119;
        byArray[124] = -126;
        byArray[125] = 62;
        byArray[126] = -126;
        byArray[127] = 66;
        byArray[128] = 34;
        byArray[129] = -110;
        byArray[130] = 8;
        byArray[131] = 96;
        byArray[137] = -118;
        byArray[138] = 33;
        byArray[139] = 18;
        byArray[140] = 16;
        byArray[141] = -120;
        byArray[142] = -94;
        byArray[143] = 68;
        byArray[144] = -111;
        byArray[145] = 68;
        byArray[146] = 80;
        byArray[147] = 34;
        byArray[148] = 65;
        byArray[149] = 34;
        byArray[150] = 68;
        byArray[151] = -128;
        byArray[152] = 68;
        byArray[153] = 17;
        byArray[154] = 34;
        byArray[155] = -127;
        byArray[156] = 8;
        byArray[157] = 32;
        byArray[158] = -112;
        byArray[159] = -120;
        byArray[160] = -108;
        byArray[161] = -126;
        byArray[162] = 24;
        byArray[168] = 34;
        byArray[169] = -120;
        byArray[170] = 68;
        byArray[171] = 4;
        byArray[172] = 35;
        byArray[173] = -24;
        byArray[174] = -111;
        byArray[175] = 36;
        byArray[176] = 81;
        byArray[177] = 8;
        byArray[178] = 8;
        byArray[179] = -109;
        byArray[180] = -56;
        byArray[181] = -97;
        byArray[182] = 35;
        byArray[183] = -111;
        byArray[184] = 4;
        byArray[185] = 79;
        byArray[186] = -96;
        byArray[187] = 66;
        byArray[188] = 8;
        byArray[189] = 36;
        byArray[190] = 34;
        byArray[191] = 37;
        byArray[192] = 32;
        byArray[193] = -119;
        byArray[199] = 8;
        byArray[200] = -94;
        byArray[201] = 17;
        byArray[202] = 2;
        byArray[203] = 16;
        byArray[204] = -126;
        byArray[205] = 36;
        byArray[206] = 73;
        byArray[207] = 20;
        byArray[208] = 66;
        byArray[209] = 2;
        byArray[210] = 37;
        byArray[211] = 18;
        byArray[212] = 36;
        byArray[213] = 8;
        byArray[214] = 4;
        byArray[215] = 65;
        byArray[216] = 18;
        byArray[217] = 8;
        byArray[218] = 32;
        byArray[219] = -127;
        byArray[220] = 17;
        byArray[221] = 8;
        byArray[222] = -120;
        byArray[223] = -60;
        byArray[224] = 68;
        byArray[225] = 32;
        byArray[230] = 2;
        byArray[231] = 40;
        byArray[232] = -124;
        byArray[233] = 72;
        byArray[234] = -124;
        byArray[235] = 34;
        byArray[236] = -119;
        byArray[237] = 18;
        byArray[238] = 69;
        byArray[239] = 17;
        byArray[240] = 68;
        byArray[241] = -119;
        byArray[242] = 68;
        byArray[243] = -119;
        byArray[244] = 18;
        byArray[245] = 1;
        byArray[246] = 18;
        byArray[247] = 68;
        byArray[248] = -118;
        byArray[249] = 8;
        byArray[250] = 62;
        byArray[251] = 56;
        byArray[252] = 60;
        byArray[253] = 114;
        byArray[254] = 48;
        byArray[255] = -31;
        byArray[256] = 8;
        byArray[262] = -119;
        byArray[263] = -103;
        byArray[264] = -30;
        byArray[265] = 66;
        byArray[266] = 7;
        byArray[267] = 30;
        byArray[268] = 60;
        byArray[269] = -111;
        byArray[270] = 56;
        byArray[271] = -119;
        byArray[272] = 60;
        byArray[273] = 79;
        byArray[274] = 34;
        byArray[275] = 56;
        byArray[276] = 96;
        byArray[277] = 60;
        byArray[278] = -111;
        byArray[279] = 28;
        byArray[280] = 100;
        byArray[282] = 2;
        byArray[295] = 64;
        byArray[296] = 16;
        byArray[297] = -128;
        byArray[299] = -128;
        byArray[303] = 8;
        byArray[311] = 1;
        byArray[314] = 96;
        byArray[326] = 16;
        byArray[330] = 32;
        byArray[334] = 2;
        this.d = byArray;
        int[] nArray = new int[]{123, 564, 89, 64, 679, 46, 41, 87};
        int[] nArray2 = new int[408];
        nArray2[3] = 255;
        nArray2[4] = 255;
        nArray2[5] = 255;
        nArray2[6] = 235;
        nArray2[7] = 235;
        nArray2[8] = 235;
        nArray2[9] = 215;
        nArray2[10] = 215;
        nArray2[11] = 215;
        nArray2[12] = 195;
        nArray2[13] = 195;
        nArray2[14] = 195;
        nArray2[15] = 175;
        nArray2[16] = 175;
        nArray2[17] = 175;
        nArray2[18] = 155;
        nArray2[19] = 155;
        nArray2[20] = 155;
        nArray2[21] = 135;
        nArray2[22] = 135;
        nArray2[23] = 135;
        nArray2[24] = 222;
        nArray2[25] = 148;
        nArray2[26] = 189;
        nArray2[27] = 214;
        nArray2[28] = 165;
        nArray2[29] = 189;
        nArray2[30] = 214;
        nArray2[31] = 132;
        nArray2[32] = 189;
        nArray2[33] = 214;
        nArray2[34] = 123;
        nArray2[35] = 189;
        nArray2[36] = 206;
        nArray2[37] = 132;
        nArray2[38] = 181;
        nArray2[39] = 206;
        nArray2[40] = 107;
        nArray2[41] = 181;
        nArray2[42] = 198;
        nArray2[43] = 198;
        nArray2[44] = 198;
        nArray2[45] = 198;
        nArray2[46] = 148;
        nArray2[47] = 173;
        nArray2[48] = 198;
        nArray2[49] = 115;
        nArray2[50] = 173;
        nArray2[51] = 198;
        nArray2[52] = 107;
        nArray2[53] = 173;
        nArray2[54] = 189;
        nArray2[55] = 115;
        nArray2[56] = 165;
        nArray2[57] = 189;
        nArray2[58] = 99;
        nArray2[59] = 156;
        nArray2[60] = 189;
        nArray2[61] = 90;
        nArray2[62] = 165;
        nArray2[63] = 173;
        nArray2[64] = 99;
        nArray2[65] = 148;
        nArray2[66] = 173;
        nArray2[67] = 82;
        nArray2[68] = 140;
        nArray2[69] = 173;
        nArray2[70] = 74;
        nArray2[71] = 148;
        nArray2[72] = 165;
        nArray2[73] = 82;
        nArray2[74] = 140;
        nArray2[75] = 156;
        nArray2[76] = 57;
        nArray2[77] = 132;
        nArray2[78] = 148;
        nArray2[79] = 66;
        nArray2[80] = 123;
        nArray2[81] = 148;
        nArray2[82] = 49;
        nArray2[83] = 115;
        nArray2[84] = 140;
        nArray2[85] = 49;
        nArray2[86] = 115;
        nArray2[87] = 140;
        nArray2[88] = 41;
        nArray2[89] = 115;
        nArray2[90] = 132;
        nArray2[91] = 41;
        nArray2[92] = 115;
        nArray2[93] = 132;
        nArray2[94] = 33;
        nArray2[95] = 107;
        nArray2[96] = 123;
        nArray2[97] = 33;
        nArray2[98] = 107;
        nArray2[99] = 115;
        nArray2[100] = 24;
        nArray2[101] = 99;
        nArray2[102] = 115;
        nArray2[103] = 16;
        nArray2[104] = 90;
        nArray2[105] = 107;
        nArray2[106] = 24;
        nArray2[107] = 90;
        nArray2[108] = 99;
        nArray2[109] = 8;
        nArray2[110] = 82;
        nArray2[111] = 99;
        nArray2[113] = 74;
        nArray2[114] = 90;
        nArray2[115] = 24;
        nArray2[116] = 82;
        nArray2[117] = 90;
        nArray2[118] = 8;
        nArray2[119] = 74;
        nArray2[120] = 90;
        nArray2[122] = 74;
        nArray2[123] = 90;
        nArray2[125] = 66;
        nArray2[126] = 82;
        nArray2[128] = 57;
        nArray2[129] = 74;
        nArray2[130] = 8;
        nArray2[131] = 74;
        nArray2[132] = 74;
        nArray2[134] = 49;
        nArray2[135] = 66;
        nArray2[136] = 16;
        nArray2[137] = 74;
        nArray2[138] = 66;
        nArray2[140] = 49;
        nArray2[141] = 57;
        nArray2[143] = 57;
        nArray2[144] = 57;
        nArray2[146] = 41;
        nArray2[147] = 57;
        nArray2[149] = 33;
        nArray2[150] = 49;
        nArray2[152] = 57;
        nArray2[153] = 49;
        nArray2[155] = 41;
        nArray2[156] = 49;
        nArray2[158] = 33;
        nArray2[159] = 41;
        nArray2[161] = 24;
        nArray2[162] = 33;
        nArray2[164] = 41;
        nArray2[165] = 33;
        nArray2[167] = 24;
        nArray2[168] = 33;
        nArray2[170] = 16;
        nArray2[171] = 16;
        nArray2[172] = 16;
        nArray2[173] = 57;
        nArray2[174] = 16;
        nArray2[176] = 33;
        nArray2[177] = 16;
        nArray2[179] = 8;
        nArray2[182] = 49;
        nArray2[185] = 33;
        nArray2[188] = 16;
        nArray2[192] = 255;
        nArray2[193] = 255;
        nArray2[194] = 255;
        nArray2[195] = 255;
        nArray2[196] = 222;
        nArray2[197] = 255;
        nArray2[198] = 255;
        nArray2[199] = 214;
        nArray2[200] = 255;
        nArray2[201] = 247;
        nArray2[202] = 181;
        nArray2[203] = 255;
        nArray2[204] = 239;
        nArray2[205] = 198;
        nArray2[206] = 255;
        nArray2[207] = 239;
        nArray2[208] = 165;
        nArray2[209] = 255;
        nArray2[210] = 231;
        nArray2[211] = 181;
        nArray2[212] = 255;
        nArray2[213] = 231;
        nArray2[214] = 148;
        nArray2[215] = 255;
        nArray2[216] = 222;
        nArray2[217] = 148;
        nArray2[218] = 255;
        nArray2[219] = 214;
        nArray2[220] = 165;
        nArray2[221] = 255;
        nArray2[222] = 214;
        nArray2[223] = 132;
        nArray2[224] = 255;
        nArray2[225] = 214;
        nArray2[226] = 123;
        nArray2[227] = 255;
        nArray2[228] = 206;
        nArray2[229] = 132;
        nArray2[230] = 249;
        nArray2[231] = 206;
        nArray2[232] = 107;
        nArray2[233] = 249;
        nArray2[234] = 198;
        nArray2[235] = 198;
        nArray2[236] = 255;
        nArray2[237] = 198;
        nArray2[238] = 148;
        nArray2[239] = 241;
        nArray2[240] = 198;
        nArray2[241] = 115;
        nArray2[242] = 241;
        nArray2[243] = 198;
        nArray2[244] = 107;
        nArray2[245] = 241;
        nArray2[246] = 189;
        nArray2[247] = 115;
        nArray2[248] = 233;
        nArray2[249] = 189;
        nArray2[250] = 99;
        nArray2[251] = 224;
        nArray2[252] = 189;
        nArray2[253] = 90;
        nArray2[254] = 233;
        nArray2[255] = 173;
        nArray2[256] = 99;
        nArray2[257] = 216;
        nArray2[258] = 173;
        nArray2[259] = 82;
        nArray2[260] = 208;
        nArray2[261] = 173;
        nArray2[262] = 74;
        nArray2[263] = 216;
        nArray2[264] = 165;
        nArray2[265] = 82;
        nArray2[266] = 208;
        nArray2[267] = 156;
        nArray2[268] = 57;
        nArray2[269] = 200;
        nArray2[270] = 148;
        nArray2[271] = 66;
        nArray2[272] = 191;
        nArray2[273] = 148;
        nArray2[274] = 49;
        nArray2[275] = 183;
        nArray2[276] = 140;
        nArray2[277] = 49;
        nArray2[278] = 183;
        nArray2[279] = 140;
        nArray2[280] = 41;
        nArray2[281] = 183;
        nArray2[282] = 132;
        nArray2[283] = 41;
        nArray2[284] = 183;
        nArray2[285] = 132;
        nArray2[286] = 33;
        nArray2[287] = 175;
        nArray2[288] = 123;
        nArray2[289] = 33;
        nArray2[290] = 175;
        nArray2[291] = 115;
        nArray2[292] = 24;
        nArray2[293] = 167;
        nArray2[294] = 115;
        nArray2[295] = 16;
        nArray2[296] = 158;
        nArray2[297] = 107;
        nArray2[298] = 24;
        nArray2[299] = 158;
        nArray2[300] = 99;
        nArray2[301] = 8;
        nArray2[302] = 150;
        nArray2[303] = 99;
        nArray2[305] = 142;
        nArray2[306] = 90;
        nArray2[307] = 24;
        nArray2[308] = 150;
        nArray2[309] = 90;
        nArray2[310] = 8;
        nArray2[311] = 142;
        nArray2[312] = 90;
        nArray2[314] = 142;
        nArray2[315] = 90;
        nArray2[317] = 134;
        nArray2[318] = 82;
        nArray2[320] = 125;
        nArray2[321] = 74;
        nArray2[322] = 8;
        nArray2[323] = 142;
        nArray2[324] = 74;
        nArray2[326] = 117;
        nArray2[327] = 66;
        nArray2[328] = 16;
        nArray2[329] = 142;
        nArray2[330] = 66;
        nArray2[332] = 117;
        nArray2[333] = 57;
        nArray2[335] = 125;
        nArray2[336] = 57;
        nArray2[338] = 109;
        nArray2[339] = 57;
        nArray2[341] = 101;
        nArray2[342] = 49;
        nArray2[344] = 125;
        nArray2[345] = 49;
        nArray2[347] = 109;
        nArray2[348] = 49;
        nArray2[350] = 101;
        nArray2[351] = 41;
        nArray2[353] = 92;
        nArray2[354] = 33;
        nArray2[356] = 109;
        nArray2[357] = 33;
        nArray2[359] = 92;
        nArray2[360] = 33;
        nArray2[362] = 84;
        nArray2[363] = 16;
        nArray2[364] = 16;
        nArray2[365] = 125;
        nArray2[366] = 16;
        nArray2[368] = 101;
        nArray2[369] = 16;
        nArray2[371] = 76;
        nArray2[374] = 117;
        nArray2[377] = 101;
        nArray2[380] = 84;
        nArray2[383] = 68;
        nArray2[387] = 34;
        nArray2[388] = 17;
        nArray2[390] = 68;
        nArray2[391] = 34;
        nArray2[393] = 102;
        nArray2[394] = 51;
        nArray2[396] = 135;
        nArray2[397] = 68;
        nArray2[399] = 170;
        nArray2[400] = 85;
        nArray2[402] = 204;
        nArray2[403] = 102;
        nArray2[405] = 238;
        nArray2[406] = 119;
    }
}

