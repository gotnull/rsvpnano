/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Graphics;
import java.awt.Image;

public class c {
    public float co;
    public float cn;
    public float cm;
    public float cl;
    public int ck;
    public int cj;
    public int ci;
    public int ch = 50;
    public boolean cg;
    public float cf;
    public float ce;
    public float cd;
    public float cc;
    public float ca;
    public float b9;
    public float b8;

    /*
     * Enabled aggressive block sorting
     */
    public final void w(Graphics graphics, Image image) {
        this.ck = 0;
        while (this.ck < this.ch) {
            this.cj = (int)(Math.cos((double)this.co + (double)((float)this.ck / 50.0f) * 12.566370964050293) * Math.sin(this.cl) * 20.0 + Math.cos((double)(2.0f * this.cn) / 2.0 - (double)((float)this.ck / 50.0f * 6.28f)) * 40.0 - Math.sin((double)(this.cn / 2.0f + this.cm) + (double)((float)this.ck / 50.0f) * 6.283185005187988) * 60.0) + 160 - 7 + 40;
            this.ci = (int)(Math.cos((double)(this.cn * 1.5f) + (double)((float)this.ck / 50.0f) * 6.283185005187988) * Math.sin(this.cm) * 20.0 - Math.sin((double)(4.0f * this.co) / 3.0 + (double)((float)this.ck / 50.0f * 6.28f * 2.0f)) * 40.0 + Math.sin((double)(this.co / 2.0f + this.cl) + (double)((float)this.ck / 50.0f) * 6.283185005187988) * 30.0) + 100 - 7;
            graphics.drawImage(image, this.cj, this.ci, null);
            byte cfr_ignored_0 = (byte)this.cn;
            ++this.ck;
        }
        this.co = (float)((double)this.co + (double)0.03f);
        this.cn = (float)((double)this.cn + (double)0.0219f);
        this.cm = (float)((double)this.cm + (double)0.0273f);
        this.cl = (float)((double)this.cl + (double)0.0309f);
    }

    /*
     * Enabled aggressive block sorting
     */
    public final void v(Graphics graphics, Image image, Image image2, Image image3, Image image4) {
        this.cg = ((double)this.cd + (double)0.4f) * 100.0 % 628.0 < 157.0 || ((double)this.cd + (double)0.4f) * 100.0 % 628.0 > 471.0;
        this.ca = (float)(Math.cos((double)this.ce + (double)0.8f) + 1.5);
        this.b9 = (float)(Math.cos((double)this.cd + (double)0.4f) + 1.5);
        this.b8 = (float)(Math.cos(this.cc) + 1.5);
        if (this.cg) {
            this.ck = 0;
            while (this.ck < 15) {
                this.ci = (int)(Math.sin((double)this.cf + (double)this.ck * (double)0.16f * 1.5) * 39.0 + 98.0 - 8.0 + 5.0);
                graphics.drawImage(image, 80, this.ci, null);
                byte cfr_ignored_0 = (byte)this.cf;
                ++this.ck;
            }
        }
        double d2 = this.cc / 2.0f;
        float f2 = 164.0f + (float)Math.cos(d2) * 80.0f;
        d2 = this.cc;
        float f3 = (float)Math.sin(d2) * 30.0f;
        d2 = -this.cc;
        int n = (int)(f2 + f3 * (float)Math.cos(d2));
        d2 = this.cc;
        float f4 = (float)Math.sin(d2) * 100.0f;
        d2 = (double)this.cc - (double)0.4f;
        float f5 = (float)Math.cos(d2) * 25.0f;
        d2 = (double)this.cc - (double)0.4f;
        graphics.drawImage(image4, n, 90 + (int)(f4 - f5 * (float)Math.sin(d2)), (int)(30.0f * this.b8), (int)(30.0f * this.b8), null);
        d2 = (double)(this.cd / 2.0f) + (double)0.8f;
        float f6 = 164.0f + (float)Math.cos(d2) * 80.0f;
        d2 = (double)this.cd + (double)0.4f;
        float f7 = (float)Math.sin(d2) * 30.0f;
        d2 = (double)(-this.cd) + (double)0.4f;
        int n2 = (int)(f6 + f7 * (float)Math.cos(d2));
        d2 = (double)this.cd + (double)0.4f;
        float f8 = (float)Math.sin(d2) * 100.0f;
        d2 = this.cd;
        float f9 = (float)Math.cos(d2) * 25.0f;
        d2 = this.cd;
        graphics.drawImage(image3, n2, 90 + (int)(f8 - f9 * (float)Math.sin(d2)), (int)(30.0f * this.b9), (int)(30.0f * this.b9), null);
        d2 = (double)(this.ce / 2.0f) + (double)1.6f;
        float f10 = 164.0f + (float)Math.cos(d2) * 80.0f;
        d2 = (double)this.ce + (double)0.8f;
        float f11 = (float)Math.sin(d2) * 30.0f;
        d2 = (double)(-this.ce) + (double)0.8f;
        int n3 = (int)(f10 + f11 * (float)Math.cos(d2));
        d2 = (double)this.ce + (double)0.8f;
        float f12 = (float)Math.sin(d2) * 100.0f;
        d2 = (double)this.ce + (double)0.4f;
        float f13 = (float)Math.cos(d2) * 25.0f;
        d2 = (double)this.ce + (double)0.4f;
        graphics.drawImage(image2, n3, 90 + (int)(f12 - f13 * (float)Math.sin(d2)), (int)(30.0f * this.ca), (int)(30.0f * this.ca), null);
        if (!this.cg) {
            this.ck = 0;
            while (this.ck < 15) {
                this.ci = (int)(Math.sin((double)this.cf + (double)this.ck * (double)0.16f * 1.5) * 39.0 + 98.0 - 8.0 + 5.0);
                graphics.drawImage(image, 80, this.ci, null);
                byte cfr_ignored_1 = (byte)this.cf;
                ++this.ck;
            }
        }
        this.ce += 0.03f;
        this.cd = this.ce - 0.1f;
        this.cc = this.ce - 0.2f;
        this.cf = (float)((double)this.cf + (double)0.03f);
    }
}

