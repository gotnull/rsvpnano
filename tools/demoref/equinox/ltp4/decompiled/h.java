/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  h
 */
/*
 * Exception performing whole class analysis ignored.
 */
class h {
    boolean a(int n2, int n3, int[] nArray, int n4, int n5, int[] nArray2, int[] nArray3, float[] fArray, float[] fArray2, float[] fArray3, float f2, int[] nArray4, int[] nArray5, int[] nArray6, int n6) {
        int[] nArray7 = new int[n4 * 2 * 3];
        int n7 = (1 << n4) - 1;
        while (n7 > 0) {
            System.out.print(h.a((String)"\u0012'r\u0005X\u001d'j\u0003IKf") + nArray3[0] + "/" + nArray2[0] + h.a((String)"Qfi\t_\u001a/p\u0001\r\u001e($F") + n2 + " " + n3 + " " + n4 + h.a((String)"Qi1F") + n7 + h.a((String)"Qf>F\rQf>F\rQK"));
            int n8 = this.a(nArray, n7, nArray7, n4);
            n8 = this.a(nArray7, n8);
            if (this.a(nArray7, n8, fArray, fArray2, fArray3, f2)) {
                this.a(nArray, nArray2, n7, n4);
                this.a(nArray4, nArray5, nArray7, n8, nArray6, n6, nArray3);
                return true;
            }
            --n7;
        }
        return false;
    }

    void a(int[] nArray, int[] nArray2, int[] nArray3, int n2, int[] nArray4, int n3, int[] nArray5) {
        int n4;
        int n5 = 0;
        while (n5 < n2) {
            n4 = n5 + 1;
            while (n4 < n2) {
                if (nArray3[n5 * 2 + 1] == nArray3[n4 * 2]) {
                    int n6 = nArray3[n4 * 2];
                    int n7 = nArray3[n4 * 2 + 1];
                    nArray3[n4 * 2] = nArray3[(n5 + 1) * 2];
                    nArray3[n4 * 2 + 1] = nArray3[(n5 + 1) * 2 + 1];
                    nArray3[(n5 + 1) * 2] = n6;
                    nArray3[(n5 + 1) * 2 + 1] = n7;
                    n4 = n2;
                }
                ++n4;
            }
            ++n5;
        }
        if (Math.min(n2, n3) > nArray4[0]) {
            nArray4[0] = Math.min(n2, n3);
        }
        n5 = 0;
        n4 = nArray3[0];
        while (n2 > 2) {
            int n8 = nArray2[0];
            nArray2[0] = n8 + 1;
            nArray[n8] = Math.min(n2, n3);
            int n9 = nArray2[0];
            nArray2[0] = n9 + 1;
            nArray[n9] = n4;
            int n10 = n5 + 1;
            while (n10 < Math.min(n2, n3) + n5) {
                int n11 = nArray2[0];
                nArray2[0] = n11 + 1;
                nArray[n11] = nArray3[n10 * 2];
                ++n10;
            }
            n2 -= Math.min(n2, n3 - 2);
            n5 += n3 - 2;
            nArray5[0] = nArray5[0] + 1;
        }
    }

    void a(int[] nArray, int[] nArray2, int n2, int n3) {
        int n4 = 1;
        int n5 = 0;
        while (n5 < n3) {
            if ((n2 >> n5 & n4) == 1) {
                nArray[n5 * 4] = -1;
            }
            ++n5;
        }
        n5 = 0;
        int n6 = 0;
        int n7 = 0;
        while (n7 < nArray2[0]) {
            if (nArray[n7 * 4] != -1) {
                nArray[n6++] = nArray[n7 * 4];
                nArray[n6++] = nArray[n7 * 4 + 1];
                nArray[n6++] = nArray[n7 * 4 + 2];
                nArray[n6++] = nArray[n7 * 4 + 3];
                ++n5;
            }
            ++n7;
        }
        nArray2[0] = n5;
    }

    int a(int[] nArray, int n2, int[] nArray2, int n3) {
        boolean bl = true;
        int n4 = 0;
        int n5 = 0;
        int n6 = 0;
        while (n6 < n3) {
            if ((n2 >> n6 & 1) == 1) {
                nArray2[n4++] = nArray[n6 * 4 + 1];
                nArray2[n4++] = nArray[n6 * 4 + 2];
                nArray2[n4++] = nArray[n6 * 4 + 2];
                nArray2[n4++] = nArray[n6 * 4 + 3];
                nArray2[n4++] = nArray[n6 * 4 + 3];
                nArray2[n4++] = nArray[n6 * 4 + 1];
                n5 += 3;
            }
            ++n6;
        }
        return n5;
    }

    int a(int[] nArray, int n2) {
        int[] nArray2 = new int[n2 * 2];
        int n3 = 0;
        int n4 = 0;
        int n5 = 0;
        while (this.b(nArray, n2)) {
            int n6 = 0;
            while (n6 < n2) {
                if (nArray[n6 * 2] != -1) {
                    n3 = nArray[n6 * 2];
                    n4 = nArray[n6 * 2 + 1];
                    nArray[n6 * 2] = -1;
                    nArray[n6 * 2 + 1] = -1;
                    n6 = n2;
                }
                ++n6;
            }
            n6 = 0;
            while (n6 < n2) {
                if (nArray[n6 * 2] == n4 && nArray[n6 * 2 + 1] == n3) {
                    nArray[n6 * 2] = -1;
                    nArray[n6 * 2 + 1] = -1;
                    n6 = n2;
                    n3 = -1;
                    n4 = -1;
                }
                ++n6;
            }
            if (n3 == -1) continue;
            nArray2[n5 * 2] = n3;
            nArray2[n5 * 2 + 1] = n4;
            ++n5;
        }
        System.arraycopy(nArray2, 0, nArray, 0, n5 * 2);
        return n5;
    }

