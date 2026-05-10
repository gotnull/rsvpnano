/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  sun.audio.AudioData
 *  sun.audio.AudioPlayer
 *  sun.audio.ContinuousAudioDataStream
 */
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import sun.audio.AudioData;
import sun.audio.AudioPlayer;
import sun.audio.ContinuousAudioDataStream;

public class a
implements Runnable {
    public String w;
    public String v;
    public long u;
    public long t;
    public short s;
    public long r;
    public short q;
    public long p;
    public byte o;
    public String n;
    public String m;
    public String l;
    public InputStream k = null;
    public boolean h = false;
    public int g;
    public Thread f = null;
    public InputStream e = null;
    public boolean d;

    public final synchronized int i() {
        return this.g;
    }

    public final synchronized void h() {
        if (this.k != null && !this.h) {
            this.h = true;
            AudioPlayer.player.start(this.k);
        }
    }

    public final synchronized void g() {
        this.f = null;
        if (this.k != null && this.h) {
            this.h = false;
            AudioPlayer.player.stop(this.k);
        }
    }

    public final synchronized void f(URL uRL, boolean bl) throws IOException {
        this.f(uRL.openStream(), bl);
    }

    public final synchronized void f(InputStream inputStream, boolean bl) {
        if (this.f != null) {
            return;
        }
        this.g = 0;
        this.d = bl;
        this.e = inputStream;
        this.f = new Thread(this);
        this.f.start();
    }

    public final void run() {
        if (this.e == null) {
            return;
        }
        try {
            this.d(this.e, this.d);
        }
        catch (IOException iOException) {
            iOException.printStackTrace();
        }
        a a2 = this;
        synchronized (a2) {
            this.e = null;
            this.f = null;
        }
        System.gc();
    }

    public final synchronized boolean e() {
        return this.f != null;
    }

    private final void d(InputStream inputStream, boolean bl) throws IOException {
        ZipInputStream zipInputStream = new ZipInputStream(inputStream);
        if (zipInputStream == null) {
            return;
        }
        ZipEntry zipEntry = zipInputStream.getNextEntry();
        if (zipEntry == null) {
            return;
        }
        System.out.println(zipEntry.getName());
        long l = zipEntry.getSize();
        System.out.println("size=" + l);
        if (l < 0L) {
            return;
        }
        byte[] byArray = new byte[(int)l];
        int n = 0;
        int n2 = 0;
        while (zipInputStream.available() > 0 && l > 0L) {
            n2 = zipInputStream.read(byArray, n, (int)l);
            n += n2;
            l -= (long)n2;
            try {
                Thread.yield();
                Thread.sleep(10L);
            }
            catch (InterruptedException interruptedException) {
                // empty catch block
            }
        }
        zipInputStream.close();
        int n3 = this.c(byArray);
        this.b();
        int n4 = 44100 / this.q;
        int n5 = 8000 / this.q;
        short[] sArray = new short[n4];
        byte[] byArray2 = new byte[(int)(this.u * (long)n5)];
        int n6 = 0;
        b b2 = new b();
        b2.u();
        double d2 = (double)n4 / (double)n5;
        System.out.println("ratio:" + d2);
        System.out.println("Generating muLaw datas...");
        System.out.println("0__________100%");
        System.out.print(" ");
        double d3 = 0.0;
        int n7 = 0;
        while ((long)n7 < this.u) {
            a a2 = this;
            synchronized (a2) {
                if (this.f == null) {
                    Object var25_26 = null;
                    return;
                }
            }
            int n8 = 0;
            while (n8 < this.o) {
                byte by = (this.t & 1L) != 0L ? byArray[n3 + n8 * (int)this.u + n7] : byArray[n3 + n7 * this.o + n8];
                b2.t((byte)n8, by);
                n8 = (byte)(n8 + 1);
            }
            b2.s(sArray);
            n8 = 0;
            while (n8 < n5) {
                double d4;
                double d5 = 0.0;
                int n9 = (int)((double)n8 * d2);
                int n10 = (int)((double)(n8 + 1) * d2);
                int n11 = n9;
                while (n11 < n10) {
                    d5 += (double)sArray[n11];
                    ++n11;
                }
                d3 = d4 = (d5 /= (double)(n10 - n9));
                byArray2[n6++] = a.a((int)d5 >> 4);
                ++n8;
            }
            if ((long)n7 % (this.u / 100L) == this.u / 100L - 1L) {
                if ((long)n7 % (this.u / 10L) == this.u / 10L - 1L) {
                    System.out.print("*");
                    Thread.yield();
                    try {
                        Thread.sleep(3L);
                    }
                    catch (InterruptedException interruptedException) {
                        // empty catch block
                    }
                }
                a a3 = this;
                synchronized (a3) {
                    ++this.g;
                }
            }
            ++n7;
        }
        System.out.println("\nEnd generate");
        if (this.k != null && this.h) {
            AudioPlayer.player.stop(this.k);
        }
        this.h = false;
        this.k = null;
        System.gc();
        this.k = new ContinuousAudioDataStream(new AudioData(byArray2));
        if (bl) {
            this.h = true;
            AudioPlayer.player.start(this.k);
        }
    }

    /*
     * Enabled aggressive block sorting
     */
    private final int c(byte[] byArray) {
        int n;
        int n2 = 0;
        if (byArray.length < 4) {
            return 0;
        }
        this.w = new String();
        int n3 = 0;
        while (n3 < 4) {
            this.w = this.w + (char)byArray[n2++];
            ++n3;
        }
        if (this.w.compareTo("YM3!") == 0) {
            this.u = (byArray.length - 4) / 14;
            this.o = (byte)14;
            this.t = 1L;
            this.q = (short)50;
            this.s = 0;
            this.r = 2000000L;
            this.p = 0L;
            this.m = new String("Unknown");
            this.n = new String("Unknown");
            this.l = new String("Unknown");
            return 4;
        }
        if (this.w.compareTo("YM3b") == 0) {
            this.u = (byArray.length - 8) / 14;
            this.o = (byte)14;
            this.t = 1L;
            this.q = (short)50;
            this.s = 0;
            this.r = 2000000L;
            this.p = 0L;
            this.p += (long)((byArray[byArray.length - 4] & 0xFF) << 24);
            this.p += (long)((byArray[byArray.length - 3] & 0xFF) << 16);
            this.p += (long)((byArray[byArray.length - 2] & 0xFF) << 8);
            this.p += (long)(byArray[byArray.length - 1] & 0xFF);
            this.m = new String("Unknown");
            this.n = new String("Unknown");
            this.l = new String("Unknown");
            return 4;
        }
        if (this.w.compareTo("YM6!") != 0 && this.w.compareTo("YM5!") != 0 && this.w.compareTo("YM4!") != 0) {
            this.u = byArray.length / 14;
            this.o = (byte)14;
            this.t = 0L;
            this.q = (short)50;
            this.s = 0;
            this.r = 2000000L;
            this.p = 0L;
            this.m = new String("Unknown");
            this.n = new String("Unknown");
            this.l = new String("Unknown");
            System.out.println("Warning: Unknown format *** -> default raw st used!");
            return 0;
        }
        this.o = (byte)16;
        this.v = new String();
        n3 = 0;
        while (n3 < 8) {
            this.v = this.v + (char)byArray[n2++];
            ++n3;
        }
        this.u = 0L;
        this.u += (long)((byArray[n2++] & 0xFF) << 24);
        this.u += (long)((byArray[n2++] & 0xFF) << 16);
        this.u += (long)((byArray[n2++] & 0xFF) << 8);
        this.u += (long)(byArray[n2++] & 0xFF);
        this.t = 0L;
        this.t += (long)((byArray[n2++] & 0xFF) << 24);
        this.t += (long)((byArray[n2++] & 0xFF) << 16);
        this.t += (long)((byArray[n2++] & 0xFF) << 8);
        this.t += (long)(byArray[n2++] & 0xFF);
        if (this.w.compareTo("YM4!") != 0) {
            this.s = 0;
            this.s = (short)(this.s + ((byArray[n2++] & 0xFF) << 8));
            this.s = (short)(this.s + (byArray[n2++] & 0xFF));
            this.r = 0L;
            this.r += (long)((byArray[n2++] & 0xFF) << 24);
            this.r += (long)((byArray[n2++] & 0xFF) << 16);
            this.r += (long)((byArray[n2++] & 0xFF) << 8);
            this.r += (long)(byArray[n2++] & 0xFF);
            this.q = 0;
            this.q = (short)(this.q + ((byArray[n2++] & 0xFF) << 8));
            this.q = (short)(this.q + (byArray[n2++] & 0xFF));
            this.p = 0L;
            this.p += (long)((byArray[n2++] & 0xFF) << 24);
            this.p += (long)((byArray[n2++] & 0xFF) << 16);
            this.p += (long)((byArray[n2++] & 0xFF) << 8);
            this.p += (long)(byArray[n2++] & 0xFF);
            n2 += 2;
        } else {
            this.s = 0;
            n2 += 2;
            this.s = (short)(this.s + ((byArray[n2++] & 0xFF) << 8));
            this.s = (short)(this.s + (byArray[n2++] & 0xFF));
            this.r = 2000000L;
            this.q = (short)50;
            this.p = 0L;
            this.p += (long)((byArray[n2++] & 0xFF) << 24);
            this.p += (long)((byArray[n2++] & 0xFF) << 16);
            this.p += (long)((byArray[n2++] & 0xFF) << 8);
            this.p += (long)(byArray[n2++] & 0xFF);
        }
        n3 = 0;
        while (n3 < this.s) {
            n = 0;
            n += (byArray[n2++] & 0xFF) << 24;
            n += (byArray[n2++] & 0xFF) << 16;
            n += (byArray[n2++] & 0xFF) << 8;
            n2 += (n += byArray[n2++] & 0xFF);
            ++n3;
        }
        this.m = new String();
        while (byArray[n2] != 0) {
            this.m = this.m + (char)byArray[n2++];
        }
        ++n2;
        this.n = new String();
        while (byArray[n2] != 0) {
            this.n = this.n + (char)byArray[n2++];
        }
        ++n2;
        this.l = new String();
        while (byArray[n2] != 0) {
            this.l = this.l + (char)byArray[n2++];
        }
        ++n2;
        if (this.w.compareTo("YM6!") != 0) {
            n = 0;
            while ((long)n < this.u) {
                byte by = byArray[n2 + (int)(this.u * 1L) + n];
                byte by2 = byArray[n2 + (int)(this.u * 3L) + n];
                byte by3 = byArray[n2 + (int)(this.u * 5L) + n];
                byte by4 = byArray[n2 + (int)(this.u * 6L) + n];
                byte by5 = byArray[n2 + (int)(this.u * 8L) + n];
                byte by6 = byArray[n2 + (int)(this.u * 9L) + n];
                byte by7 = byArray[n2 + (int)(this.u * 10L) + n];
                if ((by & 0x30) != 0) {
                    by = (byte)(by & 0x3F);
                    byte by8 = (byte)(by3 >> 4 & 0xF);
                    by3 = (byte)(by3 & 0xF);
                    switch (by & 0x30) {
                        case 16: {
                            by5 = by8;
                            break;
                        }
                        case 32: {
                            by6 = by8;
                            break;
                        }
                        case 48: {
                            by7 = by8;
                            break;
                        }
                    }
                }
                if ((by2 & 0x30) != 0) {
                    by2 = (byte)(by2 & 0x3F);
                    by2 = (byte)(by2 | 0x40);
                }
                byArray[n2 + (int)(this.u * 1L) + n] = by;
                byArray[n2 + (int)(this.u * 3L) + n] = by2;
                byArray[n2 + (int)(this.u * 5L) + n] = by3;
                byArray[n2 + (int)(this.u * 6L) + n] = by4;
                byArray[n2 + (int)(this.u * 8L) + n] = by5;
                byArray[n2 + (int)(this.u * 9L) + n] = by6;
                byArray[n2 + (int)(this.u * 10L) + n] = by7;
                ++n;
            }
        }
        return n2;
    }

    private final void b() {
        System.out.println("FileID:" + this.w);
        System.out.println("CheckStr:" + this.v);
        System.out.println("numVbl:" + this.u);
        System.out.println("sngAttr:" + this.t);
        System.out.println("nbSpl:" + this.s);
        System.out.println("YMClock:" + this.r + "Hz");
        System.out.println("playerFreq:" + this.q + "Hz");
        System.out.println("Loop at:" + this.p);
        System.out.println("Song:" + this.m);
        System.out.println("Author:" + this.n);
        System.out.println("Comments:" + this.l);
    }

    private static final byte a(int n) {
        int n2;
        if (n < 0) {
            n = -n;
            n2 = 127;
        } else {
            n2 = 255;
        }
        n = n < 32 ? 0xF0 | 15 - n / 2 : (n < 96 ? 0xE0 | 15 - (n - 32) / 4 : (n < 224 ? 0xD0 | 15 - (n - 96) / 8 : (n < 480 ? 0xC0 | 15 - (n - 224) / 16 : (n < 992 ? 0xB0 | 15 - (n - 480) / 32 : (n < 2016 ? 0xA0 | 15 - (n - 992) / 64 : (n < 4064 ? 0x90 | 15 - (n - 2016) / 128 : (n < 8160 ? 0x80 | 15 - (n - 4064) / 256 : 128)))))));
        return (byte)(n2 & n);
    }
}

