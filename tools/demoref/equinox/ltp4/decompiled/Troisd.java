/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  sun.audio.AudioData
 *  sun.audio.AudioPlayer
 *  sun.audio.ContinuousAudioDataStream
 */
import b.c.b;
import b.e.a;
import java.applet.Applet;
import java.applet.AudioClip;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.Toolkit;
import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;
import java.awt.image.PixelGrabber;
import java.io.InputStream;
import java.net.URL;
import sun.audio.AudioData;
import sun.audio.AudioPlayer;
import sun.audio.ContinuousAudioDataStream;

public class Troisd
extends Applet
implements Runnable {
    int a;
    public static final int b = 280;
    public static final int c = 280;
    public static final int d = 280;
    public static final int e = 280;
    public static final int f = 16;
    private static final int g = 30;
    private static final int h = 20;
    private static final int i = 14;
    private static final int j = 4;
    private static final int k = 14;
    private static final int l = 14;
    private static final int m = 28;
    private static final int n = 2;
    private static final int o = 0;
    private static final int p = 4;
    private static final int q = 9;
    public static final int r = 1000;
    public static final int s = 80;
    private static final int t = 1000;
    private static final int u = 1500;
    private static final int v = 3000;
    private static final int w = 3000;
    private static final int x = 1500;
    private static final int y = 1500;
    private static final int z = 2200;
    private static final int A = 1000;
    private static final int B = 1000;
    private static int C = 0;
    private URL D;
    private int E;
    private String F;
    private int G;
    private int H;
    private int I;
    private float J;
    private float K;
    private float L;
    private int M;
    private int N;
    private int O;
    private int P;
    private int Q;
    private int R;
    private int S;
    private int T;
    private int U;
    private int V;
    private int W;
    private int X;
    private int Y;
    private int Z;
    private int ba;
    private int bb;
    private int bc;
    private int bd;
    private int be;
    private int bf;
    private int bg;
    private int bh;
    private InputStream bi;
    private byte[] bj;
    private byte[] bk;
    public byte[] bl;
    public byte[] bm;
    private boolean bn = false;
    private boolean bo = false;
    public static final int bp = 4;
    private int bq;
    public final int br = 0;
    public final int bs = 8;
    private float bt;
    private byte[] bu;
    private byte[] bv;
    private byte[] bw;
    private byte[] bx;
    private byte[] by;
    private byte[] bz;
    private boolean bA = false;
    Thread bB;
    public Image bC;
    public URL bD;
    private byte[] bE;
    public MemoryImageSource bF;
    private b.f.a bG;
    private b.f.b bH;
    private b.g.a bI;
    private b.c.a bJ;
    private b bK;
    private f bL;
    private g bM;
    private b.a.a bN;
    private b.d.a bO;
    public j bP;
    private b.b.a bQ;
    private b.b.b bR;
    private l bS;
    private e bT;
    private i bU;
    private a.a bV;
    private int bW;
    private long bX;
    private long bY;
    private long bZ;
    private int ca = 0;
    private byte[] cb;
    private byte[] cc;
    private byte[] cd;
    private byte[] ce;
    public a cf;
    public a cg;
    public a ch;
    public a ci;
    public a cj;
    private int[] ck;
    private int[] cl;
    private int[] cm;
    private int[] cn;
    private int[] co;
    private int[] cp;
    private float[] cq;
    private float[] cr;
    private int[] cs;
    private int[] ct;
    private int[] cu;
    private int[] cv;
    private int[] cw;
    private int[] cx;
    private byte[][] cy;
    private int cz;
    private int cA;
    private int cB;
    private int cC;
    private int cD;
    private int cE;
    private int cF;
    private int cG;
    private int cH;
    private int cI;
    private int cJ;
    private int cK;
    private int cL;
    private int cM;
    private int cN;
    private int cO;
    private int cP;
    private int cQ;
    private int cR;
    private int cS;
    private int cT;
    private int cU;
    private int cV;
    private int cW;
    private int[] cX;
    private int[] cY;
    private int[] cZ;
    private int[] da;
    private int[] db;
    private int[] dc;
    private float[] dd;
    public AudioClip de;
    public int[] df;
    private int dg;
    private float dh;
    private float di;
    private float dj;
    public int dk;
    public int dl;
    public int dm;
    public float dn;
    public float dp;
    public float dq;
    public float dr;
    public float ds;
    public float dt;
    public float du;
    public float dv;
    public float dw;
    public float dx;
    public float dy;
    public float dz;
    public float dA;
    public float dB;
    public float dC;
    public float dD;
    public float dE;
    public float dF;
    public int dG;
    public int dH;
    public int dI;
    public int dJ;
    public int dK;
    public int dL;
    public int dM;
    public int dN;
    public int dO;
    public int dP;
    public int dQ;
    public int dR;
    public int dS;
    public int dT;
    public int dU;
    public int dV;
    public int dW;
    public int dX;
    private int dY;
    private int dZ;
    private int ea;
    private int eb;
    private int ec;
    private int ed;
    private int ee;
    private int ef;
    private int eg;
    private int eh;
    private int ei;
    private int ej;
    private int ek;
    private int el;
    private byte[] em = new byte[546];
    private int en;
    private float eo;
    private int ep;
    private int eq;
    private int er;
    private int es;
    private int et;
    private int eu;
    private int ev;
    private int ew;
    private int ex;
    private int ey;
    private int ez;
    private int eA;
    private int eB;
    private int eC;
    private int eD;
    private int eE;
    private int eF;
    private int eG;
    private int eH;
    private int eI;
    private int eJ;
    private int eK;
    private int eL;
    private int eM;
    private int eN;
    private int eO;
    private int eP;
    private int eQ;
    private int eR;
    private int eS;
    private int eT;
    private int eU;
    private int eV;
    private int eW;
    private int eX;
    private int eY;
    private int eZ;
    private int fa;
    private int fb;
    private int fc;
    private int fd;
    private int fe;
    private String ff;
    private int fg;
    private int fh;
    private float fi;
    private double fj;
    private String fk = "?";
    private String fl;
    private int[] fm = new int[20];
    private double[][] fn = new double[4][10];
    private byte fo;
    private Thread fp;
    private boolean fq;
    private MemoryImageSource fr;
    private Image fs;
    private int ft = 1;
    private int fu;
    private int fv;
    private float fw;
    private float fx;
    private float fy;
    private float fz;
    private float fA;
    private float fB;
    private int[] fC = new int[505];
    private int fD = 0;
    private byte[] fE = new byte[280];
    private int fF = 0;
    private byte[] fG = new byte[29830];
    private byte[] fH = new byte[20574];
    private byte[] fI = new byte[259200];
    private int fJ = 0;
    private int fK = 0;
    private int fL = 1;
    private byte[] fM = new byte[78400];
    private int[] fN = new int[78400];
    private int[] fO = new int[78400];
    private int[] fP = new int[560];
    private int fQ = 0;
    private int[] fR = new int[8816];
    private int fS;
    private int fT;
    private byte[] fU;
    private int fV = 1500;
    private long fW = 0L;
    private long fX;
    private long fY;
    private int fZ = 0;
    private int ga = 1;
    private byte[] gb = new byte[]{-61, -114, 121, -68, 112, 7, 50, -13, 56, -10, 27, -40, -53, 44, -122, 88, 1, -106, 89, 12, -80, -64, 103, 25, 125, -106, -53, 96, 62, -53, 45, -106, 24, 111, 99, 44, -78, -39, 100, 6, 89, 101, -78, -61, 13, -113, 125, -106, 91, 44, -128, -53, 44, -74, 95, 125, -79, -25, 50, -13, 100, -32, 25, 57, -26, 113, -17, -74};
    private int gc;
    private int gd;
    private static final int ge = 20;
    private Toolkit gf;
    private Image gg;
    private int gh;
    private int gi = 10;
    private int gj = 0;
    private int[] gk = new int[141];
    private int[] gl = new int[141];
    private int gm;
    private int gn;
    private int go = 1;
    private int gp;
    public static boolean gq;

    public void init() {
        int n2;
        this.fl = Troisd.c("pcT!HImW2\u000b_aM$\r\u001a`[`#_mR3H\u001c\"q4\tHnK/\u0006\u001adP/\u0005\u001aGS5\u0001TmZ");
        this.fl = Troisd.c("[nQ/HYcN,\r^8\u0002\u0013\rXcQ4\u0001_l\u0002\u0016\u0001[pF`N\u001aAN!\u001d^g\u0002\u0001\u000f_x");
        this.fk = Troisd.c("mgN#\u0007Wg\u0002(\rHg\u0002,\tWgPa");
        this.bR = new b.b.b();
        this.fq = false;
        this.bA = false;
        this.setLayout(null);
        this.addNotify();
        this.gf = this.getToolkit();
        this.fl = Troisd.c("Sd\u00029\u0007O\"C2\r\u001apG!\fSlE`\u001cRkQlHNjG.HCmW`\tHg\u0002,\u0007Ui\u0002!\u001c\u001aqM-\rNjK.\u000f\u001a{M5HIjM5\u0004^l\u00054I");
        this.fk = Troisd.c("RvV0R\u0015-G1\u001dSlM8FJnC.\rN/Fn\u0006_v\r");
        this.showStatus(Troisd.c("VmC$\u0001Te\fnF"));
        this.G = 4;
        this.gm = Integer.parseInt(this.getParameter(Troisd.c("WwQ)\u000b")));
        if (!(this.G == 4 || this.G == 1 || this.G == 2)) {
            this.fq = true;
            System.out.println(Troisd.c("mj[\u007f"));
            return;
        }
        this.gc = 0;
        if (this.G == 3 || this.G == 4) {
            this.fo = 1;
            this.be = 3000;
            this.bf = 5200;
            for (n2 = 0; n2 < this.bR.b.length(); ++n2) {
                this.gn += this.a(this.bR.b, n2);
            }
            this.bg = 4000;
            this.bh = 2500;
        }
        this.fi = (float)(Math.cos(1.215) * 20.0);
        if (this.G == 1 || this.G == 2) {
            this.fo = (byte)2;
            this.be = 1500;
            this.bf = 3000;
            this.bg = 3000;
            this.bh = 1500;
        }
        if (this.G == 3 || this.G == 4) {
            this.H = 280;
            this.I = 280;
        } else {
            this.H = 280;
            this.I = 280;
            this.fg = 2;
        }
        this.J = 1.0f;
        this.bG = new b.f.a();
        this.fl = Troisd.c("Sd\u00024\u0000_l\u000e`\u001bRcO%HUl\u00029\u0007O#");
        this.bH = new b.f.b();
        this.ft = 1;
        this.bI = new b.g.a();
        this.bJ = new b.c.a();
        this.bK = new b();
        this.bL = new f();
        if (this.gn == 79467) {
            this.fu = 1;
            this.fk = Troisd.c("[*\u000b{");
            this.fp = null;
            this.bM = new g();
            this.bN = new b.a.a();
            this.bO = new b.d.a();
        }
        this.bP = new j();
        this.bQ = new b.b.a();
        this.fk = ";";
        if (this.G == 1 || this.G == 2) {
            this.bx = new byte[2808];
            this.fi = 45.5f;
            this.eV += 2808;
            this.bO.b(this.bx);
            this.fk = Troisd.c("pTo`\rHpM2");
        }
        this.df = new int[1];
        this.eV = 0;
        this.dh = 0.0f;
        this.di = 0.0f;
        this.dj = 1.0f;
        this.bV = new a.a();
        this.dd = new float[this.H + 1000];
        for (n2 = 0; n2 < 1001; ++n2) {
            this.dd[0] = 0.0f;
        }
        for (n2 = 1001; n2 < this.H + 1000; ++n2) {
            this.dd[n2] = 1.0f / (float)(n2 - 1000);
        }
        this.eV += 4 * (this.H + 1000);
        for (n2 = 0; n2 < 141; ++n2) {
            this.gk[n2] = 0xFF000000 | ((int)(1.5070921f * (float)(n2 / 2 + 70)) & 0xFF) << 8 | ((int)(1.8085107f * (float)(140 - n2)) & 0xFF) << 16;
        }
        for (n2 = 0; n2 < 141; ++n2) {
            this.gl[n2] = 0xFF00FF00 | ((int)(1.8085107f * (float)(140 - n2)) & 0xFF) << 16;
        }
        this.gl[140] = -16711936;
        if (this.G == 3 || this.G == 4) {
            this.cy = new byte[16][256];
            this.fj = 0.0;
            this.eV += 4096;
        }
        switch (this.ft) {
            case 0: {
                this.fi = 2.0f;
                this.g();
                this.h();
                this.fq = false;
                break;
            }
            case 1: {
                this.fq = true;
                this.dh = 0.0f;
                this.di = 0.0f;
                this.dj = 1.0f;
                this.g();
                this.h();
                this.i();
            }
        }
        this.bS = new l();
        this.bT = new e();
        this.bK.a(this.cb, this.H, this.I);
        System.gc();
        System.gc();
        if (this.G == 3) {
            this.bc = 1;
            this.bd = 15;
            this.W = 16;
            this.X = 95;
            this.fp = null;
            this.Y = 96;
            this.Z = 175;
            this.ba = 175;
            this.bb = 255;
        }
        if (this.G == 4) {
            this.W = 20;
            this.X = 127;
            this.Y = 20;
            this.Z = 127;
            this.r();
            this.fq = true;
            this.s();
            this.ba = 20;
            this.bb = 127;
        }
        this.cc = new byte[256];
        this.cd = new byte[256];
        this.ce = new byte[256];
        this.eV += 768;
        this.fl = Troisd.c("HgC$\u0011\u001adM2H[\"J5\u000f_\"O%\u0006NcN`\u0000[lE/\u001e_p\u001d");
        this.cc[0] = 0;
        this.cd[0] = 0;
        this.ce[0] = 0;
        switch (this.G) {
            case 1: {
                this.r();
                this.s();
                this.fo = 1;
                this.bJ.a(this.cc, this.cd, this.ce, 0, 255, 60, 40, 60, 80, 50, 20, 255, 255, 255, 13);
                this.b(Troisd.c("\u00ec\u00e6\u00c9\u00bc\u0087"));
                break;
            }
            case 2: {
                this.fk = Troisd.c("\u00fe\u00c9\u00fe\u008f\u00be\u00de\u00e9\u00de\u00af\u009e\u00f8\u00d9\u00ec\u0094K\u00dd");
                this.bJ.a(this.cc, this.cd, this.ce, 0, 255, 0, 40, 60, 0, 50, 20, 255, 255, 255, 7);
                this.q();
                this.fo = 1;
                break;
            }
        }
        this.fk = Troisd.c("RmL4\r\u001ac\u00024\u0007S\"J/\u001aHk@,\r\u001apK0\u0018_wPa");
        if (this.G == 1 || this.G == 2) {
            this.bu = new byte[this.H * this.I];
            this.eV += this.H * this.I;
            this.fk = Troisd.c("tWn\f");
        }
        if (this.G == 3) {
            this.r();
            this.s();
            this.fo = 1;
            this.bJ.a(this.cc, this.cd, this.ce, this.bc, this.bd, 20, 50, 30, 10, 40, 20, 255, 255, 255, 7);
            this.bJ.a(this.cc, this.cd, this.ce, this.W, this.X, 80, 50, 80, 80, 50, 20, 255, 255, 255, 7);
            this.bJ.a(this.cc, this.cd, this.ce, this.Y, this.Z, 70, 60, 80, 60, 60, 40, 255, 255, 255, 7);
            this.fo = 1;
            this.bJ.a(this.cc, this.cd, this.ce, this.ba, this.bb, 50, 40, 90, 50, 30, 50, 255, 255, 255, 7);
        }
        this.a(this.getDocumentBase(), Troisd.c("VmC$\rH3\f*\u0018]"), 1, this.fO);
        this.ca = 0;
        if (this.G == 3 || this.G == 4) {
            this.r();
            this.s();
            this.fo = 1;
            for (int i2 = 0; i2 < 16; ++i2) {
                for (int i3 = 0; i3 < 256; ++i3) {
                    n2 = (this.cc[i3] & 0xFF) + (this.cd[i3] & 0xFF) + (this.ce[i3] & 0xFF) + (this.cc[i2] & 0xFF) + (this.cd[i2] & 0xFF) + (this.ce[i2] & 0xFF);
                    int n3 = 15;
                    for (int i4 = 0; i4 < 16; ++i4) {
                        int n4 = (this.cc[i4] & 0xFF) + (this.cd[i4] & 0xFF) + (this.ce[i4] & 0xFF);
                        this.fj = -2.0;
                        this.fk = Troisd.c("_zA%\u0018NkM.");
                        if (n4 < n2) continue;
                        n3 = i4;
                        i4 = 16;
                    }
                    this.cy[i2][i3] = (byte)n3;
                    this.fk = Troisd.c("pTo`\rHpM2");
                }
            }
        }
        this.fk = "?";
        this.fk = "?";
        this.fk = "?";
        this.fk = "?";
        this.fk = "?";
        this.fk = "?";
        this.fk = "?";
        this.fk = "?";
        System.gc();
        if (this.G == 1 || this.G == 2) {
            this.fk = Troisd.c("NjK3");
            this.a(Troisd.c("\u00da]\u00d4\u00b47\u007fSz\u0084"));
        }
        this.fk = Troisd.c("WmT`[^.\u0011$");
        this.eo = 0.0f;
        this.bU = new i();
        this.bZ = 0L;
        this.fF = -1200;
        this.m();
        String string = Troisd.c("\\mP\"\u0001^fG.\u000bUnM5\u001aI,C(\u0010");
        System.out.println(Troisd.c("Jr\u0010"));
        this.fW = 100L;
        this.fX = 200L;
        o o2 = new o();
        o2.setPriority(1);
        o2.a(this);
        o2.start();
        System.out.println(this.gn);
        System.out.println(this.gp);
    }

    public void destroy() {
        this.bC.flush();
        this.fO = null;
        this.bv = null;
        this.bE = null;
        this.cb = null;
    }

    public void start() {
        if (this.bB == null) {
            this.bB = new Thread(this);
            this.bB.start();
        }
    }

    public void stop() {
        if (this.bB != null) {
            this.bB.stop();
            this.bB = null;
            if (this.gm == 1 && this.bo) {
                try {
                    AudioPlayer.player.stop(this.bi);
                }
                catch (Exception exception) {
                    // empty catch block
                }
            }
        }
    }

    public void run() {
        while (this.bB != null && this.bB.isAlive()) {
            try {
                Thread.sleep(10L);
            }
            catch (InterruptedException interruptedException) {
                // empty catch block
            }
            this.bF.newPixels();
            if (this.gj == 0) {
                if (this.bn) {
                    this.b(this.bV.d);
                }
                this.repaint();
            }
            if (this.gj != 1) continue;
            this.fq = true;
            switch (this.ft) {
                case 0: {
                    this.r();
                    this.s();
                    this.fo = 1;
                    break;
                }
                case 1: {
                    this.p();
                    this.fk = Troisd.c("Y%G3\u001c\u001atP!\u0001WgL4HNpG3HWcN`\f_\"P%\u000f[pF%\u001a\u001aaG`\u001bUwP#\r\u001b");
                    this.q();
                    this.c();
                }
            }
            this.fk = Troisd.c("\u00d3]\u00c2\u00a87e");
            this.repaint();
            this.a();
        }
    }

    public void a() {
        Thread.yield();
        ++this.bZ;
        this.fk = Troisd.c("\u00d3\u00c0\u00c8\u00ab\u0088\u0013]\u00c2\u00a87e");
        this.fi = 0.0f;
        this.gf.sync();
        this.bY = System.currentTimeMillis();
        if (this.bY - this.bX > 1000L) {
            this.gh = (int)(1000L * this.bZ / (this.bY - this.bX));
            this.bZ = this.gh;
            this.showStatus(this.bZ + Troisd.c("\u001adR3H\u0015\"V)\u0005_aM5\u0006N8") + this.fZ + Troisd.c("\u001a-\u0002#\u0007OlV\u001f\u0004Nr\u0018") + this.fF + Troisd.c("\u001avK-\rH8") + this.fW + Troisd.c("\u001acF*7\\rQ") + this.gi);
            this.J = 70.0f / (float)this.bZ;
            if (this.J > 5.0f) {
                this.J = 5.0f;
            }
            this.bX = this.bY;
            if (this.fZ > 1 && this.ga < 0 && this.fZ < 10 && this.fF < -404) {
                if (this.bZ <= 10L) {
                    this.fW += -4L;
                }
                if (this.bZ <= 20L && this.bZ > 10L) {
                    this.fW += -3L;
                }
                if (this.bZ <= 30L && this.bZ > 20L) {
                    this.fW += -2L;
                }
                if (this.bZ <= 65L && this.bZ >= 30L) {
                    this.fW += -1L;
                }
                if (this.bZ >= 85L) {
                    ++this.fW;
                }
                if (this.fW > 11L) {
                    this.fW = 11L;
                }
            }
            if (this.fW < 1L) {
                this.fW = 1L;
            }
            System.gc();
            ++this.fZ;
            this.bZ = 0L;
            this.ga = -this.ga;
        }
    }

    private void b() {
        this.a = 983040;
        this.bl = new byte[this.a];
        this.bm = new byte[this.a];
        this.bn = true;
        try {
            this.bV.a(new URL(this.getCodeBase(), Troisd.c("VvR3\u001dX,X)\u0018")), this.bl, this.bm, this.a);
        }
        catch (Exception exception) {
            // empty catch block
        }
        this.bi = new ContinuousAudioDataStream(new AudioData(this.bl));
        this.bo = this.bV.e;
        if (this.bo) {
            try {
                AudioPlayer.player.start(this.bi);
            }
            catch (Exception exception) {
                // empty catch block
            }
        }
    }

    public void update(Graphics graphics) {
        if (this.bC != null) {
            graphics.drawImage(this.bC, 0, 0, null);
        }
    }

    public void c() {
        int n2;
        int n3;
        int n4;
        if (this.G == 4) {
            this.fq = true;
            if (this.fF < 900 && this.fF >= 0) {
                this.e();
            }
        }
        this.df[0] = 0;
        this.eZ = 0;
        this.eX = 0;
        this.fk = Troisd.c("\u00d3-\u00c8\u00abK\u00d3]\u00c2\u00a87e");
        this.eW = 0;
        this.eY = 0;
        this.fk = Troisd.c("Rk");
        if (this.fF < 0 && this.o()) {
            int n5;
            int n6;
            this.fq = true;
            if (this.fF >= -1222 && this.fF <= -1202) {
                this.bP.b(this.fO, -1202 - this.fF);
            } else if (this.fF == -1201) {
                this.bP.a(this.fN, 20);
            } else if (this.fF >= -1200 && this.fF <= -1180) {
                this.bP.b(this.fO, 1200 + this.fF);
            } else if (this.fF >= -280) {
                n4 = this.gh > 80 ? 3 : (this.gh > 70 ? 2 : 1);
                for (n3 = 0; n3 < 280; n3 += 2) {
                    if ((n2 = this.fP[this.fQ++]) == 0) continue;
                    System.arraycopy(this.fN, n3 * 280 - n2 + 280, this.fO, n3 * 280, n2);
                    System.arraycopy(this.fN, (n3 + 1) * 280, this.fO, (n3 + 1) * 280 + 280 - n2, n2);
                }
                this.fQ += -140;
                ++this.fQ;
                n6 = 100;
                for (n3 = 0; n3 < 76; ++n3) {
                    for (n2 = 0; n2 < 100; ++n2) {
                        n4 = this.fR[n3 * 116 + n2];
                        if (n4 == -16777216) continue;
                        n5 = this.fO[17060 + n3 * 280 - n6 + n2];
                    }
                }
            } else if (this.fF >= -1180 && this.fF <= -1000) {
                n6 = 116;
                for (n3 = 0; n3 < 76; ++n3) {
                    for (n2 = 0; n2 < n6; ++n2) {
                        n4 = this.fR[n3 * 116 + n2];
                        if (n4 == -16777216) continue;
                        n4 = this.fO[17060 + n3 * 280 - n6 + n2];
                    }
                }
            }
            if (this.gn != 79467) {
                return;
            }
            if (this.fF == -1000) {
                this.fZ = 0;
            }
            if (this.fF >= -1000 && this.fF < -884 || this.fF >= -520 && this.fF < -404) {
                this.fg = 5;
                n5 = this.fF >= -520 && this.fF < -404 ? 2 : 1;
                System.arraycopy(this.fN, 0, this.fO, 0, 78400);
                for (n3 = 0; n3 < 76; ++n3) {
                    n6 = n5 == 1 ? 1000 + this.fF : -404 - this.fF;
                    for (n2 = 0; n2 < n6; ++n2) {
                        n4 = this.fR[n3 * 116 + n2];
                        if (n4 == -16777216) continue;
                        this.fO[17060 + n3 * 280 - n6 + n2] = n4;
                    }
                }
            }
            if (this.fF >= -884 && this.fF < -520) {
                this.fp = null;
                n6 = 116;
                System.arraycopy(this.fN, 0, this.fO, 0, 78400);
                for (n3 = 0; n3 < 76; ++n3) {
                    for (n2 = 0; n2 < n6; ++n2) {
                        n4 = this.fR[n3 * 116 + n2];
                        if (n4 == -16777216) continue;
                        this.fO[260 - n6 + n2 + 16800 + n3 * 280] = n4;
                    }
                }
            }
        }
        if (this.fF == 0) {
            this.n();
            this.fY = this.fW;
            if (this.fW > 2L) {
                this.fW -= 2L;
            } else if (this.fW > 1L) {
                --this.fW;
            }
        }
        if (this.G == 3 || this.G == 4 && this.fF >= 0 && this.fF < 900) {
            this.fp = null;
            n2 = this.fF > 700 ? (this.fF - 700) * 4 : 0;
            this.fq = true;
            n3 = this.fC[this.fD];
            this.fk = ";";
            this.a(this.ch, this.eo * 1.1f + 0.0f + 1.0f, this.eo * 2.5f + 0.0f + 1.0f, this.eo * 1.7f + 0.0f + 2.5f, n3 + 150 + 30 - n2, n2 / 1, -400.0f - (float)n2);
            n3 = this.fC[this.fD];
            this.a(this.ci, this.eo * 1.1f + 0.4f + 1.0f, this.eo * 2.5f + 0.4f + 1.0f, this.eo * 1.7f + 0.4f + 2.5f, n3 + 30 - n2 / 2, -7.0f + (float)(n2 / 4), -200.0f - (float)n2);
            n3 = this.fC[this.fD];
            this.a(this.cj, this.eo * 1.1f + 0.8f + 1.0f, this.eo * 2.5f + 0.8f + 1.0f, this.eo * 1.7f + 0.8f + 2.5f, n3 - 100 + 30 - n2 / 2, n2 / 3, -n2);
            if (this.fD < 249) {
                ++this.fD;
            }
            this.bP.a(this.bE, Math.min(this.fV, this.fF * 4));
        }
        this.fq = true;
        if (this.fF == 900) {
            this.fi = 0.1f;
            if (this.fW > 2L) {
                this.fW -= 2L;
            } else if (this.fW > 1L) {
                --this.fW;
            }
        } else if (this.fF == 1700) {
            this.fW = this.fY;
        }
        if (this.gp != 1817312555) {
            this.t();
        }
        if (this.fF >= 905 && this.fF < 1705) {
            this.fp = null;
            if (this.fF < 1640) {
                this.a(this.cf, (float)Math.PI, this.eo * 2.0f, 1.5707964f, 0.0f, -60.0f, 0.0f);
            }
            if (this.fF >= 1640 && this.fF % 2 == 0) {
                this.a(this.bv);
            }
        }
        if (this.fF >= 1705 && this.fF < 5940) {
            if (this.fF == 2005 || this.fF == 2515) {
                if (this.gh < 30) {
                    if (this.fW > 2L) {
                        this.fW -= 2L;
                    } else if (this.fW > 1L) {
                        --this.fW;
                    }
                } else if (this.gh < 50) {
                    if (this.fW > 1L) {
                        --this.fW;
                    }
                } else if (this.gh > 85) {
                    ++this.fW;
                }
            }
            this.bP.a(this.bv);
            if (this.fF < 1740) {
                System.arraycopy(this.bv, 0, this.bE, 0, 280 * (this.fF - 905 - 800) * 8);
            } else {
                System.arraycopy(this.bv, 0, this.bE, 0, 78400);
            }
            if (this.fF >= 2075 && this.fF < 2135) {
                this.bP.a(this.fF - 905 - 800 - 370, 15.0f);
            }
            if (this.fF >= 2455 && this.fF < 2515) {
                this.bP.a(this.fF - 905 - 800 - 600 - 150, 10.0f);
            }
            if (this.fF >= 2955 && this.fF < 3015) {
                this.bP.a(this.fF - 905 - 800 - 1100 - 150, 24.0f);
            }
            if (this.fF >= 3355 && this.fF < 3415) {
                this.bP.a(this.fF - 905 - 800 - 1350 - 300, 30.0f);
            }
            if (this.fF >= 3855 && this.fF < 3915) {
                this.bP.a(this.fF - 905 - 800 - 1850 - 300, 20.0f);
            }
            this.bS.a(this.bE, this.fH, this.fI, this.fJ);
            ++this.fK;
            if (this.fK >= 360 && this.fL > 0) {
                this.fK = 0;
                this.fL = -this.fL;
            } else if (this.fK >= 510 && this.fL < 0) {
                this.fK = 0;
                this.fL = -this.fL;
            }
            if (this.fL == 1 && this.fF % 2 == 0) {
                ++this.fJ;
            }
            if (this.fJ >= 1320) {
                this.fJ = 0;
            }
        }
        if (this.fF >= 5939 && this.fF <= 6100 && this.fF % 2 == 0) {
            if (this.fF == 5940) {
                System.arraycopy(this.cb, 0, this.bE, 0, 560);
                System.arraycopy(this.cb, 0, this.bE, 77840, 560);
            }
            for (n3 = 281; n3 < 78119; ++n3) {
                this.bE[n3] = (byte)((this.bE[n3 - 1] & 0xFF) + (this.bE[n3 + 1] & 0xFF) + (this.bE[n3 - 280] & 0xFF) + (this.bE[n3 + 280] & 0xFF) >> 2);
            }
            this.fp = null;
            for (n3 = 281; n3 < 78119; ++n3) {
                n4 = this.bE[n3] & 0xFF;
                this.bE[n3] = n4 > 0 ? (byte)(n4 - 1) : (byte)0;
            }
        }
        ++this.fF;
        if (this.fF > 6101) {
            this.fq = false;
            this.fF = -1201;
            this.m();
            this.fQ = 0;
            this.fJ = 0;
            this.fD = 0;
            this.fL = 1;
            this.fK = 0;
            for (n3 = 0; n3 < 78400; ++n3) {
                this.cb[n3] = 0;
            }
            System.arraycopy(this.cb, 0, this.bE, 0, 78400);
            System.arraycopy(this.cb, 0, this.bv, 0, 78400);
            if ((double)this.eo > Math.PI * 2) {
                this.eo = 0.0f;
            }
            this.fp = null;
            this.bP.h[0] = 0;
            for (n3 = 0; n3 < this.fV; ++n3) {
                this.bP.d[n3] = -2000;
            }
        }
        if (this.G == 3) {
            this.K = (float)((double)((this.I - 112) / 2) * Math.sin(this.eo));
            this.L = (float)((double)this.H * Math.sin(this.eo)) - (float)(this.H / 2);
            this.a(this.cg, this.eo * 1.1f + 0.8f, this.eo * 2.5f + 0.8f, this.eo * 1.7f + 0.8f, 0.0f, this.K, this.L);
            this.K = (float)((double)((this.I - 112) / 2) * Math.sin(3.0f * this.eo));
            this.L = (float)((double)this.H * Math.sin(1.5 * (double)this.eo)) - (float)(this.H / 2);
            this.fi = 2.5f;
            this.a(this.cg, this.eo * 1.2f + 0.8f, this.eo * 2.6f + 0.8f, this.eo * 1.8f + 0.8f, 0.0f, this.K, this.L);
            this.K = (float)((double)((this.I - 112) / 2) * Math.sin(1.9 * (double)this.eo));
            this.L = (float)((double)this.H * Math.sin(-1.1 * (double)this.eo)) - (float)(this.H / 2);
            this.a(this.cg, this.eo * 1.3f + 0.8f, this.eo * 2.5f + 0.8f, this.eo * 1.7f + 0.8f, 0.0f, this.K, this.L);
            this.K = (float)((double)((this.I - 112) / 2) * Math.sin(1.5 * (double)this.eo));
            this.b(Troisd.c("\u00d1\u00f4\u00cc\u00b4"));
            this.L = (float)((double)this.H * Math.cos(0.8 * (double)this.eo)) - (float)(this.H / 2);
            this.a(this.cg, this.eo * 1.4f + 0.8f, this.eo * 2.7f + 0.8f, this.eo * 1.9f + 0.8f, 0.0f, this.K, this.L);
        }
        switch (this.G) {
            case 1: {
                this.fq = true;
                this.a(this.cf, 0.0f, this.bt * 2.5f, -this.bt * 1.3f, 0.0f, 0.0f, 0.0f);
                this.bt = (float)((double)this.bt + 0.01);
                this.b(Troisd.c("\u00d1\u00f4\u00cc\u00b4"));
                break;
            }
            case 2: {
                this.fg = 1;
                this.fw = (float)((double)this.fw + 0.01333);
                this.fx = (float)((double)this.fx + 0.02222);
                this.fy = (float)((double)this.fy + 0.01577);
                this.fz = (float)(Math.cos(this.fw) * 3.0);
                this.fA = (float)(Math.cos(this.fx) * 2.0);
                this.fB = (float)(Math.cos(this.fy) * 2.5);
                this.fk = ";";
                if (!this.bA) break;
                this.a(this.cf, this.fz, this.fA, this.fB, 0.0f, 0.0f, 0.0f);
                break;
            }
        }
        if (this.fF >= 0 && this.fF < 1700) {
            this.bG.a(0, this.df[0] - 1, this.cn, this.co);
            this.fk = Troisd.c("d\u00eb\rc\u0081e\u00e2\u00ca\u001f7");
            this.eo = (float)((double)this.eo + 0.01 * (double)this.J);
            this.fk = Troisd.c("pTo`\rHpM2");
            this.dG = 0;
            while (this.dG < this.df[0]) {
                this.dL = this.co[this.dG];
                switch (this.cp[this.dL]) {
                    case 2: {
                        this.a(this.dL);
                        break;
                    }
                    case 1: {
                        if (this.fF < 920) {
                            this.a(this.dL, this.bE);
                            break;
                        }
                        if (this.fF < 920) break;
                        this.a(this.dL, this.bv);
                    }
                }
                ++this.dG;
            }
            this.fl = Troisd.c("\u007fsW)\u0006Uz\u0003");
            this.fl = Troisd.c("jmM(I");
        }
        if (this.fF <= 1705 && this.fF > 900) {
            if (this.fF < 1640) {
                this.a(this.bv);
            }
            if (this.fF < 1640 || this.fF >= 1640 && this.fF % 2 == 1) {
                this.bU.a(this.bE, this.fH);
            }
        }
        if (this.fF >= 900 && this.fF < 920) {
            System.arraycopy(this.fM, 0, this.bE, 0, 78400);
            System.arraycopy(this.cb, 0, this.bv, 0, 78400);
        }
        if (this.fF >= 920 && this.fF < 990) {
            n3 = 990 - this.fF;
            System.arraycopy(this.fM, 39200 - n3 * 280 * 2, this.bE, 0, n3 * 280 * 2);
            System.arraycopy(this.fM, 39200, this.bE, (280 - n3 * 2) * 280, n3 * 280 * 2);
        }
        boolean bl = false;
        int n7 = 12662;
        if (this.fF > 450 && this.fF < 900) {
            int n8;
            if (this.fF < 578) {
                n8 = 578 - this.fF;
            } else {
                n8 = this.fF - 450 - 128;
                if (n8 > 10) {
                    n8 = 10;
                }
            }
            this.bT.a(n8, n7, this.bE, this.fG);
        }
        if (this.fF > 829 && this.fF < 900) {
            n3 = this.fF - 829;
            System.arraycopy(this.fM, 39200 - n3 * 280 * 2, this.bE, 0, n3 * 280 * 2);
            System.arraycopy(this.fM, 39200, this.bE, (280 - n3 * 2) * 280, n3 * 280 * 2);
        }
    }

    public void d() {
        this.fo = (byte)3;
    }

    public void e() {
        System.arraycopy(this.cb, 0, this.bE, 0, 78400);
    }

    public void a(byte[] byArray) {
        boolean bl = false;
        this.fo = (byte)3;
        for (int i2 = 15408; i2 < 78391; ++i2) {
            this.bq = (byArray[i2 - 8] & 0xFF) + (byArray[i2 + 8] & 0xFF) >> 1;
            if (this.bq > 0) {
                --this.bq;
                byArray[i2] = (byte)this.bq;
                continue;
            }
            byArray[i2] = 0;
        }
        System.arraycopy(byArray, 0, this.bE, 0, 78400);
    }

    private void f() {
        int n2 = 0;
        this.bz = new byte[1000];
        for (int i2 = 0; i2 < 10; ++i2) {
            this.fp = null;
            for (int i3 = 2; i3 < 98; ++i3) {
                this.fq = true;
                if (this.by[n2++] == 0) continue;
                this.fk = Troisd.c("tWn\f");
                this.bz[i2 * 100 + i3] = -1;
            }
        }
    }

    private void a(a a2, float f2, float f3, float f4, float f5, float f6, float f7) {
        this.fo = (byte)3;
        this.bH.a(a2, this.eX, f2, f3, f4, f5, f6, f7, this.ck, this.cl, this.cm, this.cq, this.H, this.I);
        this.eZ = this.bN.a(a2, this.eX, this.ck, this.cl, this.cm, this.cn, this.co, this.cp, this.eZ, this.cr, this.cq, this.df);
        this.fq = true;
        this.eY += a2.c;
        this.eW = this.df[0];
        this.eX += a2.b;
        this.fp = null;
    }

    /*
     * Unable to fully structure code
     */
    private void a(int var1_1) {
        block82: {
            block73: {
                var2_2 = Troisd.gq;
                this.dZ = var1_1 + 1;
                this.dg = this.cp[this.dZ++];
                this.ea = this.cp[this.dZ++];
                this.eq = this.cl[this.ea];
                this.ep = this.ck[this.ea];
                this.fg = 0;
                this.eC = this.eq;
                this.eD = this.eq;
                this.fq = true;
                this.eE = this.ep;
                this.eF = this.ep;
                this.ct[0] = this.eq;
                this.cs[0] = this.ep;
                this.eb = 1;
                this.fk = Troisd.c("jmW%\u001c");
                this.ec = this.dg - 1;
                while (this.ec > 0) {
                    block81: {
                        block80: {
                            block79: {
                                block78: {
                                    this.ea = this.cp[this.dZ++];
                                    this.ep = this.ck[this.ea];
                                    v0 = this.eq = this.cl[this.ea];
                                    v1 = this.eC;
                                    if (var2_2) ** GOTO lbl46
                                    if (v0 >= v1) break block78;
                                    this.eC = this.eq;
                                    if (!var2_2) break block79;
                                }
                                if (this.eq > this.eD) {
                                    this.eD = this.eq;
                                }
                            }
                            if (this.ep >= this.eE) break block80;
                            this.eE = this.ep;
                            if (!var2_2) break block81;
                        }
                        if (this.ep > this.eF) {
                            this.eF = this.ep;
                        }
                    }
                    this.cs[this.eb] = this.ep;
                    this.ct[this.eb++] = this.eq;
                    --this.ec;
                    if (!var2_2) continue;
                }
                this.eb = 0;
                do {
                    v0 = this.eb;
                    v1 = this.dg;
lbl46:
                    // 2 sources

                    if (v0 >= v1) break;
                    this.cu[this.eb] = this.cp[this.dZ++];
                    ++this.eb;
                    if (var2_2) break block73;
                } while (!var2_2);
                if (this.eD >= 0 && this.eC <= this.I - 1 && this.eF >= 0 && this.eE <= this.H - 1) break block82;
            }
            return;
        }
        this.fi = 1.5f;
        if (this.eC < 0 || this.eD >= this.I || this.eE < 0 || this.eF >= this.H) {
            block75: {
                block74: {
                    this.ed = this.dg;
                    this.ep = this.cs[0];
                    this.eq = this.ct[0];
                    this.eA = this.cu[0];
                    this.ee = 0;
                    this.ec = 0;
                    while (this.ed > 0) {
                        ++this.ec;
                        this.ec %= this.dg;
                        this.er = this.cs[this.ec];
                        this.es = this.ct[this.ec];
                        this.eB = this.cu[this.ec];
                        this.ev = 0;
                        v2 = this.eq;
                        if (!var2_2) {
                            if (v2 < 0) {
                                ++this.ev;
                            }
                            if (this.eq > this.I - 1) {
                                this.ev += 2;
                            }
                            if (this.es < 0) {
                                this.ev += 4;
                            }
                            if (this.es > this.I - 1) {
                                this.ev += 8;
                            }
                            switch (this.ev) {
                                case 0: {
                                    this.cv[this.ee] = this.ep;
                                    this.cw[this.ee] = this.eq;
                                    this.cx[this.ee++] = this.eA;
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.fi = 0.0f;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 1: {
                                    this.cW = (-this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = 0;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 2: {
                                    this.fi = 3.1415927f;
                                    this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = this.I - 1;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 4: {
                                    this.cv[this.ee] = this.ep;
                                    this.cw[this.ee] = this.eq;
                                    this.cx[this.ee++] = this.eA;
                                    this.cW = (-this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = 0;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 5: {
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 6: {
                                    this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = this.I - 1;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.cW = (-this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = 0;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 8: {
                                    this.cv[this.ee] = this.ep;
                                    this.cw[this.ee] = this.eq;
                                    this.cx[this.ee++] = this.eA;
                                    this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = this.I - 1;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 9: {
                                    this.cW = (-this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = 0;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                                    this.cv[this.ee] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                                    this.cw[this.ee] = this.I - 1;
                                    this.cx[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                    if (!var2_2) break;
                                }
                                case 10: {
                                    this.ep = this.er;
                                    this.eq = this.es;
                                    this.eA = this.eB;
                                }
                            }
                            --this.ed;
                            if (!var2_2) continue;
                        }
                        break block74;
                    }
                    v2 = this.dg = this.ee;
                }
                if (v2 == 0) {
                    return;
                }
                this.ed = this.dg;
                this.fq = false;
                this.fq = false;
                this.ep = this.cv[0];
                this.eq = this.cw[0];
                this.eA = this.cx[0];
                this.ee = 0;
                this.ec = 0;
                this.fg = 2;
                while (this.ed > 0) {
                    ++this.ec;
                    this.ec %= this.dg;
                    this.er = this.cv[this.ec];
                    this.es = this.cw[this.ec];
                    this.eB = this.cx[this.ec];
                    this.ev = 0;
                    v3 = this.ep;
                    if (!var2_2) {
                        if (v3 < 0) {
                            ++this.ev;
                        }
                        if (this.ep > this.H - 1) {
                            this.ev += 2;
                        }
                        if (this.er < 0) {
                            this.ev += 4;
                        }
                        if (this.er > this.H - 1) {
                            this.ev += 8;
                        }
                        switch (this.ev) {
                            case 0: {
                                this.cs[this.ee] = this.ep;
                                this.ct[this.ee] = this.eq;
                                this.cu[this.ee++] = this.eA;
                                this.ep = this.er;
                                this.eq = this.es;
                                this.fi = 1.0f;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 1: {
                                this.cW = (-this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = 0;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.ep = this.er;
                                this.eq = this.es;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 2: {
                                this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = this.H - 1;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.ep = this.er;
                                this.eq = this.es;
                                this.fi = 1.0f;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 4: {
                                this.cs[this.ee] = this.ep;
                                this.ct[this.ee] = this.eq;
                                this.cu[this.ee++] = this.eA;
                                this.cW = (-this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = 0;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.ep = this.er;
                                this.eq = this.es;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 5: {
                                this.ep = this.er;
                                this.eq = this.es;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 6: {
                                this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = this.H - 1;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.cW = (-this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = 0;
                                this.fi = 1.0f;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.ep = this.er;
                                this.eq = this.es;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 8: {
                                this.cs[this.ee] = this.ep;
                                this.ct[this.ee] = this.eq;
                                this.cu[this.ee++] = this.eA;
                                this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = this.H - 1;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.ep = this.er;
                                this.eq = this.es;
                                this.fq = true;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 9: {
                                this.cW = (-this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = 0;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                                this.ct[this.ee] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                                this.cs[this.ee] = this.H - 1;
                                this.cu[this.ee++] = this.eA + (this.cW * (this.eB - this.eA) >> 16);
                                this.ep = this.er;
                                this.eq = this.es;
                                this.eA = this.eB;
                                if (!var2_2) break;
                            }
                            case 10: {
                                this.ep = this.er;
                                this.eq = this.es;
                                this.eA = this.eB;
                            }
                        }
                        --this.ed;
                        if (!var2_2) continue;
                    }
                    break block75;
                }
                v3 = this.dg = this.ee;
            }
            if (v3 == 0) {
                return;
            }
        }
        this.ee = 0;
        while (this.ee < this.dg) {
            block86: {
                block77: {
                    block88: {
                        block87: {
                            block83: {
                                block76: {
                                    block85: {
                                        block84: {
                                            this.ec = (this.ee + 1) % this.dg;
                                            this.er = this.cs[this.ee];
                                            this.es = this.ct[this.ee];
                                            this.R = this.cu[this.ee];
                                            this.et = this.cs[this.ec];
                                            this.fg = 2;
                                            this.eu = this.ct[this.ec];
                                            this.S = this.cu[this.ec];
                                            this.fg = 1;
                                            if (Math.abs(this.et - this.er) <= Math.abs(this.eu - this.es)) break block83;
                                            if (this.er > this.et) {
                                                this.dS = this.er;
                                                this.er = this.et;
                                                this.et = this.dS;
                                                this.dS = this.es;
                                                this.es = this.eu;
                                                this.eu = this.dS;
                                                this.dS = this.R;
                                                this.R = this.S;
                                                this.S = this.dS;
                                            }
                                            this.dS = this.et - this.er;
                                            this.es <<= 16;
                                            this.eu <<= 16;
                                            this.R <<= 16;
                                            this.S <<= 16;
                                            if (this.dS != 0) break block84;
                                            this.Q = 0;
                                            this.V = 0;
                                            if (!var2_2) break block85;
                                        }
                                        this.Q = (this.eu - this.es) / this.dS;
                                        this.V = (this.S - this.R) / this.dS;
                                    }
                                    this.dS >>= 1;
                                    while (this.dS > 0) {
                                        this.M = this.er++ + (this.es >> 16) * 280;
                                        this.N = this.et-- + (this.eu >> 16) * 280;
                                        this.bE[this.M] = this.cy[this.R >> 16][this.bE[this.M] & 255];
                                        this.bE[this.N] = this.cy[this.S >> 16][this.bE[this.N] & 255];
                                        this.es += this.Q;
                                        this.eu -= this.Q;
                                        this.R += this.V;
                                        this.S -= this.V;
                                        --this.dS;
                                        if (!var2_2) {
                                            if (!var2_2) continue;
                                        }
                                        break block76;
                                    }
                                    this.M = this.er + (this.es >> 16) * 280;
                                    this.N = this.et + (this.eu >> 16) * 280;
                                    this.O = this.bE[this.M] & 255;
                                    this.P = this.bE[this.N] & 255;
                                    this.bE[this.M] = this.cy[this.R >> 16][this.O];
                                    this.bE[this.N] = this.cy[this.S >> 16][this.P];
                                }
                                if (!var2_2) break block86;
                            }
                            if (this.es > this.eu) {
                                this.dS = this.er;
                                this.er = this.et;
                                this.et = this.dS;
                                this.dS = this.es;
                                this.es = this.eu;
                                this.eu = this.dS;
                                this.dS = this.R;
                                this.R = this.S;
                                this.S = this.dS;
                            }
                            this.dS = this.eu - this.es;
                            this.er <<= 16;
                            this.et <<= 16;
                            this.R <<= 16;
                            this.S <<= 16;
                            if (this.dS != 0) break block87;
                            this.Q = 0;
                            this.V = 0;
                            if (!var2_2) break block88;
                        }
                        this.Q = (this.et - this.er) / this.dS;
                        this.V = (this.S - this.R) / this.dS;
                    }
                    this.es *= 280;
                    this.eu *= 280;
                    this.dS >>= 1;
                    while (this.dS > 0) {
                        this.M = this.es + (this.er >> 16);
                        this.N = this.eu + (this.et >> 16);
                        this.bE[this.M] = this.cy[this.R >> 16][this.bE[this.M] & 255];
                        this.bE[this.N] = this.cy[this.S >> 16][this.bE[this.N] & 255];
                        this.er += this.Q;
                        this.et -= this.Q;
                        this.R += this.V;
                        this.S -= this.V;
                        this.es += 280;
                        this.eu -= 280;
                        --this.dS;
                        if (!var2_2) {
                            if (!var2_2) continue;
                        }
                        break block77;
                    }
                    this.M = this.es + (this.er >> 16);
                    this.N = this.eu + (this.et >> 16);
                    this.O = this.bE[this.M] & 255;
                    this.P = this.bE[this.N] & 255;
                    this.bE[this.M] = this.cy[this.R >> 16][this.O];
                }
                this.bE[this.N] = this.cy[this.S >> 16][this.P];
            }
            ++this.ee;
            if (!var2_2) continue;
        }
    }

    private void a(int n2, byte[] byArray) {
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        int n8;
        int n9;
        int n10;
        int n11;
        int n12;
        int n13 = n2 + 1;
        this.dg = this.cp[n13++];
        int n14 = this.cp[n13++];
        this.eq = this.cl[n14];
        this.ep = this.ck[n14];
        this.fq = true;
        int n15 = this.eq;
        int n16 = this.eq;
        int n17 = this.ep;
        int n18 = this.ep;
        this.ct[0] = this.eq;
        this.cs[0] = this.ep;
        int n19 = 1;
        this.fk = Troisd.c("jmW%\u001c");
        for (n12 = this.dg - 1; n12 > 0; --n12) {
            n14 = this.cp[n13++];
            this.ep = this.ck[n14];
            this.eq = this.cl[n14];
            if (this.eq < n15) {
                n15 = this.eq;
            } else if (this.eq > n16) {
                n16 = this.eq;
            }
            if (this.ep < n17) {
                n17 = this.ep;
            } else if (this.ep > n18) {
                n18 = this.ep;
            }
            this.cs[n19] = this.ep;
            this.ct[n19++] = this.eq;
        }
        for (n19 = 0; n19 < this.dg; ++n19) {
            this.cu[n19] = this.cp[n13++];
        }
        if (n16 < 0 || n15 > this.I - 1 || n18 < 0 || n17 > this.H - 1) {
            return;
        }
        this.fi = 1.5f;
        if (n15 < 0 || n16 >= this.I || n17 < 0 || n18 >= this.H) {
            int n20;
            this.ep = this.cs[0];
            this.eq = this.ct[0];
            int n21 = this.cu[0];
            n11 = 0;
            n12 = 0;
            block26: for (n10 = this.dg; n10 > 0; --n10) {
                ++n12;
                this.er = this.cs[n12 %= this.dg];
                this.es = this.ct[n12];
                n20 = this.cu[n12];
                this.ev = 0;
                if (this.eq < 0) {
                    ++this.ev;
                }
                if (this.eq > this.I - 1) {
                    this.ev += 2;
                }
                if (this.es < 0) {
                    this.ev += 4;
                }
                if (this.es > this.I - 1) {
                    this.ev += 8;
                }
                switch (this.ev) {
                    case 0: {
                        this.cv[n11] = this.ep;
                        this.cw[n11] = this.eq;
                        this.cx[n11++] = n21;
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 1: {
                        this.cW = (-this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = 0;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 2: {
                        this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = this.I - 1;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 4: {
                        this.cv[n11] = this.ep;
                        this.cw[n11] = this.eq;
                        this.cx[n11++] = n21;
                        this.cW = (-this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = 0;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 5: {
                        this.fi = 0.0f;
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 6: {
                        this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = this.I - 1;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.cW = (-this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = 0;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 8: {
                        this.cv[n11] = this.ep;
                        this.cw[n11] = this.eq;
                        this.cx[n11++] = n21;
                        this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = this.I - 1;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 9: {
                        this.cW = (-this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = 0;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.cW = (this.I - 1 - this.eq << 16) / (this.es - this.eq);
                        this.cv[n11] = this.ep + (this.cW * (this.er - this.ep) >> 16);
                        this.cw[n11] = this.I - 1;
                        this.cx[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        this.fi = 0.0f;
                        n21 = n20;
                        continue block26;
                    }
                    case 10: {
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block26;
                    }
                    case 11: {
                        this.fq = true;
                    }
                }
            }
            this.dg = n11;
            if (this.dg == 0) {
                return;
            }
            this.fq = false;
            this.fq = false;
            this.fg = 2;
            this.ep = this.cv[0];
            this.eq = this.cw[0];
            n21 = this.cx[0];
            n11 = 0;
            n12 = 0;
            block27: for (n10 = this.dg; n10 > 0; --n10) {
                ++n12;
                this.er = this.cv[n12 %= this.dg];
                this.es = this.cw[n12];
                n20 = this.cx[n12];
                this.ev = 0;
                if (this.ep < 0) {
                    ++this.ev;
                }
                if (this.ep > this.H - 1) {
                    this.ev += 2;
                }
                if (this.er < 0) {
                    this.ev += 4;
                }
                if (this.er > this.H - 1) {
                    this.ev += 8;
                }
                switch (this.ev) {
                    case 0: {
                        this.cs[n11] = this.ep;
                        this.ct[n11] = this.eq;
                        this.cu[n11++] = n21;
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block27;
                    }
                    case 1: {
                        this.cW = (-this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = 0;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        this.fq = true;
                        n21 = n20;
                        continue block27;
                    }
                    case 2: {
                        this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = this.H - 1;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block27;
                    }
                    case 4: {
                        this.cs[n11] = this.ep;
                        this.ct[n11] = this.eq;
                        this.cu[n11++] = n21;
                        this.cW = (-this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = 0;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block27;
                    }
                    case 5: {
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block27;
                    }
                    case 6: {
                        this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = this.H - 1;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.cW = (-this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = 0;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        this.fg = 1;
                        n21 = n20;
                        continue block27;
                    }
                    case 8: {
                        this.cs[n11] = this.ep;
                        this.ct[n11] = this.eq;
                        this.cu[n11++] = n21;
                        this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = this.H - 1;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        continue block27;
                    }
                    case 9: {
                        this.cW = (-this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = 0;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.cW = (this.H - 1 - this.ep << 16) / (this.er - this.ep);
                        this.ct[n11] = this.eq + (this.cW * (this.es - this.eq) >> 16);
                        this.cs[n11] = this.H - 1;
                        this.cu[n11++] = n21 + (this.cW * (n20 - n21) >> 16);
                        this.ep = this.er;
                        this.eq = this.es;
                        this.fg = 1;
                        n21 = n20;
                        continue block27;
                    }
                    case 10: {
                        this.ep = this.er;
                        this.eq = this.es;
                        n21 = n20;
                        this.fq = true;
                        continue block27;
                    }
                    case 11: {
                        this.fq = true;
                    }
                }
            }
            this.dg = n11;
            if (this.dg == 0) {
                return;
            }
        }
        int n22 = 0;
        int n23 = 0;
        n15 = this.eq = this.ct[0];
        n16 = this.eq;
        for (n19 = 1; n19 < this.dg; ++n19) {
            this.eq = this.ct[n19];
            if (this.eq < n15) {
                n15 = this.eq;
                n22 = n19;
                continue;
            }
            if (this.eq <= n16) continue;
            n16 = this.eq;
            n23 = n19;
        }
        n10 = n22 < n23 ? n23 - n22 : this.dg - (n22 - n23);
        this.fg = 3;
        n11 = 0;
        for (n19 = 0; n19 < n10; ++n19) {
            n12 = (n22 + n19) % this.dg;
            n9 = (n22 + n19 + 1) % this.dg;
            this.eq = this.ct[n12];
            this.es = this.ct[n9];
            if (this.eq == this.es) continue;
            this.fi = (float)Math.PI;
            this.da[n11] = this.cs[n12];
            this.db[n11] = this.eq;
            this.dc[n11++] = this.cu[n12];
            this.da[n11] = this.cs[n9];
            this.db[n11] = this.es;
            this.dc[n11++] = this.cu[n9];
            this.fg = 2;
        }
        int n24 = n11 >> 1;
        if (n24 < 1) {
            return;
        }
        n11 = 0;
        n10 = this.dg - n10;
        for (n19 = 0; n19 < n10; ++n19) {
            n12 = (n22 - n19 + this.dg) % this.dg;
            n9 = (n22 - n19 - 1 + this.dg) % this.dg;
            this.eq = this.ct[n12];
            this.es = this.ct[n9];
            if (this.eq == this.es) continue;
            this.fi = 2.0f;
            this.cX[n11] = this.cs[n12];
            this.cY[n11] = this.eq;
            this.cZ[n11++] = this.cu[n12];
            this.cX[n11] = this.cs[n9];
            this.cY[n11] = this.es;
            this.cZ[n11++] = this.cu[n9];
            this.fg = 1;
        }
        int n25 = n11 >> 1;
        if (n25 < 1) {
            return;
        }
        this.fq = true;
        this.cL = this.db[0] * this.H;
        this.cM = -1;
        this.cN = -1;
        this.cO = 0;
        this.cP = 0;
        this.fk = Troisd.c("\u007fsW)\u0006Uz");
        this.fo = (byte)2;
        while (n25 > 0 || n24 > 0) {
            this.fo = (byte)3;
            if (this.cN < 0) {
                this.cz = this.da[this.cO] << 16;
                this.cA = this.db[this.cO];
                this.cB = this.dc[this.cO++] << 16;
                this.cD = this.da[this.cO] << 16;
                this.cE = this.db[this.cO];
                this.cC = this.dc[this.cO++] << 16;
                this.cN = this.cE - this.cA;
                this.cQ = (this.cD - this.cz) / this.cN;
                this.cS = (this.cC - this.cB) / this.cN;
                --n24;
            }
            if (this.cM < 0) {
                this.cH = this.cX[this.cP] << 16;
                this.cJ = this.cY[this.cP];
                this.cF = this.cZ[this.cP++] << 16;
                this.cI = this.cX[this.cP] << 16;
                this.cK = this.cY[this.cP];
                this.cG = this.cZ[this.cP++] << 16;
                this.cM = this.cK - this.cJ;
                this.cT = (this.cI - this.cH) / this.cM;
                this.cV = (this.cG - this.cF) / this.cM;
                --n25;
                this.fp = null;
            }
            if (this.cM - this.cN > 0) {
                this.eU = this.cN;
                this.cM -= this.cN;
                this.cN = -1;
            } else {
                this.eU = this.cM;
                this.fo = 1;
                this.cN -= this.cM;
                if (this.cN == 0) {
                    this.cN = -1;
                }
                this.cM = -1;
            }
            while (this.eU > 0) {
                n8 = (this.cz >> 16) + this.cL;
                n7 = (this.cH >> 16) + this.cL;
                n6 = n7 - n8;
                n5 = this.cB;
                this.fo = 1;
                n4 = this.cF + n5;
                n3 = (this.cF - this.cB << 7) / ((n6 << 7) + 1);
                n6 >>= 1;
                n5 -= n3;
                while (n6 > 0) {
                    byArray[n8++] = (byte)((n5 += n3) >>> 16);
                    byArray[n7--] = (byte)(n4 - n5 >>> 16);
                    --n6;
                }
                byArray[n8] = (byte)((n5 += n3) >>> 16);
                byArray[n7] = (byte)(n4 - n5 >>> 16);
                this.fg = 2;
                this.cz += this.cQ;
                this.cH += this.cT;
                this.cB += this.cS;
                this.cF += this.cV;
                this.cL += this.H;
                --this.eU;
            }
        }
        this.fp = null;
        n8 = (this.cz >> 16) + this.cL;
        n7 = (this.cH >> 16) + this.cL;
        n6 = n7 - n8;
        n5 = this.cB;
        this.fo = 1;
        n4 = this.cF + n5;
        n3 = (this.cF - this.cB << 7) / ((n6 << 7) + 1);
        n6 >>= 1;
        n5 -= n3;
        while (n6 > 0) {
            byArray[n8++] = (byte)((n5 += n3) >>> 16);
            byArray[n7--] = (byte)(n4 - n5 >>> 16);
            --n6;
        }
        byArray[n8] = (byte)((n5 += n3) >>> 16);
        byArray[n7] = (byte)(n4 - n5 >>> 16);
        this.fg = 2;
    }

    private void g() {
        this.cr = new float[1001];
        this.fk = Troisd.c("xMw\bI");
        this.eV += 4004;
        for (int i2 = 0; i2 < 1001; ++i2) {
            this.cr[i2] = (float)(Math.acos((float)i2 / 1000.0f) / 1.5707963267948966);
        }
    }

    private void h() {
        this.cq = new float[this.bh];
        this.eV += 4 * this.bh;
        this.cs = new int[12];
        this.eV += 48;
        this.ct = new int[12];
        this.eV += 48;
        this.cu = new int[12];
        this.eV += 48;
        this.cv = new int[12];
        this.eV += 48;
        this.cw = new int[12];
        this.eV += 48;
        this.cx = new int[12];
        this.eV += 48;
        this.fk = ".";
        this.cX = new int[12];
        this.eV += 48;
        this.cY = new int[12];
        this.eV += 48;
        this.cZ = new int[12];
        this.eV += 48;
        this.da = new int[12];
        this.eV += 48;
        this.db = new int[12];
        this.eV += 48;
        this.dc = new int[12];
        this.eV += 48;
    }

    private void i() {
        this.ck = new int[this.be];
        this.eV += 4 * this.be;
        this.cl = new int[this.be];
        this.eV += 4 * this.be;
        this.cm = new int[this.be];
        this.eV += 4 * this.be;
        this.cn = new int[this.bf];
        this.eV += 4 * this.bf;
        this.co = new int[this.bf];
        this.eV += 4 * this.bf;
        this.fk = Troisd.c("zGS5\u0001TmZ");
        this.fo = (byte)4;
        this.cp = new int[this.bf * 9];
        this.eV += 4 * (this.bf * 9);
        this.bE = new byte[this.H * this.I];
        this.bv = new byte[this.H * this.I];
        this.eV += 1 * (this.H * this.I);
        this.cb = new byte[this.H * this.I];
        this.eV += 1 * (this.H * this.I);
    }

    private void j() {
        this.fk = Troisd.c("zGS5\u0001TmZ");
    }

    private void k() {
        this.fk = Troisd.c("zGS5\u0001TmZ");
    }

    private void l() {
        this.cg = new a();
        this.cg.e = 2;
        this.fk = Troisd.c("zGS5\u0001TmZ");
        this.cg.b = 184;
        this.cg.g = new float[184];
        this.eV += 736;
        this.cg.h = new float[184];
        this.eV += 736;
        this.cg.f = new float[184];
        this.eV += 736;
        this.fk = "?";
        this.cg.c = 182;
        this.cg.i = new int[1260];
        this.eV += 5040;
        this.bI.a(this.cg, 14, 14, 28, 15, 1, 0);
        this.eV += this.bM.a(this.cg);
        this.fg = 0;
    }

    private void m() {
        System.gc();
        this.fk = Troisd.c("ivK,\u0004\u001avP9\u0001Te\u00024\u0007\u001apG!\f\u001avJ)\u001b\u001aaM$\r\u0005");
        switch (this.ft) {
            case 1: {
                this.fi = 2.0f;
                this.fq = false;
                DirectColorModel directColorModel = new DirectColorModel(24, 0xFF0000, 65280, 255);
                this.bF = new MemoryImageSource(this.H, this.I, (ColorModel)directColorModel, this.fO, 0, this.H);
                this.bF.setAnimated(true);
                this.fg = 2;
                break;
            }
            case 0: {
                this.fq = true;
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.cc, this.cd, this.ce);
                this.bF = new MemoryImageSource(this.H, this.I, (ColorModel)indexColorModel, this.bE, 0, this.H);
                this.bF.setAnimated(true);
                this.fj = 0.0;
            }
        }
        this.fp = null;
        this.bF.setFullBufferUpdates(true);
        this.bC = this.createImage(this.bF);
        this.bF.newPixels();
    }

    private void n() {
        System.gc();
        this.fk = Troisd.c("ivK,\u0004\u001avP9\u0001Te\u00024\u0007\u001apG!\f\u001avJ)\u001b\u001aaM$\r\u0005");
        switch (this.ft) {
            case 0: {
                this.fi = 2.0f;
                this.fq = false;
                IndexColorModel indexColorModel = new IndexColorModel(24, 65288, this.cd, this.ce, this.cd);
                this.bF = new MemoryImageSource(this.H, this.I, (ColorModel)indexColorModel, this.bE, 0, this.H);
                this.bF.setAnimated(false);
                break;
            }
            case 1: {
                this.fq = true;
                IndexColorModel indexColorModel = new IndexColorModel(8, 256, this.cc, this.cd, this.ce);
                this.bF = new MemoryImageSource(this.H, this.I, (ColorModel)indexColorModel, this.bE, 0, this.H);
                this.bF.setAnimated(true);
                this.fj = 0.0;
            }
        }
        this.fp = null;
        this.bF.setFullBufferUpdates(true);
        this.bC = this.createImage(this.bF);
        this.bF.newPixels();
        System.gc();
    }

    private boolean o() {
        return this.gp == 1817312555;
    }

    private void a(String string) {
        this.fq = false;
        for (int i2 = 0; i2 < 13; ++i2) {
            this.fo = (byte)i2;
            for (int i3 = 0; i3 < 216; ++i3) {
                this.fg = i3 * i2;
                this.fi = 0.0f;
                if (this.bx[i2 * 216 + i3] != 0) {
                    this.fk = string;
                    this.bE[i2 * this.H + i3 + this.H / 2 - 108 + (this.I / 2 - 7) * this.H] = -1;
                    this.fq = true;
                }
                this.fk = Troisd.c("\u00cc]}%7\u00dd\u00e2");
                this.fq = false;
            }
        }
    }

    private void p() {
        this.fk = Troisd.c("\tf\u0002\u0006\u001dTaV)\u0007Tq\u0002!\u001a_\"E/\u0007^\"D/\u001a\u001a{M5I");
        this.fp = null;
        this.q();
        this.fi = 0.0f;
    }

    private void q() {
        this.fq = true;
        this.r();
        this.fi = 1.57075f;
    }

    private void b(String string) {
        this.fq = true;
        String string2 = string;
        this.r();
        this.fi = 1.57075f;
    }

    private void r() {
        this.fq = false;
        this.fk = Troisd.c("[,ChA");
        this.s();
        this.fj = 0.0;
    }

    private void s() {
        this.fp = null;
    }

    private void t() {
        this.bB.stop();
        if (this.gm == 1) {
            try {
                AudioPlayer.player.stop(this.bi);
            }
            catch (Exception exception) {
                // empty catch block
            }
        }
    }

    private void u() {
        this.fq = false;
        if (this.G == 3 || this.G == 4) {
            this.ch = new a();
            try {
                this.bD = new URL(this.getDocumentBase(), Troisd.c("_g\f:\u0001J"));
            }
            catch (Exception exception) {
                // empty catch block
            }
            this.fi = 0.0f;
            this.eV += this.bL.a(this.bD, Troisd.c("_g\f:\u0001J"), (int)((double)this.H / 3.0), this.ch, this.X, this.W, 1);
            if (this.ch.a) {
                this.eV += this.bM.a(this.ch);
            } else {
                System.out.println(Troisd.c("ycL.\u0007N\"D)\u0006^\"") + String.valueOf(this.bD));
                return;
            }
            this.ci = new a();
            try {
                this.bD = new URL(this.getDocumentBase(), Troisd.c("Ks\f:\u0001J"));
            }
            catch (Exception exception) {
                // empty catch block
            }
            this.fi = 1.0f;
            this.eV += this.bL.a(this.bD, Troisd.c("Ks\f:\u0001J"), (int)((double)this.H / 3.0), this.ci, this.Z, this.Y, 1);
            this.fo = 1;
            if (this.ci.a) {
                this.eV += this.bM.a(this.ci);
            } else {
                System.out.println(Troisd.c("ycL.\u0007N\"D)\u0006^\"") + String.valueOf(this.bD));
                return;
            }
            this.cj = new a();
            try {
                this.bD = new URL(this.getDocumentBase(), Troisd.c("Bz\f:\u0001J"));
            }
            catch (Exception exception) {
                // empty catch block
            }
            this.fi = 2.0f;
            this.eV += this.bL.a(this.bD, Troisd.c("Bz\f:\u0001J"), (int)((double)this.H / 3.0), this.cj, this.bb, this.ba, 1);
            if (this.cj.a) {
                this.eV += this.bM.a(this.cj);
            } else {
                System.out.println(Troisd.c("ycL.\u0007N\"D)\u0006^\"") + String.valueOf(this.bD));
                return;
            }
            System.gc();
        }
        this.cf = new a();
        if (this.G == 4) {
            try {
                this.bD = new URL(this.getDocumentBase(), Troisd.c("RgC$F@kR"));
            }
            catch (Exception exception) {
                // empty catch block
            }
            this.fi = 3.0f;
            this.eV += this.bL.a(this.bD, Troisd.c("RgC$F@kR"), this.I / 2, this.cf, 127, 0, 1);
            this.fi = 2.0f;
            if (this.cf.a) {
                this.eV += this.bM.a(this.cf);
            } else {
                System.out.println(Troisd.c("ycL.\u0007N\"D)\u0006^\"") + String.valueOf(this.bD));
                return;
            }
            this.fo = 0;
        }
        System.gc();
        this.fn[0][0] = 0.0;
        if (this.G == 2) {
            try {
                this.bD = new URL(this.getDocumentBase(), Troisd.c("QlM4\\\u0014xK0"));
            }
            catch (Exception exception) {
                // empty catch block
            }
            this.eV += this.bL.a(this.bD, Troisd.c("QlM4\\\u0014xK0"), this.I / 2, this.cf, 255, 0, 1);
            this.fk = Troisd.c("\u00fe\u00c9\u0001\u009c\u00be^\u00d4\u00d6\u00b6");
            if (this.cf.a) {
                this.eV += this.bM.a(this.cf);
            } else {
                System.out.println(Troisd.c("ycL.\u0007N\"D)\u0006^\"") + String.valueOf(this.bD));
                this.fn[0][0] = 0.0;
                return;
            }
            this.fg = -20;
        }
        this.bA = true;
    }

    private void a(URL uRL, String string, int n2, byte[] byArray) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        int n3 = 6;
        int n4 = 6;
        int n5 = image.getWidth(null);
        int n6 = image.getHeight(null);
        int[] nArray = new int[n5 * n6];
        n2 = 0;
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, n5, n6, nArray, 0, n5);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        this.fk = Troisd.c("\u007fzA%\u0018NkM.");
        int[] nArray2 = new int[n5 * n6];
        for (int i2 = 0; i2 < n5 * n6; ++i2) {
            this.fi = 0.0f;
            nArray2[i2] = nArray[i2];
            if (nArray2[i2] == -16777216) {
                this.fo = (byte)(nArray2[i2] - 1);
                byArray[i2] = 0;
                this.fq = true;
            }
            this.fq = true;
            if (nArray2[i2] == -16777216) continue;
            this.fo = (byte)(nArray2[i2] & 0xFF);
            byArray[i2] = (byte)((nArray2[i2] & 0xFF) / 2 + 128);
            this.fk = Troisd.c("X*\u000b{");
        }
        System.gc();
    }

    private void a(URL uRL, String string, int n2) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        int n3 = 6;
        int n4 = 6;
        int n5 = image.getWidth(null);
        int n6 = image.getHeight(null);
        int[] nArray = new int[n5 * n6];
        n2 = 0;
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, n5, n6, nArray, 0, n5);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        this.fk = Troisd.c("\u007fzA%\u0018NkM.");
        int[] nArray2 = new int[n5 * n6];
        for (int i2 = 0; i2 < n5 * n6; ++i2) {
            this.fi = 0.0f;
            nArray2[i2] = nArray[i2];
            if (nArray2[i2] == -16777216) {
                this.fo = (byte)(nArray2[i2] - 1);
                this.fH[i2] = 0;
                this.fq = true;
            }
            this.fq = true;
            if (nArray2[i2] == -16777216) continue;
            this.fo = (byte)(nArray2[i2] & 0xFF);
            this.fH[i2] = (byte)((nArray2[i2] & 0xFF) / 2 + 128);
            this.fk = Troisd.c("X*\u000b{");
        }
        System.gc();
    }

    private void a(URL uRL, String string, int n2, int[] nArray) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        int n3 = 6;
        int n4 = 6;
        int n5 = image.getWidth(null);
        int n6 = image.getHeight(null);
        n2 = 0;
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, n5, n6, nArray, 0, n5);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        System.gc();
    }

    private void b(URL uRL, String string, int n2) {
        MediaTracker mediaTracker = new MediaTracker(this);
        Image image = this.getImage(uRL, string);
        mediaTracker.addImage(image, 0);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        int n3 = image.getWidth(null);
        int n4 = image.getHeight(null);
        int[] nArray = new int[n3 * n4];
        n2 = 0;
        this.fU = new byte[2408];
        PixelGrabber pixelGrabber = new PixelGrabber(image, 0, 0, n3, n4, nArray, 0, n3);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {
            // empty catch block
        }
        this.fk = Troisd.c("TwN,");
        for (n2 = 0; n2 < 43; ++n2) {
            this.fq = false;
            int[] nArray2 = new int[56];
            for (int i2 = 0; i2 < 56; ++i2) {
                this.fg = i2;
                nArray2[i2] = nArray[i2 + n2 * 7 * 8];
                if (nArray2[i2] == -16777216) {
                    this.fU[i2 + n2 * 7 * 8] = 0;
                    this.fq = true;
                }
                if (nArray2[i2] == -16777216) continue;
                this.fo = (byte)nArray2[i2];
                this.fU[i2 + n2 * 7 * 8] = -1;
                this.fk = Troisd.c("X*\u000b{");
            }
        }
    }

    private void v() {
        boolean bl = false;
        boolean bl2 = false;
        boolean bl3 = false;
        this.fS = 0;
        this.fT = 0;
        this.fk = Troisd.c("\u00de-\u00f4\u00aaG\u00d4]\u00f9");
        block0: for (int i2 = 0; i2 < 94; ++i2) {
            for (int i3 = 0; i3 < 30; ++i3) {
                int n2;
                if ((n2 = this.bR.b.charAt(this.fS++)) == 32) {
                    n2 = 91;
                } else if (n2 == 46) {
                    n2 = 92;
                } else if (n2 == 33) {
                    n2 = 93;
                } else if (n2 == 58) {
                    n2 = 94;
                } else if (n2 == 44) {
                    n2 = 95;
                } else if (n2 == 45) {
                    n2 = 96;
                } else if (n2 == 47) {
                    n2 = 97;
                } else if (n2 > 47 && n2 < 58) {
                    n2 += 50;
                }
                if (n2 == 60) continue block0;
                n2 -= 65;
                this.fk = Troisd.c("[*\u000b{");
                this.fg = 0;
                for (int i4 = 0; i4 < 8; ++i4) {
                    for (int i5 = 0; i5 < 7; ++i5) {
                        this.fI[i2 * 240 * 9 + i3 * 8 + i4 * 240 + i5 + 43200] = this.fU[n2 * 7 * 8 + i4 * 7 + i5];
                    }
                }
            }
        }
        this.fS = 0;
        this.fT = 0;
    }

    private void w() {
        int n2;
        for (n2 = 0; n2 < 29830; ++n2) {
            this.gp += this.fG[n2];
        }
        for (n2 = 0; n2 < 20574; ++n2) {
            this.gp += this.fH[n2];
        }
        for (n2 = 0; n2 < 78400; ++n2) {
            this.gp += this.fM[n2];
            this.gp += this.fN[n2];
            this.gp += this.fO[n2];
        }
        for (n2 = 0; n2 < 8816; ++n2) {
            this.gp += this.fR[n2];
        }
        this.gp = 1817312555;
    }

    public void x() {
        this.bP.a(this.gb, this.em);
        this.ca = 0;
        if (this.G == 4) {
            int n2;
            this.r();
            this.s();
            this.fo = 1;
            this.bJ.a(this.cc, this.cd, this.ce, this.W, this.X, 80, 50, 80, 80, 50, 30, 255, 255, 255, 7);
            for (n2 = 1; n2 < 20; ++n2) {
                this.fg = 1;
                float f2 = n2;
                this.cc[n2] = (byte)(f2 * 80.0f / 20.0f);
                this.cd[n2] = (byte)(f2 * 50.0f / 20.0f);
                this.ce[n2] = (byte)(f2 * 80.0f / 20.0f);
            }
            for (n2 = 128; n2 < 256; ++n2) {
                this.fg = 1;
                this.cc[n2] = (byte)((n2 - 128) * 2);
                this.cd[n2] = (byte)((n2 - 128) * 2);
                this.ce[n2] = (byte)((n2 - 128) * 2);
            }
            for (n2 = 0; n2 < 250; ++n2) {
                this.fC[n2] = 500 - n2 * 2;
            }
            for (n2 = 0; n2 < 280; ++n2) {
                this.fE[n2] = -16;
            }
            for (n2 = 0; n2 < 29830; ++n2) {
                this.fG[n2] = (byte)(Math.random() * 127.0 + 128.0);
            }
            this.ca = 10;
            this.y();
            this.a(this.getDocumentBase(), Troisd.c("KwC4\u001a_,E)\u000e"), 1, this.fG);
            this.ca = 20;
            this.y();
            this.a(this.getDocumentBase(), Troisd.c("IcQn\u000fSd"), 1, this.fM);
            this.ca = 45;
            this.y();
            this.a(this.getDocumentBase(), Troisd.c("UtG2\u0000_cFn\u000fSd"), 1);
            this.ca = 50;
            this.y();
            this.a(this.getDocumentBase(), Troisd.c("SlV2\u0007erK#FPrE"), 1, this.fN);
            this.ca = 65;
            this.y();
            this.a(this.getDocumentBase(), Troisd.c("SlV2\u0007evG8\u001c\u0014eK&"), 1, this.fR);
            this.ca = 70;
            this.y();
            this.b(this.getDocumentBase(), Troisd.c("\\mL4\rI5\u001an\u000fSd"), 1);
            this.ca = 75;
            this.y();
            this.z();
            this.bP.a(this.fV, 280, 280);
            for (n2 = 0; n2 < 140; ++n2) {
                this.fP[n2] = 280;
                this.fP[n2 + 140] = (int)(Math.cos(0.022439929f * (float)n2) * 140.0 + 140.0);
                if (this.fP[n2 + 140] < 0) {
                    this.fP[n2 + 140] = 0;
                }
                if (this.fP[n2 + 140] > 280) {
                    this.fP[n2 + 140] = 280;
                }
                this.fP[n2 + 140 + 140] = 0;
            }
            this.fQ = 0;
            this.v();
        }
        this.ca = 80;
        this.y();
        this.u();
        this.ca = 100;
        this.y();
        this.w();
        if (this.gm == 1) {
            this.a(this.fO, this.em);
            this.b();
            this.b(this.a);
        }
        this.bP.a(this.fO, 20);
        this.fW = 6L;
        this.fX = 1L;
        o.b = 1;
        o.c = 1;
        this.fF = -1342;
        this.gj = 1;
        this.go = 0;
    }

    private void y() {
        if (this.gj == 0) {
            for (int i2 = 0; i2 < 11; ++i2) {
                System.arraycopy(this.gk, 0, this.fO, i2 * 280 + this.H / 4 + 50 + (this.I / 2 + 10 + 45) * 280, (int)((float)this.ca * 1.4f) + 1);
            }
            this.showStatus(Troisd.c("vmC$\u0001Te\u0002!\u0006^\"F%\u0018[aI)\u0006]\"F!\u001c[q\u0002h") + this.ca + Troisd.c("\u001f+"));
        }
    }

    private void b(int n2) {
        int n3 = (int)((float)n2 * 100.0f / (float)this.a);
        if (this.gj == 0) {
            for (int i2 = 0; i2 < 11; ++i2) {
                System.arraycopy(this.gl, 0, this.fO, i2 * 280 + this.H / 4 + 50 + (this.I / 2 + 10 + 45) * 280, (int)((float)n3 * 1.4f) + 1);
            }
            if (n3 % 2 == 0) {
                this.showStatus(Troisd.c("vmC$\u0001Te\u0002!\u0006^\"F%\u0018[aI)\u0006]\"O5\u001bSa\u0002h") + n3 + Troisd.c("\u001f+"));
            }
        }
    }

    private void a(int[] nArray, byte[] byArray) {
        int n2 = 0;
        for (int i2 = 0; i2 < 6; ++i2) {
            for (int i3 = 0; i3 < 91; ++i3) {
                if (byArray[n2++] == 0) continue;
                nArray[(i2 + 197 - 13) * this.H + i3 + 145] = -1;
            }
        }
    }

    private int a(String string, int n2) {
        return string.charAt(n2);
    }

    private void z() {
    }

    public Troisd() {
        this.fk = Troisd.c("\u007fsW)\u0006Uz\u00022\u001dVgQa");
    }

    /*
     * Handled impossible loop by adding 'first' condition
     * Enabled aggressive block sorting
     */
    private static String c(String string) {
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
                        n5 = 58;
                        break;
                    }
                    case 1: {
                        n5 = 2;
                        break;
                    }
                    case 2: {
                        n5 = 34;
                        break;
                    }
                    case 3: {
                        n5 = 64;
                        break;
                    }
                    default: {
                        n5 = 104;
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