    /*
     * Unable to fully structure code
     */
    boolean a(int[] var1_1, int var2_2, float[] var3_3, float[] var4_4, float[] var5_5, float var6_6) {
        var45_7 = Troisd.gq;
        var38_8 = new float[var2_2 * 2];
        var39_9 = new float[var2_2 * 2];
        var40_10 = new float[var2_2 * 2];
        var41_11 = new float[var2_2 * 2];
        var43_12 = 0;
        if (!var45_7) ** GOTO lbl83
        do {
            var13_13 = var3_3[var1_1[0]];
            var14_14 = var4_4[var1_1[0]];
            var15_15 = var5_5[var1_1[0]];
            var16_16 = var3_3[var1_1[1]];
            var17_17 = var4_4[var1_1[1]];
            var18_18 = var5_5[var1_1[1]];
            var44_26 = 2;
            if (!var45_7) ** GOTO lbl25
            do {
                var19_19 = var3_3[var1_1[var44_26]];
                var20_20 = var4_4[var1_1[var44_26]];
                var21_21 = var5_5[var1_1[var44_26]];
                var39_9[var44_26] = (var20_20 - var14_14) * (var16_16 - var13_13) - (var17_17 - var14_14) * (var19_19 - var13_13);
                var40_10[var44_26] = (var21_21 - var15_15) * (var16_16 - var13_13) - (var18_18 - var15_15) * (var19_19 - var13_13);
                var41_11[var44_26] = (var21_21 - var15_15) * (var17_17 - var14_14) - (var18_18 - var15_15) * (var20_20 - var14_14);
                ++var44_26;
lbl25:
                // 2 sources

            } while (var44_26 < 2 * var2_2);
            var36_24 = false;
            var37_25 = false;
            var44_26 = 2;
            if (!var45_7) ** GOTO lbl38
            block2: while (true) {
                v0 = (cfr_temp_0 = var39_9[var44_26] - 0.0f) == 0.0f ? 0 : (cfr_temp_0 > 0.0f ? 1 : -1);
                do {
                    if (v0 > 0) {
                        var36_24 = true;
                    }
                    if (var39_9[var44_26] < 0.0f) {
                        var37_25 = true;
                    }
                    ++var44_26;
lbl38:
                    // 2 sources

                    if (var44_26 < 2 * var2_2) continue block2;
                    v0 = (float)var36_24;
                } while (var45_7);
                break;
            }
            if (v0 != false && var37_25 != false) {
                return false;
            }
            var36_24 = false;
            var37_25 = false;
            var44_26 = 2;
            if (!var45_7) ** GOTO lbl55
            block4: while (true) {
                v1 = (cfr_temp_1 = var40_10[var44_26] - 0.0f) == 0.0f ? 0 : (cfr_temp_1 > 0.0f ? 1 : -1);
                do {
                    if (v1 > 0) {
                        var36_24 = true;
                    }
                    if (var40_10[var44_26] < 0.0f) {
                        var37_25 = true;
                    }
                    ++var44_26;
lbl55:
                    // 2 sources

                    if (var44_26 < 2 * var2_2) continue block4;
                    v1 = (float)var36_24;
                } while (var45_7);
                break;
            }
            if (v1 != false && var37_25 != false) {
                return false;
            }
            var36_24 = false;
            var37_25 = false;
            var44_26 = 2;
            if (!var45_7) ** GOTO lbl72
            block6: while (true) {
                v2 = (cfr_temp_2 = var41_11[var44_26] - 0.0f) == 0.0f ? 0 : (cfr_temp_2 > 0.0f ? 1 : -1);
                do {
                    if (v2 > 0) {
                        var36_24 = true;
                    }
                    if (var41_11[var44_26] < 0.0f) {
                        var37_25 = true;
                    }
                    ++var44_26;
lbl72:
                    // 2 sources

                    if (var44_26 < 2 * var2_2) continue block6;
                    v2 = (float)var36_24;
                } while (var45_7);
                break;
            }
            if (v2 != false && var37_25 != false) {
                return false;
            }
            var34_22 = var1_1[0];
            var35_23 = var1_1[1];
            System.arraycopy(var1_1, 2, var1_1, 0, (var2_2 - 1) * 2);
            var1_1[(var2_2 - 1) * 2] = var34_22;
            var1_1[(var2_2 - 1) * 2 + 1] = var35_23;
            ++var43_12;
lbl83:
            // 2 sources

        } while (var43_12 < var2_2);
        return true;
    }

    boolean b(int[] nArray, int n2) {
        boolean bl = false;
        int n3 = 0;
        while (n3 < n2) {
            if (nArray[n3 * 2] != -1) {
                bl = true;
            }
            ++n3;
        }
        return bl;
    }

    h() {
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
                        n5 = 113;
                        break;
                    }
                    case 1: {
                        n5 = 70;
                        break;
                    }
                    case 2: {
                        n5 = 30;
                        break;
                    }
                    case 3: {
                        n5 = 102;
                        break;
                    }
                    default: {
                        n5 = 45;
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

