/*
 * Decompiled with CFR 0.152.
 */
public class e {
    public void a(int n2, int n3, byte[] byArray, byte[] byArray2) {
        boolean bl = Troisd.gq;
        int n4 = 0;
        int n5 = 0;
        block0: while (true) {
            int n6 = n5;
            block1: while (n6 < 190) {
                for (int i2 = 0; i2 < 157; ++i2) {
                    block8: {
                        block9: {
                            byte by;
                            int n7;
                            n6 = n7 = byArray2[n4++] & 0xFF;
                            if (bl) continue block1;
                            if (n6 == 0) break block8;
                            if ((n7 -= n2) < 128) {
                                n7 = 128;
                            }
                            if ((by = byArray[n3]) < 0) {
                                by = 0;
                            }
                            if ((n7 += by & 0xFF) >= 256) break block9;
                            byArray[n3] = (byte)n7;
                            if (!bl) break block8;
                        }
                        byArray[n3] = -1;
                    }
                    ++n3;
                    if (!bl) continue;
                }
                n3 += 123;
                ++n5;
                if (!bl) continue block0;
            }
            break;
        }
    }
}

