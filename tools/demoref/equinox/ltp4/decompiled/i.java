/*
 * Decompiled with CFR 0.152.
 */
public class i {
    public void a(byte[] byArray, byte[] byArray2) {
        int n2;
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        boolean bl;
        block13: {
            bl = Troisd.gq;
            n7 = 0;
            n6 = 2859;
            n5 = 0;
            block0: while (true) {
                int n8 = n5;
                block1: while (n8 < 60) {
                    n4 = 0;
                    if (bl) break block13;
                    for (n3 = v13221; n3 < 162; ++n3) {
                        block14: {
                            block15: {
                                n8 = n2 = byArray2[n7++] & 0xFF;
                                if (bl) continue block1;
                                if (n8 == 0) break block14;
                                if (n2 < 128) {
                                    n2 = 128;
                                }
                                if ((n2 += byArray[n6] & 0xFF) >= 256) break block15;
                                byArray[n6] = (byte)n2;
                                if (!bl) break block14;
                            }
                            byArray[n6] = -1;
                        }
                        ++n6;
                        if (!bl) continue;
                    }
                    n6 += 118;
                    ++n5;
                    if (!bl) continue block0;
                }
                break;
            }
            n4 = 60;
        }
        n5 = n4;
        block3: while (true) {
            int n9 = n5;
            block4: while (n9 < 127) {
                for (n3 = 0; n3 < 162; ++n3) {
                    block16: {
                        block17: {
                            n9 = n2 = byArray2[n7++] & 0xFF;
                            if (bl) continue block4;
                            if (n9 == 0) break block16;
                            n2 = n2 - 128 >> 2;
                            if ((n2 += byArray[n6] & 0xFF) >= 128) break block17;
                            byArray[n6] = (byte)n2;
                            if (!bl) break block16;
                        }
                        byArray[n6] = 127;
                    }
                    ++n6;
                    if (!bl) continue;
                }
                n6 += 118;
                ++n5;
                if (!bl) continue block3;
            }
            break;
        }
    }
}

