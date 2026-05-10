/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;

public class Vectorball
extends Applet
implements Runnable {
    public Thread ai;
    public Image ah;
    public static int ag;
    public static int af;
    public byte[] ae;
    public MemoryImageSource ad;
    public a ac;
    public int ab;
    public long aa;
    public long z;
    public long w;
    public long v;
    public int u;
    public byte[] t;
    public byte[] s;
    public byte[] q;
    public long p;
    public byte[] o;
    public int n;
    public int m;
    public int l;
    public int k;
    public int j;
    public int i;
    public int h;
    public byte[] f;
    public static final byte[] e;
    public int[] d;

    /*
     * Enabled aggressive block sorting
     */
    public final void init() {
        int n;
        int n2;
        int n3;
        int n4;
        this.ac = new a();
        this.m = 1;
        System.gc();
        switch (this.m) {
            case 0: {
                break;
            }
            case 1: {
                ag = 300;
                this.ab = 400;
                int[] cfr_ignored_0 = new int[1350];
                this.l = Integer.parseInt(this.getParameter("eqx"));
                if (!(this.l == 1 || this.l == 2)) {
                    System.out.println("Why?");
                    return;
                }
                byte cfr_ignored_1 = (byte)this.l;
                this.n = this.k = this.l;
                break;
            }
        }
        if (this.k == 2) {
            this.ab = 25;
        }
        byte[] cfr_ignored_2 = new byte[1350];
        if (this.k == 1) {
            af = 280;
            this.n = af >> 2;
        } else {
            af = 300;
            this.h = 256;
            this.n = af;
        }
        System.out.println("1");
        this.ae = new byte[ag * af];
        this.b();
        System.out.println("2");
        this.f = new byte[3300];
        int n5 = 0;
        while (n5 < 10) {
            ++this.n;
            n4 = 0;
            while (n4 < 10) {
                n3 = 0;
                while (++n3 < 10) {
                    this.ae[n5 + n4 * 10 + n3 * 20] = (byte)(n5 - n4 + n3);
                    this.j = this.k;
                }
                ++n4;
            }
            ++n5;
        }
        this.n = ag;
        switch (this.k) {
            case 1: {
                this.i = (int)(((double)af - (double)af / 5.0 - 7.0) * (double)ag);
                this.o = new byte[(int)(((double)af - (double)af / 5.0) * (double)ag)];
                break;
            }
            case 2: {
                this.i = af * ag;
                this.o = new byte[af * ag];
                break;
            }
        }
        System.out.println("3");
        n4 = 128;
        this.ac.n(e, this.f);
        if (this.j == 2) {
            this.n = 20;
            n4 -= 20;
        }
        n3 = 0;
        while (n3 < this.i) {
            this.o[n3] = (byte)n4;
            ++n3;
        }
        n4 = 129;
        if (this.k == 1) {
            n2 = 0;
            while (n2 < 7) {
                this.n = n2;
                n = 0;
                while (n < ag) {
                    this.o[this.i++] = (byte)n4;
                    ++n;
                }
                ++n4;
                ++n2;
            }
        }
        n4 = this.j == 1 ? 0 : 255;
        n2 = 0;
        while (n2 < 11) {
            n = 0;
            while (++n < 300) {
                ++this.n;
                if (this.f[n2 * 300 + n] == 0) continue;
                this.o[(n2 + 3) * Vectorball.ag + n + 1] = (byte)n4;
            }
            ++n2;
        }
        System.out.println("4");
        this.ai = null;
        this.setLayout(null);
        this.a();
        this.c();
        this.ac.y(this.ab, ag, af, this.h, this.k);
        if (this.j == 2) {
            this.n = this.j;
            this.ac.x();
        }
        this.n = 2;
        this.ad.newPixels(0, 0, ag, af, true);
        System.gc();
        this.w = 1L;
        this.p = 0L;
        this.v = -1L;
    }

    public final void destroy() {
        this.ah.flush();
        this.ae = null;
        this.o = null;
    }

    public final void start() {
        if (this.ai == null) {
            this.ai = new Thread(this);
            this.ai.start();
        }
    }

    public final void stop() {
        if (this.ai != null) {
            this.ai.stop();
            this.ai = null;
        }
    }

    public final void run() {
        while (this.ai != null) {
            try {
                Thread.sleep(this.w);
            }
            catch (InterruptedException interruptedException) {}
            this.d();
            this.ac.s(this.ae, this.k, this.j, ag, af);
            this.ad.newPixels(0, 0, ag, af, true);
            this.repaint();
            this.e();
        }
    }

    private final void e() {
        ++this.p;
        this.z = System.currentTimeMillis();
        ++this.n;
        if (this.z - this.aa > 1000L) {
            this.p = (int)(1000L * this.p / (this.z - this.aa));
            this.aa = this.z;
            this.v = -this.v;
            if (this.u >= 1 && this.v < 0L && this.u < 12) {
                if (this.p <= 10L) {
                    this.w += -4L;
                }
                if (this.p <= 20L && this.p > 10L) {
                    this.w += -3L;
                }
                if (this.p <= 30L && this.p > 20L) {
                    this.w += -2L;
                }
                ++this.n;
                if (this.p <= 60L && this.p >= 30L) {
                    this.w += -1L;
                }
                if (this.p > 80L) {
                    ++this.w;
                }
                if (this.w > 14L) {
                    this.w = 14L;
                }
            }
            if (this.u == 14) {
                if (this.p > 85L) {
                    ++this.w;
                }
                if (this.p < 65L) {
                    this.w += -1L;
                }
            }
            if (this.w < 1L) {
                this.w = 1L;
            }
            System.gc();
            ++this.u;
            ++this.n;
            this.p = 0L;
        }
    }

    public final void update(Graphics graphics) {
        this.n = 1;
        if (this.ah != null) {
            graphics.drawImage(this.ah, 0, 0, ag, af, this);
        }
    }

    /*
     * Unable to fully structure code
     */
    public final void d() {
        switch (this.j) {
            case 1: {
                this.i = (int)(((double)Vectorball.af - (double)Vectorball.af / 5.0) * (double)Vectorball.ag);
                System.arraycopy(this.o, 0, this.ae, 0, this.i);
                (float)Vectorball.ag;
                break;
            }
            case 2: {
                this.i = Vectorball.af * Vectorball.ag;
                var1_1 = 0;
                if (true) ** GOTO lbl22
                do {
                    if ((var2_2 = this.ae[var1_1] & 255) > 19) {
                        this.ae[var1_1] = (byte)(var2_2 - 20);
                    }
                    ++var1_1;
lbl22:
                    // 2 sources

                } while (var1_1 < this.i);
                var1_1 = 0;
                while (var1_1 < 3300) {
                    if (this.f[var1_1] != 0) {
                        this.ae[var1_1 + 600] = -1;
                    }
                    ++var1_1;
                }
                break;
            }
        }
    }

    private final void c() {
        ++this.n;
        switch (ag) {
            case 0: {
                this.n = 0;
                IndexColorModel indexColorModel = new IndexColorModel(24, 56746, this.s, this.q, this.s);
                this.n = 32;
                this.ad = new MemoryImageSource(ag, af, (ColorModel)indexColorModel, this.ae, 0, ag);
                break;
            }
            default: {
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.t, this.s, this.q);
                this.n = 24;
                this.ad = new MemoryImageSource(ag, af, (ColorModel)indexColorModel, this.ae, 0, ag);
                this.ad.setAnimated(true);
            }
        }
        this.ad.setAnimated(true);
        this.ad.setFullBufferUpdates(true);
        this.ah = this.createImage(this.ad);
    }

    private final void b() {
        this.t = new byte[256];
        this.s = new byte[256];
        this.q = new byte[256];
        if (this.k == 1) {
            int n = 0;
            while (n < 136) {
                byte cfr_ignored_0 = (byte)n;
                this.t[n] = (byte)this.d[n * 3];
                this.s[n] = (byte)this.d[n * 3 + 1];
                this.n = this.t[n];
                this.q[n] = (byte)this.d[n * 3 + 2];
                ++n;
            }
            return;
        }
        int n = 0;
        while (n < 256) {
            float cfr_ignored_1 = (float)Math.cos((double)n * (double)0.01f);
            this.t[n] = (byte)((double)n / 4.0 + 64.0);
            this.s[n] = (byte)((double)n / 4.0 + 64.0);
            this.q[n] = (byte)((double)n / 2.0 + 128.0);
            ++n;
        }
        this.t[255] = -21;
        this.s[255] = -21;
        this.q[255] = -1;
        int n2 = 0;
        while (n2 < 20) {
            this.n = n2;
            byte cfr_ignored_2 = (byte)n2;
            this.t[n2] = 0;
            this.s[n2] = 0;
            this.q[n2] = 0;
            ++n2;
        }
    }

    private final void a() {
        int n = 0;
        while (n < 180) {
            float cfr_ignored_0 = (float)Math.cos((double)n / 180.0);
            ++n;
        }
    }

    public Vectorball() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
        this.m = 1;
        int[] nArray = new int[]{123, 564, 89, 64, 679, 46, 41, 87};
        int[] nArray2 = new int[408];
        nArray2[0] = 255;
        nArray2[1] = 255;
        nArray2[2] = 255;
        nArray2[3] = 255;
        nArray2[4] = 222;
        nArray2[5] = 247;
        nArray2[6] = 255;
        nArray2[7] = 214;
        nArray2[8] = 231;
        nArray2[9] = 247;
        nArray2[10] = 181;
        nArray2[11] = 222;
        nArray2[12] = 239;
        nArray2[13] = 198;
        nArray2[14] = 222;
        nArray2[15] = 239;
        nArray2[16] = 165;
        nArray2[17] = 206;
        nArray2[18] = 231;
        nArray2[19] = 181;
        nArray2[20] = 206;
        nArray2[21] = 231;
        nArray2[22] = 148;
        nArray2[23] = 206;
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
        this.d = nArray2;
    }

    static {
        byte[] byArray = new byte[413];
        byArray[3] = -128;
        byArray[5] = 64;
        byArray[17] = 1;
        byArray[21] = 8;
        byArray[22] = 64;
        byArray[24] = 1;
        byArray[29] = -128;
        byArray[33] = 8;
        byArray[37] = -128;
        byArray[38] = -64;
        byArray[40] = 8;
        byArray[41] = 4;
        byArray[42] = 4;
        byArray[44] = 1;
        byArray[45] = -15;
        byArray[46] = -62;
        byArray[47] = 19;
        byArray[48] = -104;
        byArray[49] = -121;
        byArray[50] = 8;
        byArray[51] = 64;
        byArray[55] = 16;
        byArray[56] = 68;
        byArray[59] = -124;
        byArray[66] = 8;
        byArray[69] = 16;
        byArray[71] = -128;
        byArray[73] = 16;
        byArray[74] = 8;
        byArray[75] = 12;
        byArray[78] = -128;
        byArray[79] = 64;
        byArray[80] = 64;
        byArray[82] = 16;
        byArray[83] = 34;
        byArray[84] = 33;
        byArray[85] = 17;
        byArray[86] = -120;
        byArray[87] = -120;
        byArray[88] = -124;
        byArray[92] = 1;
        byArray[93] = 4;
        byArray[94] = 64;
        byArray[96] = 16;
        byArray[97] = -128;
        byArray[104] = -128;
        byArray[106] = 1;
        byArray[108] = 8;
        byArray[110] = 1;
        byArray[111] = 1;
        byArray[112] = 1;
        byArray[113] = 35;
        byArray[114] = -57;
        byArray[115] = -120;
        byArray[116] = -17;
        byArray[117] = 7;
        byArray[118] = -120;
        byArray[119] = -127;
        byArray[120] = 4;
        byArray[121] = 18;
        byArray[122] = 17;
        byArray[123] = 20;
        byArray[124] = -112;
        byArray[125] = 68;
        byArray[126] = -128;
        byArray[130] = 30;
        byArray[131] = -1;
        byArray[132] = 60;
        byArray[133] = 65;
        byArray[134] = 8;
        byArray[135] = -29;
        byArray[136] = -56;
        byArray[137] = -109;
        byArray[138] = -57;
        byArray[139] = 17;
        byArray[140] = 7;
        byArray[141] = -119;
        byArray[142] = -57;
        byArray[143] = -121;
        byArray[144] = 60;
        byArray[145] = 7;
        byArray[146] = -125;
        byArray[147] = -61;
        byArray[148] = -68;
        byArray[149] = 16;
        byArray[150] = 18;
        byArray[151] = 34;
        byArray[152] = 68;
        byArray[153] = -111;
        byArray[154] = 64;
        byArray[155] = 68;
        byArray[156] = 80;
        byArray[157] = 31;
        byArray[158] = 65;
        byArray[159] = 33;
        byArray[160] = 17;
        byArray[161] = 73;
        byArray[162] = 4;
        byArray[163] = 48;
        byArray[167] = 1;
        byArray[168] = 20;
        byArray[169] = 66;
        byArray[170] = 36;
        byArray[171] = 33;
        byArray[172] = 17;
        byArray[173] = 68;
        byArray[174] = -119;
        byArray[175] = 34;
        byArray[176] = -120;
        byArray[177] = -96;
        byArray[178] = 68;
        byArray[179] = -126;
        byArray[180] = 68;
        byArray[181] = -119;
        byArray[183] = -120;
        byArray[184] = 34;
        byArray[185] = 69;
        byArray[186] = 2;
        byArray[187] = 1;
        byArray[188] = 34;
        byArray[189] = 36;
        byArray[190] = 73;
        byArray[191] = -12;
        byArray[192] = 4;
        byArray[193] = 69;
        byArray[194] = 1;
        byArray[195] = 4;
        byArray[196] = 18;
        byArray[197] = 17;
        byArray[198] = 18;
        byArray[199] = -112;
        byArray[200] = 67;
        byArray[205] = 17;
        byArray[206] = 68;
        byArray[207] = 34;
        byArray[208] = 2;
        byArray[209] = 17;
        byArray[210] = -12;
        byArray[211] = 72;
        byArray[212] = -110;
        byArray[213] = 40;
        byArray[214] = -124;
        byArray[215] = 4;
        byArray[216] = 73;
        byArray[217] = -28;
        byArray[218] = 79;
        byArray[219] = -111;
        byArray[220] = -56;
        byArray[221] = -126;
        byArray[222] = 39;
        byArray[223] = -48;
        byArray[224] = 32;
        byArray[225] = 63;
        byArray[226] = 34;
        byArray[227] = 68;
        byArray[228] = -112;
        byArray[229] = 64;
        byArray[230] = 68;
        byArray[231] = 80;
        byArray[232] = 16;
        byArray[233] = 65;
        byArray[234] = 33;
        byArray[235] = 17;
        byArray[236] = 41;
        byArray[237] = 4;
        byArray[238] = 72;
        byArray[242] = 1;
        byArray[243] = 20;
        byArray[244] = 66;
        byArray[245] = 32;
        byArray[246] = 66;
        byArray[247] = 16;
        byArray[248] = 68;
        byArray[249] = -119;
        byArray[250] = 34;
        byArray[251] = -120;
        byArray[252] = 64;
        byArray[253] = 68;
        byArray[254] = -94;
        byArray[255] = 68;
        byArray[256] = -127;
        byArray[258] = -120;
        byArray[259] = 34;
        byArray[260] = 65;
        byArray[261] = 4;
        byArray[262] = 2;
        byArray[263] = 18;
        byArray[264] = 36;
        byArray[265] = 73;
        byArray[266] = 20;
        byArray[267] = 4;
        byArray[268] = 66;
        byArray[269] = 1;
        byArray[270] = 2;
        byArray[271] = 34;
        byArray[272] = 17;
        byArray[273] = 17;
        byArray[274] = -120;
        byArray[275] = -120;
        byArray[276] = 64;
        byArray[280] = 17;
        byArray[281] = 68;
        byArray[282] = 34;
        byArray[283] = 68;
        byArray[284] = 33;
        byArray[285] = 20;
        byArray[286] = 72;
        byArray[287] = -110;
        byArray[288] = 40;
        byArray[289] = -118;
        byArray[290] = 36;
        byArray[291] = 74;
        byArray[292] = 36;
        byArray[293] = 72;
        byArray[294] = -112;
        byArray[295] = 8;
        byArray[296] = -110;
        byArray[297] = 36;
        byArray[298] = 80;
        byArray[299] = 64;
        byArray[300] = 33;
        byArray[301] = 60;
        byArray[302] = 120;
        byArray[303] = -114;
        byArray[304] = 48;
        byArray[305] = 120;
        byArray[306] = 32;
        byArray[307] = 31;
        byArray[308] = 28;
        byArray[309] = 30;
        byArray[310] = 57;
        byArray[311] = 24;
        byArray[312] = 112;
        byArray[313] = -124;
        byArray[317] = 1;
        byArray[318] = 19;
        byArray[319] = 51;
        byArray[320] = -60;
        byArray[321] = -124;
        byArray[322] = 14;
        byArray[323] = 60;
        byArray[324] = 121;
        byArray[325] = 34;
        byArray[326] = 113;
        byArray[327] = 18;
        byArray[328] = 120;
        byArray[329] = -98;
        byArray[330] = 68;
        byArray[331] = 112;
        byArray[332] = -64;
        byArray[333] = 121;
        byArray[334] = 34;
        byArray[335] = 56;
        byArray[336] = -56;
        byArray[338] = 2;
        byArray[339] = 4;
        byArray[343] = 2;
        byArray[346] = 64;
        byArray[357] = 32;
        byArray[358] = 8;
        byArray[359] = 64;
        byArray[361] = 64;
        byArray[365] = 4;
        byArray[374] = -128;
        byArray[376] = 32;
        byArray[377] = 64;
        byArray[381] = 64;
        byArray[383] = 3;
        byArray[394] = 2;
        byArray[398] = 4;
        byArray[403] = 64;
        e = byArray;
    }
}

