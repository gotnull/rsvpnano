/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  m
 */
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class m {
    /*
     * Unable to fully structure code
     */
    public static void main(String[] var0) throws IOException {
        var12_1 = Troisd.gq;
        var1_2 = new FileInputStream(m.a((String)"t*~6<v)}"));
        var2_3 = new FileOutputStream(m.a((String)"t*~6<s!}"));
        var3_4 = new DataInputStream(var1_2);
        var4_5 = new DataOutputStream(var2_3);
        var5_6 = Integer.parseInt(var3_4.readLine());
        var4_5.writeInt(var5_6);
        var7_7 = 0;
        if (!var12_1) ** GOTO lbl20
        block0: while (true) {
            v0 = var3_4.readLine();
            do {
                var6_8 = Float.valueOf(v0).floatValue();
                var4_5.writeFloat(var6_8);
                var6_8 = Float.valueOf(var3_4.readLine()).floatValue();
                var4_5.writeFloat(var6_8);
                var6_8 = Float.valueOf(var3_4.readLine()).floatValue();
                var4_5.writeFloat(var6_8);
                ++var7_7;
lbl20:
                // 2 sources

                if (var7_7 < var5_6) continue block0;
                v0 = var3_4.readLine();
            } while (var12_1);
            break;
        }
        var7_7 = Integer.parseInt(v0);
        var4_5.writeInt(3);
        var4_5.writeInt(var7_7);
        var11_9 = 0;
        if (!var12_1) ** GOTO lbl39
        block2: while (true) {
            v1 = var3_4;
            do {
                var8_10 = (short)Integer.parseInt(v1.readLine());
                var8_10 = (short)Integer.parseInt(var3_4.readLine());
                var9_11 = (short)Integer.parseInt(var3_4.readLine());
                var10_12 = (short)Integer.parseInt(var3_4.readLine());
                var4_5.writeShort(var8_10);
                var4_5.writeShort(var9_11);
                var4_5.writeShort(var10_12);
                ++var11_9;
lbl39:
                // 2 sources

                if (var11_9 < var7_7) continue block2;
                v1 = var3_4;
            } while (var12_1);
            break;
        }
        v1.close();
        var4_5.close();
        System.out.println(m.a((String)"~$x<"));
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
                        n5 = 25;
                        break;
                    }
                    case 1: {
                        n5 = 75;
                        break;
                    }
                    case 2: {
                        n5 = 23;
                        break;
                    }
                    case 3: {
                        n5 = 88;
                        break;
                    }
                    default: {
                        n5 = 18;
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

