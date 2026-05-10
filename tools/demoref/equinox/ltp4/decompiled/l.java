/*
 * Decompiled with CFR 0.152.
 */
public class l {
    public static int a;

    public void a(byte[] byArray, byte[] byArray2, byte[] byArray3, int n2) {
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        int n8;
        int n9;
        boolean bl;
        block16: {
            bl = Troisd.gq;
            n9 = 0;
            n8 = 2859;
            n7 = 0;
            n6 = 60;
            block0: while (true) {
                int n10 = n6;
                block1: while (n10 < 127) {
                    n5 = 161;
                    if (bl) break block16;
                    for (n4 = v44062; n4 >= 0; --n4) {
                        block17: {
                            block18: {
                                n10 = n3 = byArray2[n9++] & 0xFF;
                                if (bl) continue block1;
                                if (n10 == 0) break block17;
                                n3 = (n3 - 128) / 4;
                                if ((n3 += byArray[n8] & 0xFF) >= 128) break block18;
                                byArray[n8] = (byte)n3;
                                if (!bl) break block17;
                            }
                            byArray[n8] = 127;
                        }
                        ++n8;
                        if (!bl) continue;
                    }
                    n8 += 118;
                    ++n6;
                    if (!bl) continue block0;
                }
                break;
            }
            n9 = n2 * 8 * 30;
            n8 = 22420;
            n5 = 180;
        }
        n6 = n5;
        block3: while (true) {
            int n11 = n6;
            block4: while (n11 >= 0) {
                block20: {
                    block21: {
                        block19: {
                            if (n6 >= 14) break block19;
                            n7 = 14 - n6 << 2;
                            if (!bl) break block20;
                        }
                        if (n6 <= 166) break block21;
                        n7 = n6 - 180 + 14 << 2;
                        if (!bl) break block20;
                    }
                    n7 = 0;
                }
                for (n4 = 239; n4 >= 0; --n4) {
                    block22: {
                        block23: {
                            n11 = n3 = byArray3[n9++] & 0xFF;
                            if (bl) continue block4;
                            if (n11 == 0) break block22;
                            if ((n3 = (n3 >> 2) - n7) < 0) {
                                n3 = 0;
                            }
                            if ((n3 += byArray[n8] & 0xFF) >= 128) break block23;
                            byArray[n8] = (byte)n3;
                            if (!bl) break block22;
                        }
                        byArray[n8] = 127;
                    }
                    ++n8;
                    if (!bl) continue;
                }
                n8 += 40;
                --n6;
                if (!bl) continue block3;
            }
            break;
        }
    }
}

