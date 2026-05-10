/*
 * Decompiled with CFR 0.152.
 */
package b.c;

import b.e.a;

public class b {
    private int a;
    public static boolean b;

    /*
     * Handled impossible loop by adding 'first' condition
     * Enabled aggressive block sorting
     */
    public void a(byte[] byArray, int n2, int n3) {
        boolean bl = b;
        int n4 = 0;
        boolean bl2 = true;
        do {
            if (bl2 && !(bl2 = false) && !bl) continue;
            byArray[n4] = 0;
            ++this.a;
            ++n4;
        } while (n4 < n2 * n3);
        if (b.e.a.m != 0) {
            b = !bl;
        }
    }
}

