/*
 * Decompiled with CFR 0.152.
 */
package a;

import java.io.DataInputStream;
import java.net.URL;
import java.util.zip.ZipInputStream;

public class a {
    private static final byte[] a = new byte[]{0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    private static final int b = 132;
    private static final int c = 32635;
    public int d = 0;
    public boolean e = false;
    public static int f;

    /*
     * Unable to fully structure code
     */
    public void a(URL var1_1, byte[] var2_2, byte[] var3_3, int var4_4) {
        block16: {
            var12_5 = a.a.f;
            var5_6 = null;
            var6_7 = null;
            try {
                var5_6 = var1_1.openStream();
                if (var5_6 == null) break block16;
                try {
                    var7_8 = null;
                    System.out.println("a");
                    var7_8 = new ZipInputStream(var5_6);
                    System.out.println("b");
                    var7_8.getNextEntry();
                    System.out.println("c");
                    var6_7 = new DataInputStream(var7_8);
                    System.out.println("d");
                    var8_13 = 0;
                    if (var12_5 == 0) ** GOTO lbl24
                    block8: while (true) {
                        var3_3[var8_13] = var6_7.readByte();
                        this.d = var8_13;
                        do {
                            ++var8_13;
lbl24:
                            // 2 sources

                            if (var8_13 < var4_4) continue block8;
                            var8_13 = 0;
                            var9_14 = 0;
                            System.out.println(a.a.a(">:7#\u0013>:7#\u0013>:7#\u0013>"));
                            var8_13 = 0;
                            var9_14 = 0;
                            var10_15 = 0;
                            var11_16 = 0;
                        } while (var12_5 != 0);
                        break;
                    }
                    if (var12_5 == 0) ** GOTO lbl40
                    block10: while (true) {
                        var9_14 = var3_3[var11_16];
                        var2_2[var11_16] = (byte)((var9_14 & 255) + (var8_13 & 255));
                        var8_13 = var2_2[var11_16];
                        do {
                            ++var11_16;
lbl40:
                            // 2 sources

                            if (var11_16 < var4_4) continue block10;
                            System.out.println(a.a.a("yqe") + var10_15);
                            var11_16 = 0;
                        } while (var12_5 != 0);
                        break;
                    }
                    if (var12_5 == 0) ** GOTO lbl49
                    block12: while (true) {
                        var2_2[var11_16] = this.a(var2_2[var11_16] * 256);
                        do {
                            ++var11_16;
lbl49:
                            // 2 sources

                            if (var11_16 < var4_4) continue block12;
                        } while (var12_5 != 0);
                        break;
                    }
                }
                catch (Exception var7_9) {
                    // empty catch block
                }
                try {
                    if (var6_7 != null) {
                        var6_7.close();
                    }
                }
                catch (Exception var7_10) {}
            }
            catch (Exception var7_11) {
                // empty catch block
            }
        }
        try {
            if (var5_6 != null) {
                var5_6.close();
            }
        }
        catch (Exception var7_12) {
            // empty catch block
        }
        this.e = true;
    }

    private byte a(int n2) {
        int n3;
        byte by;
        int n4;
        int n5;
        if (n2 < 0) {
            n2 = -n2;
            n5 = 128;
        } else {
            n5 = 0;
        }
        if (n2 > 32635) {
            n2 = 32635;
        }
        if ((n4 = ~(n5 | (by = a[(n2 += 132) >> 7 & 0xFF]) << 4 | (n3 = n2 >> by + 3 & 0xF)) & 0xFF) == 0) {
            n4 = 2;
        }
        return (byte)n4;
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
                        n5 = 20;
                        break;
                    }
                    case 1: {
                        n5 = 16;
                        break;
                    }
                    case 2: {
                        n5 = 29;
                        break;
                    }
                    case 3: {
                        n5 = 9;
                        break;
                    }
                    default: {
                        n5 = 57;
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

