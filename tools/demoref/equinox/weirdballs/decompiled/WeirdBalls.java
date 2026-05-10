/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;

public class WeirdBalls
extends Applet
implements Runnable {
    public Thread ag;
    public Image af;
    public static int ae;
    public static int ad;
    public byte[] ac;
    public byte[] ab;
    public int[] aa;
    public MemoryImageSource z;
    public a w;
    public static byte[] v;
    public int u;
    public long t;
    public long s;
    public long q;
    public long p;
    public int o;
    public byte[] n;
    public byte[] m;
    public byte[] l;
    public int k;
    public long j;
    public byte[] i;
    public int h;
    public int f;
    public int[] e;
    public int[] d;

    public final void init() {
        int n;
        int n2;
        this.w = new a();
        this.f = 1;
        switch (this.f) {
            case 0: {
                break;
            }
            case 1: {
                ae = 300;
                this.u = 400;
                int[] cfr_ignored_0 = new int[1350];
                break;
            }
        }
        v = new byte[1350];
        ad = 300;
        this.b();
        int n3 = 0;
        while (n3 < 10) {
            ++this.h;
            n2 = 0;
            while (n2 < 10) {
                n = 0;
                while (n < 10) {
                    this.ac[n3 + n2 * 10 + n * 10] = (byte)(n3 - n2 + n);
                    ++n;
                }
                ++n2;
            }
            ++n3;
        }
        this.getToolkit().getScreenSize();
        n2 = ad * ae;
        this.i = new byte[ad * ae];
        n = 0;
        while (n < n2) {
            this.i[n] = -128;
            ++n;
        }
        this.ag = null;
        this.setLayout(null);
        this.ab = new byte[ae * ad];
        this.f();
        this.c();
        this.e();
        this.w.t(this.u, ae, ad);
        this.h = 2;
        this.z.newPixels(0, 0, ae, ad, true);
        this.q = 1L;
        this.j = 0L;
        this.p = -1L;
    }

    public final void start() {
        if (this.ag == null) {
            this.ag = new Thread(this);
        }
        this.ag.start();
    }

    public final void stop() {
        if (this.ag != null) {
            this.ag = null;
        }
    }

    public final void run() {
        while (this.ag != null) {
            try {
                Thread.sleep(this.q);
            }
            catch (InterruptedException interruptedException) {}
            this.g(this.ac, this.ab);
            this.w.s();
            this.w.r(this.ac, ae, ad, v);
            this.a();
            this.z.newPixels(0, 0, ae, ad, true);
            this.repaint();
        }
    }

    private final void h() {
        ++this.j;
        this.s = System.currentTimeMillis();
        if (this.s - this.t > 1000L) {
            this.j = (int)(1000L * this.j / (this.s - this.t));
            this.showStatus(String.valueOf(this.j) + " fps");
            this.t = this.s;
            this.p = -this.p;
            if (this.o >= 1 && this.p < 0L && this.o < 12) {
                if (this.j <= 10L) {
                    this.q += -4L;
                }
                if (this.j <= 20L && this.j > 10L) {
                    this.q += -3L;
                    this.h = 0;
                }
                if (this.j <= 30L && this.j > 20L) {
                    this.q += -2L;
                }
                if (this.j <= 60L && this.j >= 30L) {
                    this.q += -1L;
                }
                if (this.j > 80L) {
                    ++this.q;
                }
                if (this.q > 14L) {
                    this.q = 14L;
                }
            }
            if (this.o == 14) {
                if (this.j > 80L) {
                    ++this.q;
                }
                if (this.j < 70L) {
                    this.q += -1L;
                }
            }
            if (this.q < 0L) {
                this.q = 0L;
            }
            ++this.o;
            this.j = 0L;
        }
    }

    public final void update(Graphics graphics) {
        this.h = 1;
        graphics.drawImage(this.af, 0, 0, ae, ad, this);
        this.h();
    }

    private final void g(byte[] byArray, byte[] byArray2) {
        boolean bl = false;
        int n = 0;
        while (n < ae * ad) {
            byArray2[n] = byArray[this.aa[n]];
            ++n;
        }
        System.arraycopy(byArray2, 0, byArray, 0, ae * ad);
    }

    private final void f() {
        this.aa = new int[ae * ad];
        int n = 0;
        float f = -80.0f;
        float f2 = 0.07853975f;
        f2 = 0.0f;
        float f3 = (float)(1.0 / (400.0 - (double)f));
        int n2 = -ad / 2;
        while (n2 < ad / 2) {
            int n3 = -ae / 2;
            while (n3 < ae / 2) {
                int n4 = (int)(400.0 * (double)n3 * (double)f3);
                int n5 = (int)(400.0 * (double)n2 * (double)f3);
                int n6 = (int)((double)n4 * Math.cos(f2) - (double)n5 * Math.sin(f2));
                int n7 = (int)((double)n4 * Math.sin(f2) + (double)n5 * Math.cos(f2));
                this.aa[n] = n6 + ae / 2 + ae * (n7 + ad / 2);
                float cfr_ignored_3 = this.aa[n];
                ++n;
                ++n3;
            }
            ++n2;
        }
    }

    private final void e() {
        ++this.h;
        switch (ae) {
            case 0: {
                this.h = 0;
                IndexColorModel indexColorModel = new IndexColorModel(24, 56746, this.m, this.l, this.m);
                this.h = 32;
                this.z = new MemoryImageSource(ae, ad, (ColorModel)indexColorModel, this.ac, 0, ae);
                break;
            }
            case 300: {
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.n, this.m, this.l);
                this.h = 24;
                this.z = new MemoryImageSource(ae, ad, (ColorModel)indexColorModel, this.ac, 0, ae);
                this.z.setAnimated(true);
                break;
            }
        }
        this.z.setFullBufferUpdates(true);
        this.af = this.createImage(this.z);
    }

    private final void d() {
        this.n = new byte[256];
        this.m = new byte[256];
        this.l = new byte[256];
        int n = 0;
        while (n < 136) {
            this.n[n] = (byte)this.d[n * 3];
            this.m[n] = (byte)this.d[n * 3 + 1];
            ++this.h;
            this.l[n] = (byte)this.d[n * 3 + 2];
            ++n;
        }
    }

    private final void c() {
        int n = 0;
        while (n < 180) {
            float cfr_ignored_0 = (float)Math.cos((double)n / 180.0);
            ++n;
        }
    }

    private final void b() {
        this.ac = new byte[ae * ad];
        int n = 0;
        while (n < ae * ad) {
            this.ac[n] = -128;
            ++n;
        }
        this.d();
    }

    private final void a() {
        this.k = 0;
        while (this.k < 3300) {
            if (this.e[this.k] != 0) {
                this.ac[this.k + 600] = 0;
            }
            ++this.k;
        }
    }

    public WeirdBalls() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
        this.f = 1;
        int[] nArray = new int[3300];
        nArray[24] = 1;
        nArray[41] = 1;
        nArray[143] = 1;
        nArray[172] = 1;
        nArray[177] = 1;
        nArray[199] = 1;
        nArray[232] = 1;
        nArray[268] = 1;
        nArray[296] = 1;
        nArray[304] = 1;
        nArray[305] = 1;
        nArray[324] = 1;
        nArray[333] = 1;
        nArray[341] = 1;
        nArray[359] = 1;
        nArray[360] = 1;
        nArray[361] = 1;
        nArray[362] = 1;
        nArray[363] = 1;
        nArray[367] = 1;
        nArray[368] = 1;
        nArray[369] = 1;
        nArray[374] = 1;
        nArray[379] = 1;
        nArray[382] = 1;
        nArray[383] = 1;
        nArray[384] = 1;
        nArray[387] = 1;
        nArray[388] = 1;
        nArray[392] = 1;
        nArray[397] = 1;
        nArray[398] = 1;
        nArray[399] = 1;
        nArray[404] = 1;
        nArray[409] = 1;
        nArray[443] = 1;
        nArray[449] = 1;
        nArray[453] = 1;
        nArray[472] = 1;
        nArray[477] = 1;
        nArray[532] = 1;
        nArray[555] = 1;
        nArray[568] = 1;
        nArray[587] = 1;
        nArray[596] = 1;
        nArray[604] = 1;
        nArray[605] = 1;
        nArray[624] = 1;
        nArray[633] = 1;
        nArray[641] = 1;
        nArray[659] = 1;
        nArray[666] = 1;
        nArray[670] = 1;
        nArray[674] = 1;
        nArray[679] = 1;
        nArray[683] = 1;
        nArray[687] = 1;
        nArray[688] = 1;
        nArray[692] = 1;
        nArray[696] = 1;
        nArray[700] = 1;
        nArray[704] = 1;
        nArray[709] = 1;
        nArray[743] = 1;
        nArray[749] = 1;
        nArray[753] = 1;
        nArray[771] = 1;
        nArray[776] = 1;
        nArray[832] = 1;
        nArray[855] = 1;
        nArray[868] = 1;
        nArray[887] = 1;
        nArray[895] = 1;
        nArray[903] = 1;
        nArray[906] = 1;
        nArray[910] = 1;
        nArray[911] = 1;
        nArray[912] = 1;
        nArray[913] = 1;
        nArray[917] = 1;
        nArray[918] = 1;
        nArray[919] = 1;
        nArray[920] = 1;
        nArray[924] = 1;
        nArray[928] = 1;
        nArray[929] = 1;
        nArray[930] = 1;
        nArray[932] = 1;
        nArray[933] = 1;
        nArray[934] = 1;
        nArray[935] = 1;
        nArray[941] = 1;
        nArray[942] = 1;
        nArray[943] = 1;
        nArray[944] = 1;
        nArray[948] = 1;
        nArray[952] = 1;
        nArray[959] = 1;
        nArray[965] = 1;
        nArray[971] = 1;
        nArray[974] = 1;
        nArray[979] = 1;
        nArray[983] = 1;
        nArray[987] = 1;
        nArray[989] = 1;
        nArray[992] = 1;
        nArray[995] = 1;
        nArray[1001] = 1;
        nArray[1005] = 1;
        nArray[1008] = 1;
        nArray[1043] = 1;
        nArray[1044] = 1;
        nArray[1045] = 1;
        nArray[1046] = 1;
        nArray[1048] = 1;
        nArray[1049] = 1;
        nArray[1050] = 1;
        nArray[1051] = 1;
        nArray[1052] = 1;
        nArray[1053] = 1;
        nArray[1054] = 1;
        nArray[1055] = 1;
        nArray[1058] = 1;
        nArray[1059] = 1;
        nArray[1060] = 1;
        nArray[1061] = 1;
        nArray[1065] = 1;
        nArray[1071] = 1;
        nArray[1076] = 1;
        nArray[1080] = 1;
        nArray[1081] = 1;
        nArray[1082] = 1;
        nArray[1086] = 1;
        nArray[1087] = 1;
        nArray[1088] = 1;
        nArray[1089] = 1;
        nArray[1092] = 1;
        nArray[1096] = 1;
        nArray[1099] = 1;
        nArray[1102] = 1;
        nArray[1103] = 1;
        nArray[1104] = 1;
        nArray[1105] = 1;
        nArray[1109] = 1;
        nArray[1110] = 1;
        nArray[1111] = 1;
        nArray[1115] = 1;
        nArray[1119] = 1;
        nArray[1125] = 1;
        nArray[1126] = 1;
        nArray[1127] = 1;
        nArray[1128] = 1;
        nArray[1132] = 1;
        nArray[1135] = 1;
        nArray[1136] = 1;
        nArray[1137] = 1;
        nArray[1141] = 1;
        nArray[1142] = 1;
        nArray[1143] = 1;
        nArray[1144] = 1;
        nArray[1149] = 1;
        nArray[1150] = 1;
        nArray[1151] = 1;
        nArray[1154] = 1;
        nArray[1155] = 1;
        nArray[1156] = 1;
        nArray[1157] = 1;
        nArray[1165] = 1;
        nArray[1166] = 1;
        nArray[1167] = 1;
        nArray[1168] = 1;
        nArray[1174] = 1;
        nArray[1175] = 1;
        nArray[1176] = 1;
        nArray[1177] = 1;
        nArray[1182] = 1;
        nArray[1183] = 1;
        nArray[1184] = 1;
        nArray[1186] = 1;
        nArray[1187] = 1;
        nArray[1188] = 1;
        nArray[1189] = 1;
        nArray[1195] = 1;
        nArray[1203] = 1;
        nArray[1206] = 1;
        nArray[1210] = 1;
        nArray[1214] = 1;
        nArray[1217] = 1;
        nArray[1221] = 1;
        nArray[1224] = 1;
        nArray[1227] = 1;
        nArray[1231] = 1;
        nArray[1233] = 1;
        nArray[1241] = 1;
        nArray[1245] = 1;
        nArray[1249] = 1;
        nArray[1251] = 1;
        nArray[1259] = 1;
        nArray[1260] = 1;
        nArray[1261] = 1;
        nArray[1262] = 1;
        nArray[1263] = 1;
        nArray[1265] = 1;
        nArray[1271] = 1;
        nArray[1274] = 1;
        nArray[1279] = 1;
        nArray[1283] = 1;
        nArray[1287] = 1;
        nArray[1289] = 1;
        nArray[1292] = 1;
        nArray[1295] = 1;
        nArray[1301] = 1;
        nArray[1306] = 1;
        nArray[1307] = 1;
        nArray[1343] = 1;
        nArray[1347] = 1;
        nArray[1349] = 1;
        nArray[1353] = 1;
        nArray[1358] = 1;
        nArray[1362] = 1;
        nArray[1365] = 1;
        nArray[1370] = 1;
        nArray[1375] = 1;
        nArray[1379] = 1;
        nArray[1383] = 1;
        nArray[1385] = 1;
        nArray[1389] = 1;
        nArray[1392] = 1;
        nArray[1396] = 1;
        nArray[1399] = 1;
        nArray[1402] = 1;
        nArray[1406] = 1;
        nArray[1408] = 1;
        nArray[1412] = 1;
        nArray[1416] = 1;
        nArray[1418] = 1;
        nArray[1425] = 1;
        nArray[1429] = 1;
        nArray[1432] = 1;
        nArray[1438] = 1;
        nArray[1441] = 1;
        nArray[1445] = 1;
        nArray[1448] = 1;
        nArray[1452] = 1;
        nArray[1455] = 1;
        nArray[1464] = 1;
        nArray[1468] = 1;
        nArray[1474] = 1;
        nArray[1478] = 1;
        nArray[1481] = 1;
        nArray[1485] = 1;
        nArray[1487] = 1;
        nArray[1494] = 1;
        nArray[1503] = 1;
        nArray[1506] = 1;
        nArray[1510] = 1;
        nArray[1514] = 1;
        nArray[1517] = 1;
        nArray[1521] = 1;
        nArray[1524] = 1;
        nArray[1527] = 1;
        nArray[1528] = 1;
        nArray[1529] = 1;
        nArray[1530] = 1;
        nArray[1531] = 1;
        nArray[1533] = 1;
        nArray[1541] = 1;
        nArray[1545] = 1;
        nArray[1549] = 1;
        nArray[1551] = 1;
        nArray[1559] = 1;
        nArray[1565] = 1;
        nArray[1571] = 1;
        nArray[1574] = 1;
        nArray[1579] = 1;
        nArray[1583] = 1;
        nArray[1587] = 1;
        nArray[1590] = 1;
        nArray[1592] = 1;
        nArray[1595] = 1;
        nArray[1601] = 1;
        nArray[1606] = 1;
        nArray[1607] = 1;
        nArray[1643] = 1;
        nArray[1647] = 1;
        nArray[1649] = 1;
        nArray[1653] = 1;
        nArray[1658] = 1;
        nArray[1662] = 1;
        nArray[1670] = 1;
        nArray[1675] = 1;
        nArray[1679] = 1;
        nArray[1680] = 1;
        nArray[1681] = 1;
        nArray[1682] = 1;
        nArray[1683] = 1;
        nArray[1685] = 1;
        nArray[1689] = 1;
        nArray[1692] = 1;
        nArray[1696] = 1;
        nArray[1699] = 1;
        nArray[1702] = 1;
        nArray[1706] = 1;
        nArray[1708] = 1;
        nArray[1712] = 1;
        nArray[1717] = 1;
        nArray[1725] = 1;
        nArray[1729] = 1;
        nArray[1732] = 1;
        nArray[1735] = 1;
        nArray[1736] = 1;
        nArray[1737] = 1;
        nArray[1738] = 1;
        nArray[1741] = 1;
        nArray[1745] = 1;
        nArray[1748] = 1;
        nArray[1749] = 1;
        nArray[1750] = 1;
        nArray[1751] = 1;
        nArray[1752] = 1;
        nArray[1755] = 1;
        nArray[1759] = 1;
        nArray[1760] = 1;
        nArray[1761] = 1;
        nArray[1764] = 1;
        nArray[1768] = 1;
        nArray[1774] = 1;
        nArray[1778] = 1;
        nArray[1781] = 1;
        nArray[1782] = 1;
        nArray[1783] = 1;
        nArray[1784] = 1;
        nArray[1785] = 1;
        nArray[1787] = 1;
        nArray[1794] = 1;
        nArray[1802] = 1;
        nArray[1803] = 1;
        nArray[1804] = 1;
        nArray[1805] = 1;
        nArray[1806] = 1;
        nArray[1807] = 1;
        nArray[1810] = 1;
        nArray[1814] = 1;
        nArray[1817] = 1;
        nArray[1821] = 1;
        nArray[1824] = 1;
        nArray[1827] = 1;
        nArray[1833] = 1;
        nArray[1841] = 1;
        nArray[1845] = 1;
        nArray[1849] = 1;
        nArray[1851] = 1;
        nArray[1859] = 1;
        nArray[1865] = 1;
        nArray[1871] = 1;
        nArray[1874] = 1;
        nArray[1879] = 1;
        nArray[1883] = 1;
        nArray[1887] = 1;
        nArray[1890] = 1;
        nArray[1892] = 1;
        nArray[1895] = 1;
        nArray[1901] = 1;
        nArray[1905] = 1;
        nArray[1908] = 1;
        nArray[1943] = 1;
        nArray[1947] = 1;
        nArray[1949] = 1;
        nArray[1953] = 1;
        nArray[1958] = 1;
        nArray[1962] = 1;
        nArray[1969] = 1;
        nArray[1974] = 1;
        nArray[1979] = 1;
        nArray[1985] = 1;
        nArray[1989] = 1;
        nArray[1992] = 1;
        nArray[1996] = 1;
        nArray[1999] = 1;
        nArray[2002] = 1;
        nArray[2006] = 1;
        nArray[2008] = 1;
        nArray[2012] = 1;
        nArray[2017] = 1;
        nArray[2025] = 1;
        nArray[2029] = 1;
        nArray[2032] = 1;
        nArray[2034] = 1;
        nArray[2038] = 1;
        nArray[2041] = 1;
        nArray[2045] = 1;
        nArray[2048] = 1;
        nArray[2055] = 1;
        nArray[2064] = 1;
        nArray[2068] = 1;
        nArray[2074] = 1;
        nArray[2078] = 1;
        nArray[2081] = 1;
        nArray[2087] = 1;
        nArray[2093] = 1;
        nArray[2102] = 1;
        nArray[2107] = 1;
        nArray[2110] = 1;
        nArray[2114] = 1;
        nArray[2117] = 1;
        nArray[2121] = 1;
        nArray[2124] = 1;
        nArray[2127] = 1;
        nArray[2131] = 1;
        nArray[2133] = 1;
        nArray[2141] = 1;
        nArray[2145] = 1;
        nArray[2150] = 1;
        nArray[2159] = 1;
        nArray[2166] = 1;
        nArray[2170] = 1;
        nArray[2174] = 1;
        nArray[2179] = 1;
        nArray[2183] = 1;
        nArray[2187] = 1;
        nArray[2191] = 1;
        nArray[2192] = 1;
        nArray[2196] = 1;
        nArray[2200] = 1;
        nArray[2204] = 1;
        nArray[2209] = 1;
        nArray[2243] = 1;
        nArray[2247] = 1;
        nArray[2249] = 1;
        nArray[2253] = 1;
        nArray[2258] = 1;
        nArray[2262] = 1;
        nArray[2265] = 1;
        nArray[2269] = 1;
        nArray[2274] = 1;
        nArray[2279] = 1;
        nArray[2283] = 1;
        nArray[2285] = 1;
        nArray[2289] = 1;
        nArray[2292] = 1;
        nArray[2296] = 1;
        nArray[2299] = 1;
        nArray[2302] = 1;
        nArray[2306] = 1;
        nArray[2308] = 1;
        nArray[2312] = 1;
        nArray[2316] = 1;
        nArray[2318] = 1;
        nArray[2322] = 1;
        nArray[2325] = 1;
        nArray[2329] = 1;
        nArray[2332] = 1;
        nArray[2334] = 1;
        nArray[2338] = 1;
        nArray[2341] = 1;
        nArray[2345] = 1;
        nArray[2348] = 1;
        nArray[2352] = 1;
        nArray[2355] = 1;
        nArray[2364] = 1;
        nArray[2368] = 1;
        nArray[2371] = 1;
        nArray[2374] = 1;
        nArray[2378] = 1;
        nArray[2381] = 1;
        nArray[2385] = 1;
        nArray[2387] = 1;
        nArray[2393] = 1;
        nArray[2402] = 1;
        nArray[2407] = 1;
        nArray[2410] = 1;
        nArray[2411] = 1;
        nArray[2412] = 1;
        nArray[2413] = 1;
        nArray[2417] = 1;
        nArray[2418] = 1;
        nArray[2419] = 1;
        nArray[2420] = 1;
        nArray[2424] = 1;
        nArray[2428] = 1;
        nArray[2429] = 1;
        nArray[2430] = 1;
        nArray[2434] = 1;
        nArray[2435] = 1;
        nArray[2441] = 1;
        nArray[2442] = 1;
        nArray[2443] = 1;
        nArray[2444] = 1;
        nArray[2450] = 1;
        nArray[2459] = 1;
        nArray[2460] = 1;
        nArray[2461] = 1;
        nArray[2462] = 1;
        nArray[2463] = 1;
        nArray[2467] = 1;
        nArray[2468] = 1;
        nArray[2469] = 1;
        nArray[2475] = 1;
        nArray[2476] = 1;
        nArray[2477] = 1;
        nArray[2478] = 1;
        nArray[2482] = 1;
        nArray[2483] = 1;
        nArray[2484] = 1;
        nArray[2487] = 1;
        nArray[2491] = 1;
        nArray[2492] = 1;
        nArray[2497] = 1;
        nArray[2498] = 1;
        nArray[2499] = 1;
        nArray[2504] = 1;
        nArray[2509] = 1;
        nArray[2543] = 1;
        nArray[2547] = 1;
        nArray[2550] = 1;
        nArray[2551] = 1;
        nArray[2554] = 1;
        nArray[2555] = 1;
        nArray[2558] = 1;
        nArray[2559] = 1;
        nArray[2560] = 1;
        nArray[2561] = 1;
        nArray[2565] = 1;
        nArray[2568] = 1;
        nArray[2573] = 1;
        nArray[2580] = 1;
        nArray[2581] = 1;
        nArray[2582] = 1;
        nArray[2586] = 1;
        nArray[2587] = 1;
        nArray[2588] = 1;
        nArray[2589] = 1;
        nArray[2593] = 1;
        nArray[2594] = 1;
        nArray[2595] = 1;
        nArray[2596] = 1;
        nArray[2599] = 1;
        nArray[2602] = 1;
        nArray[2606] = 1;
        nArray[2609] = 1;
        nArray[2610] = 1;
        nArray[2611] = 1;
        nArray[2615] = 1;
        nArray[2619] = 1;
        nArray[2622] = 1;
        nArray[2625] = 1;
        nArray[2626] = 1;
        nArray[2627] = 1;
        nArray[2628] = 1;
        nArray[2632] = 1;
        nArray[2635] = 1;
        nArray[2636] = 1;
        nArray[2637] = 1;
        nArray[2638] = 1;
        nArray[2641] = 1;
        nArray[2645] = 1;
        nArray[2649] = 1;
        nArray[2650] = 1;
        nArray[2651] = 1;
        nArray[2656] = 1;
        nArray[2657] = 1;
        nArray[2665] = 1;
        nArray[2666] = 1;
        nArray[2667] = 1;
        nArray[2668] = 1;
        nArray[2671] = 1;
        nArray[2674] = 1;
        nArray[2678] = 1;
        nArray[2682] = 1;
        nArray[2683] = 1;
        nArray[2684] = 1;
        nArray[2688] = 1;
        nArray[2689] = 1;
        nArray[2692] = 1;
        nArray[2710] = 1;
        nArray[2717] = 1;
        nArray[2750] = 1;
        nArray[2769] = 1;
        nArray[2858] = 1;
        nArray[2868] = 1;
        nArray[2873] = 1;
        nArray[2889] = 1;
        nArray[2925] = 1;
        nArray[2992] = 1;
        nArray[3010] = 1;
        nArray[3017] = 1;
        nArray[3049] = 1;
        nArray[3070] = 1;
        nArray[3071] = 1;
        nArray[3158] = 1;
        nArray[3189] = 1;
        nArray[3225] = 1;
        this.e = nArray;
        int[] nArray2 = new int[]{123, 564, 89, 64, 679, 46, 41, 87};
        int[] nArray3 = new int[408];
        nArray3[1] = 255;
        nArray3[2] = 255;
        nArray3[4] = 222;
        nArray3[5] = 247;
        nArray3[7] = 214;
        nArray3[8] = 231;
        nArray3[10] = 181;
        nArray3[11] = 222;
        nArray3[13] = 198;
        nArray3[14] = 222;
        nArray3[16] = 165;
        nArray3[17] = 206;
        nArray3[19] = 181;
        nArray3[20] = 206;
        nArray3[22] = 148;
        nArray3[23] = 206;
        nArray3[25] = 148;
        nArray3[26] = 189;
        nArray3[28] = 165;
        nArray3[29] = 189;
        nArray3[31] = 132;
        nArray3[32] = 189;
        nArray3[34] = 123;
        nArray3[35] = 189;
        nArray3[37] = 132;
        nArray3[38] = 181;
        nArray3[40] = 107;
        nArray3[41] = 181;
        nArray3[43] = 198;
        nArray3[44] = 198;
        nArray3[46] = 148;
        nArray3[47] = 173;
        nArray3[49] = 115;
        nArray3[50] = 173;
        nArray3[52] = 107;
        nArray3[53] = 173;
        nArray3[55] = 115;
        nArray3[56] = 165;
        nArray3[58] = 99;
        nArray3[59] = 156;
        nArray3[61] = 90;
        nArray3[62] = 165;
        nArray3[64] = 99;
        nArray3[65] = 148;
        nArray3[67] = 82;
        nArray3[68] = 140;
        nArray3[70] = 74;
        nArray3[71] = 148;
        nArray3[73] = 82;
        nArray3[74] = 140;
        nArray3[76] = 57;
        nArray3[77] = 132;
        nArray3[79] = 66;
        nArray3[80] = 123;
        nArray3[82] = 49;
        nArray3[83] = 115;
        nArray3[85] = 49;
        nArray3[86] = 115;
        nArray3[88] = 41;
        nArray3[89] = 115;
        nArray3[91] = 41;
        nArray3[92] = 115;
        nArray3[94] = 33;
        nArray3[95] = 107;
        nArray3[97] = 33;
        nArray3[98] = 107;
        nArray3[100] = 24;
        nArray3[101] = 99;
        nArray3[103] = 16;
        nArray3[104] = 90;
        nArray3[106] = 24;
        nArray3[107] = 90;
        nArray3[109] = 8;
        nArray3[110] = 82;
        nArray3[113] = 74;
        nArray3[115] = 24;
        nArray3[116] = 82;
        nArray3[118] = 8;
        nArray3[119] = 74;
        nArray3[122] = 74;
        nArray3[125] = 66;
        nArray3[128] = 57;
        nArray3[130] = 8;
        nArray3[131] = 74;
        nArray3[134] = 49;
        nArray3[136] = 16;
        nArray3[137] = 74;
        nArray3[140] = 49;
        nArray3[143] = 57;
        nArray3[146] = 41;
        nArray3[149] = 33;
        nArray3[152] = 57;
        nArray3[155] = 41;
        nArray3[158] = 33;
        nArray3[161] = 24;
        nArray3[164] = 41;
        nArray3[167] = 24;
        nArray3[170] = 16;
        nArray3[172] = 16;
        nArray3[173] = 57;
        nArray3[176] = 33;
        nArray3[179] = 8;
        nArray3[182] = 49;
        nArray3[185] = 33;
        nArray3[188] = 16;
        nArray3[192] = 255;
        nArray3[193] = 255;
        nArray3[194] = 255;
        nArray3[195] = 255;
        nArray3[196] = 222;
        nArray3[197] = 255;
        nArray3[198] = 255;
        nArray3[199] = 214;
        nArray3[200] = 255;
        nArray3[201] = 247;
        nArray3[202] = 181;
        nArray3[203] = 255;
        nArray3[204] = 239;
        nArray3[205] = 198;
        nArray3[206] = 255;
        nArray3[207] = 239;
        nArray3[208] = 165;
        nArray3[209] = 255;
        nArray3[210] = 231;
        nArray3[211] = 181;
        nArray3[212] = 255;
        nArray3[213] = 231;
        nArray3[214] = 148;
        nArray3[215] = 255;
        nArray3[216] = 222;
        nArray3[217] = 148;
        nArray3[218] = 255;
        nArray3[219] = 214;
        nArray3[220] = 165;
        nArray3[221] = 255;
        nArray3[222] = 214;
        nArray3[223] = 132;
        nArray3[224] = 255;
        nArray3[225] = 214;
        nArray3[226] = 123;
        nArray3[227] = 255;
        nArray3[228] = 206;
        nArray3[229] = 132;
        nArray3[230] = 249;
        nArray3[231] = 206;
        nArray3[232] = 107;
        nArray3[233] = 249;
        nArray3[234] = 198;
        nArray3[235] = 198;
        nArray3[236] = 255;
        nArray3[237] = 198;
        nArray3[238] = 148;
        nArray3[239] = 241;
        nArray3[240] = 198;
        nArray3[241] = 115;
        nArray3[242] = 241;
        nArray3[243] = 198;
        nArray3[244] = 107;
        nArray3[245] = 241;
        nArray3[246] = 189;
        nArray3[247] = 115;
        nArray3[248] = 233;
        nArray3[249] = 189;
        nArray3[250] = 99;
        nArray3[251] = 224;
        nArray3[252] = 189;
        nArray3[253] = 90;
        nArray3[254] = 233;
        nArray3[255] = 173;
        nArray3[256] = 99;
        nArray3[257] = 216;
        nArray3[258] = 173;
        nArray3[259] = 82;
        nArray3[260] = 208;
        nArray3[261] = 173;
        nArray3[262] = 74;
        nArray3[263] = 216;
        nArray3[264] = 165;
        nArray3[265] = 82;
        nArray3[266] = 208;
        nArray3[267] = 156;
        nArray3[268] = 57;
        nArray3[269] = 200;
        nArray3[270] = 148;
        nArray3[271] = 66;
        nArray3[272] = 191;
        nArray3[273] = 148;
        nArray3[274] = 49;
        nArray3[275] = 183;
        nArray3[276] = 140;
        nArray3[277] = 49;
        nArray3[278] = 183;
        nArray3[279] = 140;
        nArray3[280] = 41;
        nArray3[281] = 183;
        nArray3[282] = 132;
        nArray3[283] = 41;
        nArray3[284] = 183;
        nArray3[285] = 132;
        nArray3[286] = 33;
        nArray3[287] = 175;
        nArray3[288] = 123;
        nArray3[289] = 33;
        nArray3[290] = 175;
        nArray3[291] = 115;
        nArray3[292] = 24;
        nArray3[293] = 167;
        nArray3[294] = 115;
        nArray3[295] = 16;
        nArray3[296] = 158;
        nArray3[297] = 107;
        nArray3[298] = 24;
        nArray3[299] = 158;
        nArray3[300] = 99;
        nArray3[301] = 8;
        nArray3[302] = 150;
        nArray3[303] = 99;
        nArray3[305] = 142;
        nArray3[306] = 90;
        nArray3[307] = 24;
        nArray3[308] = 150;
        nArray3[309] = 90;
        nArray3[310] = 8;
        nArray3[311] = 142;
        nArray3[312] = 90;
        nArray3[314] = 142;
        nArray3[315] = 90;
        nArray3[317] = 134;
        nArray3[318] = 82;
        nArray3[320] = 125;
        nArray3[321] = 74;
        nArray3[322] = 8;
        nArray3[323] = 142;
        nArray3[324] = 74;
        nArray3[326] = 117;
        nArray3[327] = 66;
        nArray3[328] = 16;
        nArray3[329] = 142;
        nArray3[330] = 66;
        nArray3[332] = 117;
        nArray3[333] = 57;
        nArray3[335] = 125;
        nArray3[336] = 57;
        nArray3[338] = 109;
        nArray3[339] = 57;
        nArray3[341] = 101;
        nArray3[342] = 49;
        nArray3[344] = 125;
        nArray3[345] = 49;
        nArray3[347] = 109;
        nArray3[348] = 49;
        nArray3[350] = 101;
        nArray3[351] = 41;
        nArray3[353] = 92;
        nArray3[354] = 33;
        nArray3[356] = 109;
        nArray3[357] = 33;
        nArray3[359] = 92;
        nArray3[360] = 33;
        nArray3[362] = 84;
        nArray3[363] = 16;
        nArray3[364] = 16;
        nArray3[365] = 125;
        nArray3[366] = 16;
        nArray3[368] = 101;
        nArray3[369] = 16;
        nArray3[371] = 76;
        nArray3[374] = 117;
        nArray3[377] = 101;
        nArray3[380] = 84;
        nArray3[383] = 68;
        nArray3[387] = 34;
        nArray3[388] = 17;
        nArray3[390] = 68;
        nArray3[391] = 34;
        nArray3[393] = 102;
        nArray3[394] = 51;
        nArray3[396] = 135;
        nArray3[397] = 68;
        nArray3[399] = 170;
        nArray3[400] = 85;
        nArray3[402] = 204;
        nArray3[403] = 102;
        nArray3[405] = 238;
        nArray3[406] = 119;
        this.d = nArray3;
    }
}

