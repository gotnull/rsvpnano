/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Color;
import java.awt.Event;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.image.MemoryImageSource;
import java.awt.image.PixelGrabber;
import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.zip.ZipInputStream;

public class Quaternions
extends Applet
implements Runnable {
    public int k;
    public Image l;
    public Image s;
    public Image G;
    public Image H;
    public Image I;
    public Image J;
    public Image K;
    public Image L;
    public Image M;
    public Image N;
    public Image O;
    public Image P;
    public Image Q;
    public boolean R;
    public int S;
    public Image[] T;
    public Image U;
    public Image V;
    public Image W;
    public Image X;
    public Image Y;
    public Image Z;
    public Image ba;
    public Image bb;
    public Image bc;
    public int bd = 1;
    public int bg;
    public int bi;
    public int bj;
    public int bk;
    public long bl;
    public Thread bn;
    public Image bo;
    public Graphics bp;
    public boolean cp;
    public int cr;
    public int cs;
    public long ct;
    public b cx;
    public b[] cy;
    public a cz;
    public int cA;
    public int[] cB;
    public int[] cC;
    public int[] cD;
    public int[] cE;

    public final void init() {
        this.k = 102;
        this.bn = null;
        this.setLayout(null);
        this.resize(440, 530);
        this.cB = new int[7];
        this.cC = new int[7];
        this.cD = new int[7];
        this.cE = new int[7];
        this.cB[0] = 70;
        this.cC[0] = 140;
        this.cB[1] = 80;
        this.cC[1] = 140;
        this.cB[2] = 80;
        this.cC[2] = 130;
        this.cB[3] = 99;
        this.cC[3] = 145;
        this.cB[4] = 80;
        this.cC[4] = 160;
        this.cB[5] = 80;
        this.cC[5] = 150;
        this.cB[6] = 70;
        this.cC[6] = 150;
        this.cD[0] = 40;
        this.cE[0] = 140;
        this.cD[1] = 30;
        this.cE[1] = 140;
        this.cD[2] = 30;
        this.cE[2] = 130;
        this.cD[3] = 11;
        this.cE[3] = 145;
        this.cD[4] = 30;
        this.cE[4] = 160;
        this.cD[5] = 30;
        this.cE[5] = 150;
        this.cD[6] = 40;
        this.cE[6] = 150;
        URL uRL = this.getCodeBase();
        URL uRL2 = this.getDocumentBase();
        String string = uRL.getHost();
        String string2 = uRL2.getHost();
        this.bo = this.createImage(440, 530);
        this.bp = this.bo.getGraphics();
        this.bp.setColor(Color.black);
        this.bp.fillRect(0, 0, 440, 530);
        this.bp.setColor(new Color(77, 92, 189));
        int n = 0;
        while (n < 530) {
            this.bp.fillRect(0, n, 440, 1);
            n += 2;
        }
        this.R = false;
        this.l = this.getImage(this.getCodeBase(), Quaternions.u("Qk\u007flh\u0004*5p?]|"));
        this.s = this.getImage(this.getCodeBase(), Quaternions.u("Rkr?,\u001a}n8"));
        this.G = this.getImage(this.getCodeBase(), Quaternions.u("@ur*vSsa"));
        this.U = this.getImage(this.getCodeBase(), Quaternions.u("Wui-7X\u007f)91R"));
        this.V = this.getImage(this.getCodeBase(), Quaternions.u("Wui-7X\u007ft69P\u007f)91R"));
        this.bb = this.getImage(this.getCodeBase(), Quaternions.u("Zoj<=Fi)91R"));
        this.bc = this.getImage(this.getCodeBase(), Quaternions.u("@ss2=\u001a}n8"));
        if (string.equals(Quaternions.u("Qkr76[b).4Utb*uP4i;,"))) {
            if (string2.equals(Quaternions.u("Qkr76[b).4Utb*uP4i;,"))) {
                InputStream inputStream = null;
                DataInputStream dataInputStream = null;
                try {
                    URL uRL3 = new URL(this.getCodeBase(), Quaternions.u("Gyu7(@4a,9Wnf2+\u001a{u="));
                    inputStream = uRL3.openStream();
                    if (inputStream != null) {
                        ZipInputStream zipInputStream = null;
                        zipInputStream = new ZipInputStream(inputStream);
                        zipInputStream.getNextEntry();
                        dataInputStream = new DataInputStream(zipInputStream);
                        this.cr = dataInputStream.readInt();
                        this.cy = new b[this.cr];
                        int n2 = 0;
                        while (n2 < this.cr) {
                            this.cy[n2] = new b();
                            this.cy[n2].u(dataInputStream.readInt());
                            this.cy[n2].u(dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat());
                            this.cy[n2].u(dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readInt());
                            this.cy[n2].u(dataInputStream.readInt(), dataInputStream.readInt());
                            int n3 = dataInputStream.readInt();
                            int n4 = 0;
                            while (n4 < n3) {
                                this.cy[n2].t(dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat(), dataInputStream.readFloat());
                                ++n4;
                            }
                            System.gc();
                            ++n2;
                        }
                        if (dataInputStream != null) {
                            dataInputStream.close();
                        }
                    }
                }
                catch (Exception exception) {
                    // empty catch block
                }
                try {
                    if (inputStream != null) {
                        inputStream.close();
                    }
                }
                catch (Exception exception) {
                    // empty catch block
                }
            }
        }
        MediaTracker mediaTracker = new MediaTracker(this);
        mediaTracker.addImage(this.l, 0);
        mediaTracker.addImage(this.s, 1);
        mediaTracker.addImage(this.G, 2);
        mediaTracker.addImage(this.U, 3);
        mediaTracker.addImage(this.V, 4);
        mediaTracker.addImage(this.bb, 5);
        mediaTracker.addImage(this.bc, 6);
        try {
            mediaTracker.waitForAll();
        }
        catch (InterruptedException interruptedException) {
            System.out.println(Quaternions.u("@us1i"));
        }
        this.s = this.u(this.s, 0, 0, 157, 12);
        this.u(this.G, 0, 0, 123, 13);
        this.u(this.G, 0, 13, 77, 14);
        this.u(this.G, 25, 71, 26, 31);
        this.u(this.G, 76, 71, 26, 31);
        this.u(this.G, 51, 71, 25, 31);
        this.u(this.G, 0, 71, 25, 31);
        this.H = this.u(this.bb, 0, 0, 11, 11);
        this.I = this.u(this.bb, 11, 0, 11, 11);
        this.J = this.u(this.bb, 22, 0, 11, 11);
        this.K = this.u(this.bb, 33, 0, 11, 11);
        this.L = this.u(this.bb, 44, 0, 11, 11);
        this.M = this.u(this.bb, 55, 0, 11, 11);
        this.N = this.u(this.bb, 66, 0, 11, 11);
        this.O = this.u(this.bb, 77, 0, 11, 11);
        this.P = this.u(this.bb, 88, 0, 11, 11);
        this.Q = this.u(this.bb, 99, 0, 11, 11);
        this.W = this.u(this.V, 0, 0, 72, 25);
        this.X = this.u(this.V, 72, 0, 72, 25);
        this.Y = this.u(this.V, 144, 0, 72, 25);
        this.Z = this.u(this.V, 216, 0, 72, 25);
        this.ba = this.u(this.V, 288, 0, 72, 25);
        this.S = (int)(Math.log(this.cr) / Math.log(10.0)) + 1;
        this.T = new Image[10];
        this.T[0] = this.H;
        this.T[1] = this.I;
        this.T[2] = this.J;
        this.T[3] = this.K;
        this.T[4] = this.L;
        this.T[5] = this.M;
        this.T[6] = this.N;
        this.T[7] = this.O;
        this.T[8] = this.P;
        this.T[9] = this.Q;
        this.cp = false;
        System.gc();
        this.cx = new b(440);
        this.cs = 0;
        this.cx.u((byte)77, (byte)92, (byte)-67, (byte)0, (byte)0, (byte)0);
        this.cx.t(this.k);
        this.cx.u(this.cy[this.cs].aq, this.cy[this.cs].ap, this.cy[this.cs].ao, this.cy[this.cs].an, this.cy[this.cs].am, this.cy[this.cs].al);
        this.cx.u(this.cy[this.cs].av, this.cy[this.cs].au, this.cy[this.cs].at, this.cy[this.cs].as, this.cy[this.cs].ar, this.cy[this.cs].ai);
        this.cx.u(this.cy[this.cs].ad);
        this.cx.u(this.cy[this.cs].ak, this.cy[this.cs].aj);
        int n5 = 0;
        while (n5 < this.cy[this.cs].a1) {
            this.cx.t(this.cy[this.cs].a_[n5], this.cy[this.cs].az[n5], this.cy[this.cs].a0[n5], this.cy[this.cs].ay[n5], this.cy[this.cs].ax[n5], this.cy[this.cs].aw[n5]);
            ++n5;
        }
        this.cz = new a(440);
        this.cz.u(this.cx, 440);
        this.cA = 0;
        this.cp = false;
        this.bi = 0;
        this.bj = 0;
        this.bg = -1;
        this.bk = 1;
        this.bl = this.ct = System.currentTimeMillis();
        this.bn = null;
    }

    public final void start() {
        if (this.bn == null) {
            this.cz.u(this.cx, 440);
            this.bn = new Thread(this);
            this.bn.start();
            this.cz.al();
        }
    }

    public final void stop() {
        if (this.bn != null) {
            this.cz.ak();
            this.bn = null;
            this.bn.stop();
        }
    }

    public final void destroy() {
    }

    /*
     * Unable to fully structure code
     */
    public final void run() {
        var4_1 = a.a2;
        block6: while (Thread.currentThread() == this.bn) {
            this.repaint();
            try {
                Thread.sleep(20L);
            }
            catch (InterruptedException var1_3) {
                // empty catch block
            }
            do {
                block58: {
                    block60: {
                        block63: {
                            block62: {
                                block61: {
                                    block59: {
                                        if (System.currentTimeMillis() - this.bl > 1000L) {
                                            this.bl = System.currentTimeMillis();
                                            this.bk = 1;
                                        }
                                        if (!this.R) {
                                            this.bp.drawImage(this.bc, 0, 0, this);
                                            this.bp.drawImage(this.U, 0, 454, this);
                                            if (this.bd == 0) {
                                                this.bp.drawImage(this.W, 80, 454, this);
                                            }
                                            if (this.bd == 1) {
                                                this.bp.drawImage(this.X, 152, 454, this);
                                            }
                                            if (this.bd == 2) {
                                                this.bp.drawImage(this.Y, 224, 454, this);
                                            }
                                            if (this.bd == 3) {
                                                this.bp.drawImage(this.Z, 296, 454, this);
                                            }
                                            if (this.bd == 4) {
                                                this.bp.drawImage(this.ba, 368, 454, this);
                                            }
                                            this.u(this.bp, this.S, this.cr, this.T);
                                            this.R = true;
                                        }
                                        var1_2 = false;
                                        if (this.bg == 6 && this.cs < this.cr - 1) {
                                            ++this.cs;
                                            var1_2 = true;
                                            this.bg = -1;
                                        }
                                        if (this.bg == 5 && this.cs > 0) {
                                            --this.cs;
                                            var1_2 = true;
                                            this.bg = -1;
                                        }
                                        if (this.bg == 8 && this.cs < this.cr - 1) {
                                            this.cs += 10;
                                            if (this.cs >= this.cr) {
                                                this.cs = this.cr - 1;
                                            }
                                            var1_2 = true;
                                            this.bg = -1;
                                        }
                                        if (this.bg == 7 && this.cs > 0) {
                                            this.cs -= 10;
                                            if (this.cs < 0) {
                                                this.cs = 0;
                                            }
                                            var1_2 = true;
                                            this.bg = -1;
                                        }
                                        if (!var1_2) break block58;
                                        this.cz.ak();
                                        if (var4_1 == 0) ** GOTO lbl65
                                        block8: while (true) {
                                            try {
                                                Thread.sleep(10L);
                                            }
                                            catch (InterruptedException var2_5) {
                                                // empty catch block
                                            }
lbl65:
                                            // 3 sources

                                            do {
                                                if (this.cz.t()) continue block8;
                                                this.bp.setColor(new Color(77, 92, 189));
                                                var2_4 = 14;
                                            } while (var4_1 != 0 || var4_1 != 0 && var4_1 != 0);
                                            break;
                                        }
                                        block10: while (var2_4 < 515) {
                                            this.bp.fillRect(0, var2_4, 452, 1);
                                            while (true) {
                                                var2_4 += 2;
                                                continue block10;
                                                break;
                                            }
                                        }
                                        this.bp.setColor(new Color(0, 0, 0));
                                        var2_4 = 14;
                                        if (var4_1 != 0) ** continue;
                                        if (var4_1 == 0) ** GOTO lbl83
                                        block12: while (true) {
                                            this.bp.fillRect(0, var2_4 + 1, 452, 1);
                                            do {
                                                var2_4 += 2;
lbl83:
                                                // 2 sources

                                                if (var2_4 < 515) continue block12;
                                                System.gc();
                                                this.cA = 0;
                                                this.cp = false;
                                                this.bp.setColor(new Color(77, 92, 189));
                                                var2_4 = 0;
                                            } while (var4_1 != 0 && var4_1 != 0);
                                            break;
                                        }
                                        block14: while (var2_4 < 16) {
                                            this.bp.fillRect(0, var2_4 + 250 - 34, 150, 1);
                                            while (true) {
                                                var2_4 += 2;
                                                continue block14;
                                                break;
                                            }
                                        }
                                        this.bp.setColor(new Color(0, 0, 0));
                                        var2_4 = 0;
                                        if (var4_1 != 0) ** continue;
                                        if (var4_1 == 0) ** GOTO lbl104
                                        block16: while (true) {
                                            v0 = this;
                                            do {
                                                v0.bp.fillRect(0, var2_4 + 250 - 34 + 1, 150, 1);
                                                var2_4 += 2;
lbl104:
                                                // 2 sources

                                                if (var2_4 < 16) continue block16;
                                                this.bp.drawImage(this.U, 0, 454, this);
                                                this.u(this.bp, this.S, this.cr, this.T);
                                                v0 = this;
                                            } while (var4_1 != 0);
                                            break;
                                        }
                                        if (v0.bd != 0) break block59;
                                        this.bp.drawImage(this.W, 80, 454, this);
                                        if (var4_1 == 0) break block60;
                                    }
                                    if (this.bd != 1) break block61;
                                    this.bp.drawImage(this.X, 152, 454, this);
                                    if (var4_1 == 0) break block60;
                                }
                                if (this.bd != 2) break block62;
                                this.bp.drawImage(this.Y, 224, 454, this);
                                if (var4_1 == 0) break block60;
                            }
                            if (this.bd != 3) break block63;
                            this.bp.drawImage(this.Z, 296, 454, this);
                            if (var4_1 == 0) break block60;
                        }
                        if (this.bd == 4) {
                            this.bp.drawImage(this.ba, 368, 454, this);
                        }
                    }
                    this.bg = -1;
                    this.cx.ab = false;
                    this.cx.ac = 0;
                    this.cx.ag = 0;
                    this.cA = 0;
                    this.cp = false;
                    this.bi = 0;
                    this.bj = 0;
                    this.bk = 1;
                    this.cx.u((byte)77, (byte)92, (byte)-67, (byte)0, (byte)0, (byte)0);
                    this.cx.t(this.k);
                    this.cx.u(this.cy[this.cs].aq, this.cy[this.cs].ap, this.cy[this.cs].ao, this.cy[this.cs].an, this.cy[this.cs].am, this.cy[this.cs].al);
                    this.cx.u(this.cy[this.cs].av, this.cy[this.cs].au, this.cy[this.cs].at, this.cy[this.cs].as, this.cy[this.cs].ar, this.cy[this.cs].ai);
                    this.cx.u(this.cy[this.cs].ad);
                    this.cx.u(this.cy[this.cs].ak, this.cy[this.cs].aj);
                    var2_4 = 0;
                    if (var4_1 == 0) ** GOTO lbl156
                    block18: while (true) {
                        v1 = this;
                        do {
                            v1.cx.t(this.cy[this.cs].a_[var2_4], this.cy[this.cs].az[var2_4], this.cy[this.cs].a0[var2_4], this.cy[this.cs].ay[var2_4], this.cy[this.cs].ax[var2_4], this.cy[this.cs].aw[var2_4]);
                            ++var2_4;
lbl156:
                            // 2 sources

                            if (var2_4 < this.cy[this.cs].a1) continue block18;
                            this.cA = 0;
                            this.cp = false;
                            this.bi = 0;
                            this.bj = 0;
                            this.bk = 1;
                            this.cx.ag = 0;
                            this.cx.ab = false;
                            this.cz.u(this.cx, 440);
                            this.cz.al();
                            v1 = this;
                        } while (var4_1 != 0);
                        break;
                    }
                    v1.ct = System.currentTimeMillis();
                }
                if (this.bg < 0 || this.bg > 4) ** GOTO lbl283
                this.cz.ak();
                if (var4_1 == 0) ** GOTO lbl179
                block20: while (true) {
                    try {
                        Thread.sleep(10L);
                    }
                    catch (InterruptedException var2_6) {
                        // empty catch block
                    }
lbl179:
                    // 3 sources

                    v2 = this.cz.t();
                    do {
                        if (v2 != 0) continue block20;
                        v2 = this.bg;
                    } while (var4_1 != 0);
                    break;
                }
                if (v2 == 0) {
                    this.cx.ae = 50;
                    this.k = 50;
                }
                if (this.bg == 1) {
                    this.cx.ae = 102;
                    this.k = 102;
                }
                if (this.bg == 2) {
                    this.cx.ae = 202;
                    this.k = 202;
                }
                if (this.bg == 3) {
                    this.cx.ae = 302;
                    this.k = 302;
                }
                if (this.bg != 4) ** GOTO lbl201
                v3 = this.cx;
                do {
                    block65: {
                        block68: {
                            block67: {
                                block66: {
                                    block64: {
                                        v3.ae = 450;
                                        this.k = 450;
lbl201:
                                        // 2 sources

                                        this.bp.setColor(new Color(77, 92, 189));
                                        var2_4 = 14;
                                        if (var4_1 == 0) ** GOTO lbl208
                                        block23: while (true) {
                                            this.bp.fillRect(0, var2_4, 452, 1);
                                            do {
                                                var2_4 += 2;
lbl208:
                                                // 2 sources

                                                if (var2_4 < 515) continue block23;
                                                this.bp.setColor(new Color(0, 0, 0));
                                                var2_4 = 14;
                                            } while (var4_1 != 0 && var4_1 != 0);
                                            break;
                                        }
                                        block25: while (var2_4 < 515) {
                                            v4 = this;
                                            while (true) {
                                                v4.bp.fillRect(0, var2_4 + 1, 452, 1);
                                                var2_4 += 2;
                                                continue block25;
                                                break;
                                            }
                                        }
                                        this.bp.drawImage(this.U, 0, 454, this);
                                        v4 = this;
                                        if (var4_1 != 0) ** continue;
                                        if (v4.bd != 0) break block64;
                                        this.bp.drawImage(this.W, 80, 454, this);
                                        if (var4_1 == 0) break block65;
                                    }
                                    if (this.bd != 1) break block66;
                                    this.bp.drawImage(this.X, 152, 454, this);
                                    if (var4_1 == 0) break block65;
                                }
                                if (this.bd != 2) break block67;
                                this.bp.drawImage(this.Y, 224, 454, this);
                                if (var4_1 == 0) break block65;
                            }
                            if (this.bd != 3) break block68;
                            this.bp.drawImage(this.Z, 296, 454, this);
                            if (var4_1 == 0) break block65;
                        }
                        if (this.bd == 4) {
                            this.bp.drawImage(this.ba, 368, 454, this);
                        }
                    }
                    this.u(this.bp, this.S, this.cr, this.T);
                    this.bg = -1;
                    System.gc();
                    this.cx.ab = false;
                    this.cx.ac = 0;
                    this.cx.ag = 0;
                    this.cA = 0;
                    this.cp = false;
                    this.bi = 0;
                    this.bj = 0;
                    this.bk = 1;
                    this.cx.u((byte)77, (byte)92, (byte)-67, (byte)0, (byte)0, (byte)0);
                    this.cx.t(this.k);
                    this.cx.u(this.cy[this.cs].aq, this.cy[this.cs].ap, this.cy[this.cs].ao, this.cy[this.cs].an, this.cy[this.cs].am, this.cy[this.cs].al);
                    this.cx.u(this.cy[this.cs].av, this.cy[this.cs].au, this.cy[this.cs].at, this.cy[this.cs].as, this.cy[this.cs].ar, this.cy[this.cs].ai);
                    this.cx.u(this.cy[this.cs].ad);
                    this.cx.u(this.cy[this.cs].ak, this.cy[this.cs].aj);
                    var2_4 = 0;
                    if (var4_1 == 0) ** GOTO lbl270
                    block27: while (true) {
                        v5 = this;
                        do {
                            v5.cx.t(this.cy[this.cs].a_[var2_4], this.cy[this.cs].az[var2_4], this.cy[this.cs].a0[var2_4], this.cy[this.cs].ay[var2_4], this.cy[this.cs].ax[var2_4], this.cy[this.cs].aw[var2_4]);
                            ++var2_4;
lbl270:
                            // 2 sources

                            if (var2_4 < this.cy[this.cs].a1) continue block27;
                            this.cA = 0;
                            this.cp = false;
                            this.bi = 0;
                            this.bj = 0;
                            this.bk = 1;
                            this.cx.ag = 0;
                            this.cx.ab = false;
                            this.cz.u(this.cx, 440);
                            this.cz.al();
                            v5 = this;
                        } while (var4_1 != 0);
                        break;
                    }
                    v5.ct = System.currentTimeMillis();
lbl283:
                    // 2 sources

                    if (this.cp) continue block6;
                    v3 = this.cx;
                } while (var4_1 != 0);
                if (v3 == null) continue block6;
                this.bp = this.bo.getGraphics();
                var2_4 = (this.cx.af - this.cx.ae) / 2 + this.cA;
            } while (var4_1 != 0);
            if (var4_1 == 0) ** GOTO lbl303
            block29: while (true) {
                v6 = this.cx.af - this.cx.ae;
                v7 = 2;
                do {
                    var3_7 = v6 / v7;
                    if (var4_1 == 0) ** GOTO lbl301
                    do {
                        this.bp.setColor(new Color(this.cx.ah[var2_4 * 440 * 3 + var3_7 * 3] & 255, this.cx.ah[var2_4 * 440 * 3 + var3_7 * 3 + 1] & 255, this.cx.ah[var2_4 * 440 * 3 + var3_7 * 3 + 2] & 255));
                        this.bp.fillRect(var3_7, var2_4 + 15, 1, 1);
                        ++var3_7;
lbl301:
                        // 2 sources

                    } while (var3_7 < (this.cx.af - this.cx.ae) / 2 + this.cx.ae);
                    ++var2_4;
lbl303:
                    // 2 sources

                    if (var2_4 < (this.cx.af - this.cx.ae) / 2 + this.cx.ac) continue block29;
                    this.bi = this.cA = this.cx.ac;
                    v6 = (int)this.cx.ab;
                    v7 = 1;
                } while (var4_1 != 0);
                break;
            }
            if (v6 == v7) {
                System.currentTimeMillis();
                this.cp = true;
            }
            this.showStatus(Quaternions.u("dhh9*Qitdx") + String.valueOf(this.cx.ag) + Quaternions.u("\u0014?"));
            this.bp.setColor(new Color(0, 255, 0));
            this.bp.fillRect(88, 512, this.cx.ag * 344 / 100 + 1, 9);
        }
    }

    public final void update(Graphics graphics) {
        if (this.bn != null) {
            if (this.bk == 1) {
                graphics.drawImage(this.bo, 0, 0, 440, 530, this);
                this.bk = 0;
            } else {
                int n = this.bi;
                graphics.drawImage(this.bo, (this.cx.af - this.cx.ae) / 2, 15 + (this.cx.af - this.cx.ae) / 2 + this.bj, (this.cx.af - this.cx.ae) / 2 + this.cx.ae, 15 + (this.cx.af - this.cx.ae) / 2 + n, (this.cx.af - this.cx.ae) / 2, 15 + (this.cx.af - this.cx.ae) / 2 + this.bj, (this.cx.af - this.cx.ae) / 2 + this.cx.ae, 15 + (this.cx.af - this.cx.ae) / 2 + n, this);
                this.bj = n;
            }
        }
    }

    private final void u(Graphics graphics, int n, int n2, Image[] imageArray) {
        int n3;
        int n4 = 1;
        int n5 = 0;
        while (n5 < n) {
            n4 *= 10;
            ++n5;
        }
        n5 = 0;
        while (n5 < n) {
            n3 = (this.cs + 1) % n4 / (n4 / 10);
            n4 /= 10;
            graphics.drawImage(imageArray[n3], 241 + n5 * 10 - 7, 485, this);
            ++n5;
        }
        n4 = 1;
        n5 = 0;
        while (n5 < n) {
            n4 *= 10;
            ++n5;
        }
        n5 = 0;
        while (n5 < n) {
            n3 = n2 % n4 / (n4 / 10);
            n4 /= 10;
            graphics.drawImage(imageArray[n3], 241 + n5 * 10 + n * 10 + 5, 485, this);
            ++n5;
        }
    }

    public final boolean mouseDown(Event event, int n, int n2) {
        this.bg = -1;
        if (n2 > 454 && n2 < 479) {
            if (n > 80 && n < 150) {
                this.bg = 0;
                this.bd = 0;
            } else if (n > 150 && n < 220) {
                this.bg = 1;
                this.bd = 1;
            } else if (n > 220 && n < 290) {
                this.bg = 2;
                this.bd = 2;
            } else if (n > 290 && n < 360) {
                this.bg = 3;
                this.bd = 3;
            } else if (n > 360) {
                this.bg = 4;
                this.bd = 4;
            }
        } else if (n2 > 479 && n2 < 504) {
            if (n > 80 && n < 150) {
                this.bg = 7;
            } else if (n > 150 && n < 220) {
                this.bg = 5;
            } else if (n > 290 && n < 360) {
                this.bg = 6;
            } else if (n > 360) {
                this.bg = 8;
            }
        }
        return true;
    }

    public final boolean mouseMove(Event event, int n, int n2) {
        if (n > 0 && n < 70) {
            if (n2 <= 60 && n2 >= 49) {
            }
            if (n2 <= 78 && n2 >= 67) {
            }
            if (n2 <= 96 && n2 >= 85) {
            }
            if (n2 <= 114 && n2 >= 103) {
            }
            if (n2 <= 132 && n2 >= 121) {
            }
        }
        if (n2 < 207 && n2 > 176) {
            if (n < 37 && n > 12) {
            }
            if (n < 88 && n > 62) {
            }
        }
        if (n2 < 237 && n2 > 206) {
            if (n < 37 && n > 12) {
            }
            if (n < 88 && n > 62) {
            }
        }
        return true;
    }

    private final Image u(Image image, int n, int n2, int n3, int n4) {
        int[] nArray = new int[n3 * n4];
        PixelGrabber pixelGrabber = new PixelGrabber(image, n, n2, n3, n4, nArray, 0, n3);
        try {
            pixelGrabber.grabPixels();
        }
        catch (InterruptedException interruptedException) {
            System.out.println(interruptedException);
        }
        return this.createImage(new MemoryImageSource(n3, n4, nArray, 0, n3));
    }

    public Quaternions() {
        int[] cfr_ignored_0 = new int[20];
        double[][] cfr_ignored_1 = new double[4][10];
    }

    private static String u(String string) {
        char[] cArray = string.toCharArray();
        int n = cArray.length;
        int n2 = 0;
        while (n2 < n) {
            int n3;
            int n4 = n2;
            char c = cArray[n4];
            switch (n2 % 5) {
                case 0: {
                    n3 = 52;
                    break;
                }
                case 1: {
                    n3 = 26;
                    break;
                }
                case 2: {
                    n3 = 7;
                    break;
                }
                case 3: {
                    n3 = 94;
                    break;
                }
                default: {
                    n3 = 88;
                }
            }
            cArray[n4] = (char)(c ^ n3);
            ++n2;
        }
        return new String(cArray);
    }
}

