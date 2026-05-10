/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  c
 *  d
 *  h
 *  k
 *  n
 */
import b.e.a;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class n {
    /*
     * Unable to fully structure code
     */
    public static void main(String[] var0) throws IOException {
        var67_1 = Troisd.gq;
        var1_2 = 1.0E-7f;
        var2_3 = 1.0E-5f;
        var3_4 = 10;
        var4_5 = true;
        var20_6 = new FileInputStream(n.a((String)"\u0016f\u0011W\b\u0014e\u0012"));
        var21_7 = new FileOutputStream(n.a((String)"\u0016f\u0011W\b\u0011m\u0012"));
        var22_8 = new DataInputStream(var20_6);
        var23_9 = new DataOutputStream(var21_7);
        var24_10 = Integer.parseInt(var22_8.readLine());
        var26_11 = new float[var24_10 + 1];
        var27_12 = new float[var24_10 + 1];
        var28_13 = new float[var24_10 + 1];
        System.out.println(" ");
        var29_14 = 0;
        if (!var67_1) ** GOTO lbl26
        var68_16 = a.m;
        a.m = ++var68_16;
        block0: while (true) {
            System.out.print(n.a((String)"\tb\u0019]\u0006\u000bh\u0011WR\b=X") + (var29_14 + 1) + n.a((String)"['X\u0019\u0006[\n"));
            do {
                var26_11[var29_14] = var25_17 = Float.valueOf(var22_8.readLine()).floatValue();
                var27_12[var29_14] = var25_17 = Float.valueOf(var22_8.readLine()).floatValue();
                var28_13[var29_14] = var25_17 = Float.valueOf(var22_8.readLine()).floatValue();
                ++var29_14;
lbl26:
                // 2 sources

                if (var29_14 < var24_10) continue block0;
                System.out.println();
            } while (var67_1);
            break;
        }
        var29_15 = new int[]{Integer.parseInt(var22_8.readLine())};
        var30_18 = var24_10;
        var31_19 = var29_15[0];
        var32_20 = new int[var29_15[0] * 4];
        var33_21 = new int[var29_15[0] * 20];
        var34_22 = new int[]{0};
        var35_23 = new int[]{0};
        var36_24 = 0;
        var40_25 = 0;
        if (!var67_1) ** GOTO lbl52
        block2: while (true) {
            System.out.print(n.a((String)"\tb\u0019]\u0006\u000bh\u0014@UA'") + (var40_25 + 1) + n.a((String)"['X\u0019\u0006[\n"));
            v0 = Integer.parseInt(var22_8.readLine());
            do {
                var37_27 = v0;
                var32_20[var36_24++] = var37_27;
                var37_27 = Integer.parseInt(var22_8.readLine());
                var32_20[var36_24++] = var37_27;
                var38_28 = Integer.parseInt(var22_8.readLine());
                var32_20[var36_24++] = var38_28;
                var39_29 = Integer.parseInt(var22_8.readLine());
                var32_20[var36_24++] = var39_29;
                ++var40_25;
lbl52:
                // 2 sources

                if (var40_25 < var29_15[0]) continue block2;
                System.out.println();
                v0 = var29_15[0];
            } while (var67_1);
            break;
        }
        var40_26 = new float[v0];
        var41_30 = new float[var29_15[0]];
        var42_31 = new float[var29_15[0]];
        var43_32 = new float[var29_15[0]];
        var44_33 = new float[var29_15[0]];
        var45_34 = new float[var29_15[0]];
        var36_24 = 0;
        var46_35 = 0;
        if (!var67_1) ** GOTO lbl89
        block4: while (true) {
            System.out.print(n.a((String)"\u0016f\u0013\\\u0006\u0015h\nTG\u0017tB\u0019") + (var46_35 + 1) + n.a((String)"['X\u0019\u0006['X\u0019+"));
            do {
                var6_37 = var26_11[var32_20[var36_24 + 1]];
                var9_40 = var27_12[var32_20[var36_24 + 1]];
                var12_43 = var28_13[var32_20[var36_24 + 1]];
                var7_38 = var26_11[var32_20[var36_24 + 2]];
                var10_41 = var27_12[var32_20[var36_24 + 2]];
                var13_44 = var28_13[var32_20[var36_24 + 2]];
                var8_39 = var26_11[var32_20[var36_24 + 3]];
                var11_42 = var27_12[var32_20[var36_24 + 3]];
                var14_45 = var28_13[var32_20[var36_24 + 3]];
                var17_48 = (var7_38 - var6_37) * (var11_42 - var9_40) - (var8_39 - var6_37) * (var10_41 - var9_40);
                var15_46 = (var10_41 - var9_40) * (var14_45 - var12_43) - (var11_42 - var9_40) * (var13_44 - var12_43);
                var16_47 = (var13_44 - var12_43) * (var8_39 - var6_37) - (var14_45 - var12_43) * (var7_38 - var6_37);
                var18_49 = (float)Math.sqrt(var17_48 * var17_48 + var15_46 * var15_46 + var16_47 * var16_47);
                var40_26[var46_35] = var15_46 / var18_49;
                var41_30[var46_35] = var16_47 / var18_49;
                var42_31[var46_35] = var17_48 / var18_49;
                var43_32[var46_35] = var6_37;
                var44_33[var46_35] = var9_40;
                var45_34[var46_35] = var12_43;
                var36_24 += 4;
                ++var46_35;
lbl89:
                // 2 sources

                if (var46_35 < var29_15[0]) continue block4;
                System.out.println();
            } while (var67_1);
            break;
        }
        var46_36 = new c();
        var47_50 = new k();
        var48_51 = new d();
        var49_52 = new h();
        var52_53 = new int[]{0};
        var53_54 = new int[var24_10];
        var54_55 = new int[var24_10];
        var55_56 = 0;
        if (!var67_1) ** GOTO lbl106
        block6: while (true) {
            var53_54[var55_56] = var55_56;
            var54_55[var55_56] = 0;
            do {
                ++var55_56;
lbl106:
                // 2 sources

                if (var55_56 < var24_10) continue block6;
                var55_56 = var24_10;
                var56_57 = 0;
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl123
        block8: while (true) {
            v1 = var57_58 = var56_57 + 1;
            do {
                if (!var67_1) ** GOTO lbl121
                do {
                    if (var26_11[var56_57] == var26_11[var57_58] && var27_12[var56_57] == var27_12[var57_58] && var28_13[var56_57] == var28_13[var57_58] && var54_55[var57_58] == 0) {
                        var53_54[var57_58] = var56_57;
                        var54_55[var57_58] = 1;
                        --var55_56;
                    }
                    ++var57_58;
lbl121:
                    // 2 sources

                } while (var57_58 < var24_10);
                ++var56_57;
lbl123:
                // 2 sources

                if (var56_57 < var24_10) continue block8;
                v1 = 0;
            } while (var67_1);
            break;
        }
        var56_57 = v1;
        if (!var67_1) ** GOTO lbl134
        block11: while (true) {
            var32_20[var56_57 * 4 + 1] = var53_54[var32_20[var56_57 * 4 + 1]];
            var32_20[var56_57 * 4 + 2] = var53_54[var32_20[var56_57 * 4 + 2]];
            var32_20[var56_57 * 4 + 3] = var53_54[var32_20[var56_57 * 4 + 3]];
            do {
                ++var56_57;
lbl134:
                // 2 sources

                if (var56_57 < var29_15[0]) continue block11;
                System.out.println(n.a((String)"\tb\u0019UV\u0014n\u0016MUA'") + var55_56);
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl237
        block13: do {
            var51_61 = 1;
            var50_60 = 1;
            var56_57 = 1;
            if (!var67_1) ** GOTO lbl150
            block14: while (true) {
                v2 = (int)var46_36.a(var40_26[0], var41_30[0], var42_31[0], var40_26[var56_57], var41_30[var56_57], var42_31[var56_57], var43_32[0], var44_33[0], var45_34[0], var56_57, var32_20, var26_11, var27_12, var28_13, 1.0E-7f);
                do {
                    if ((var19_59 = v2) == 1) {
                        var47_50.a(var40_26, var41_30, var42_31, var43_32, var44_33, var45_34, var32_20, var56_57, var51_61);
                        ++var51_61;
                    }
                    ++var56_57;
lbl150:
                    // 2 sources

                    if (var56_57 < var29_15[0]) continue block14;
                    v2 = var51_61;
                } while (var67_1);
                break;
            }
            if (v2 == 1) {
                var34_22[0] = var34_22[0] + 1;
                v3 = var35_23[0];
                var35_23[0] = v3 + 1;
                var33_21[v3] = 3;
                v4 = var35_23[0];
                var35_23[0] = v4 + 1;
                var33_21[v4] = var32_20[1];
                v5 = var35_23[0];
                var35_23[0] = v5 + 1;
                var33_21[v5] = var32_20[2];
                v6 = var35_23[0];
                var35_23[0] = v6 + 1;
                var33_21[v6] = var32_20[3];
                System.arraycopy(var32_20, 4, var32_20, 0, (var29_15[0] - 1) * 4);
                System.arraycopy(var40_26, 1, var40_26, 0, var29_15[0] - 1);
                System.arraycopy(var41_30, 1, var41_30, 0, var29_15[0] - 1);
                System.arraycopy(var42_31, 1, var42_31, 0, var29_15[0] - 1);
                System.arraycopy(var43_32, 1, var43_32, 0, var29_15[0] - 1);
                System.arraycopy(var44_33, 1, var44_33, 0, var29_15[0] - 1);
                System.arraycopy(var45_34, 1, var45_34, 0, var29_15[0] - 1);
                var29_15[0] = var29_15[0] - 1;
                System.out.print(n.a((String)"\u0018f\u0014ZS\u0017f\f\\BA'") + var34_22[0] + "/" + var29_15[0] + n.a((String)"['X\u0019\u0006['X\u0019\u0006['X\u0019\u0006['X\u0019\u0006[\n"));
                if (3 >= var52_53[0]) {
                    var52_53[0] = 3;
                }
                if (!var67_1) continue;
            }
            var56_57 = 0;
            if (!var67_1) ** GOTO lbl194
            block16: while (true) {
                v7 = var56_57;
                v8 = 1;
                do {
                    var57_58 = v7 + v8;
                    if (!var67_1) ** GOTO lbl192
                    do {
                        if (var48_51.a(var32_20, var56_57, var57_58) == true && var50_60 <= var57_58) {
                            var47_50.a(var40_26, var41_30, var42_31, var43_32, var44_33, var45_34, var32_20, var57_58, var50_60);
                            ++var50_60;
                        }
                        ++var57_58;
lbl192:
                        // 2 sources

                    } while (var57_58 < var51_61);
                    ++var56_57;
lbl194:
                    // 2 sources

                    if (var56_57 < var50_60) continue block16;
                    var56_57 = var51_61;
                    var57_58 = var50_60;
                    v7 = var50_60;
                    v8 = 1;
                } while (var67_1);
                break;
            }
            if (v7 > v8) {
                var50_60 = 1;
                var51_61 = 1;
            }
            if (var50_60 != 1) ** GOTO lbl232
            var34_22[0] = var34_22[0] + 1;
            v9 = var35_23[0];
            var35_23[0] = v9 + 1;
            var33_21[v9] = 3;
            v10 = var35_23[0];
            var35_23[0] = v10 + 1;
            var33_21[v10] = var32_20[1];
            v11 = var35_23[0];
            var35_23[0] = v11 + 1;
            var33_21[v11] = var32_20[2];
            v12 = var35_23[0];
            var35_23[0] = v12 + 1;
            var33_21[v12] = var32_20[3];
            System.arraycopy(var32_20, 4, var32_20, 0, (var29_15[0] - 1) * 4);
            System.arraycopy(var40_26, 1, var40_26, 0, var29_15[0] - 1);
            System.arraycopy(var41_30, 1, var41_30, 0, var29_15[0] - 1);
            System.arraycopy(var42_31, 1, var42_31, 0, var29_15[0] - 1);
            System.arraycopy(var43_32, 1, var43_32, 0, var29_15[0] - 1);
            System.arraycopy(var44_33, 1, var44_33, 0, var29_15[0] - 1);
            System.arraycopy(var45_34, 1, var45_34, 0, var29_15[0] - 1);
            var29_15[0] = var29_15[0] - 1;
            if (3 >= var52_53[0]) {
                var52_53[0] = 3;
            }
            v13 = System.out;
            v14 = n.a((String)"\u0018f\u0014ZS\u0017f\f\\BA'") + var34_22[0] + "/" + var29_15[0] + n.a((String)"['X\u0019\u0006['X\u0019\u0006['X\u0019\u0006['X\u0019\u0006[\n");
            do {
                v13.print(v14);
                if (!var67_1) continue block13;
lbl232:
                // 2 sources

                if (var49_52.a(var56_57, var57_58, var32_20, var50_60, var51_61, var29_15, var34_22, var26_11, var27_12, var28_13, 1.0E-7f, var33_21, var35_23, var52_53, 10)) continue block13;
                v13 = System.out;
                v14 = n.a((String)"\u001a'\u001eXO\tbX");
            } while (var67_1);
            v13.println(v14);
lbl237:
            // 5 sources

        } while (var29_15[0] > 1);
        if (var29_15[0] == 1) {
            var34_22[0] = var34_22[0] + 1;
            v15 = var35_23[0];
            var35_23[0] = v15 + 1;
            var33_21[v15] = 3;
            v16 = var35_23[0];
            var35_23[0] = v16 + 1;
            var33_21[v16] = var32_20[1];
            v17 = var35_23[0];
            var35_23[0] = v17 + 1;
            var33_21[v17] = var32_20[2];
            v18 = var35_23[0];
            var35_23[0] = v18 + 1;
            var33_21[v18] = var32_20[3];
            if (3 >= var52_53[0]) {
                var52_53[0] = 3;
            }
        }
        System.out.print(n.a((String)"\u0018f\u0014ZS\u0017f\f\\BA'") + var34_22[0] + "/" + var29_15[0] + n.a((String)"['X\u0019\u0006['X\u0019\u0006['X\u0019\u0006['X\u0019\u0006[\n"));
        System.out.println();
        var56_57 = 0;
        if (!var67_1) ** GOTO lbl262
        block20: while (true) {
            var54_55[var56_57] = -1;
            do {
                ++var56_57;
lbl262:
                // 2 sources

                if (var56_57 < var24_10) continue block20;
                var55_56 = var24_10;
                var56_57 = 0;
                var59_62 = 0;
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl296
        block22: while (true) {
            var56_57 = 0;
            do {
                block64: {
                    block63: {
                        var58_63 = 0;
                        var60_64 = 0;
                        if (!var67_1) ** GOTO lbl286
                        block24: while (true) {
                            v19 = var33_21[var56_57++];
                            do {
                                var57_58 = v19;
                                var61_65 = 0;
                                if (!var67_1) ** GOTO lbl284
                                do {
                                    if (var33_21[var56_57++] == var59_62) {
                                        var58_63 = 1;
                                    }
                                    ++var61_65;
lbl284:
                                    // 2 sources

                                } while (var61_65 < var57_58);
                                ++var60_64;
lbl286:
                                // 2 sources

                                if (var60_64 < var34_22[0]) continue block24;
                                v19 = var58_63;
                            } while (var67_1);
                            break;
                        }
                        if (v19 != 1) break block63;
                        var54_55[var59_62] = 1;
                        if (!var67_1) break block64;
                    }
                    --var55_56;
                }
                ++var59_62;
lbl296:
                // 2 sources

                if (var59_62 < var24_10) continue block22;
                var59_62 = 0;
                var60_64 = 0;
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl309
        block27: while (true) {
            if (var54_55[var60_64] == 1) {
                var53_54[var60_64] = var59_62;
                var26_11[var59_62] = var26_11[var60_64];
                var27_12[var59_62] = var27_12[var60_64];
                var28_13[var59_62++] = var28_13[var60_64];
            }
            do {
                ++var60_64;
lbl309:
                // 2 sources

                if (var60_64 < var24_10) continue block27;
                var56_57 = 0;
                var60_64 = 0;
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl326
        block29: while (true) {
            v20 = var33_21[var56_57++];
            do {
                var57_58 = v20;
                var61_65 = 0;
                if (!var67_1) ** GOTO lbl324
                do {
                    var33_21[var56_57] = var53_54[var33_21[var56_57]];
                    ++var56_57;
                    ++var61_65;
lbl324:
                    // 2 sources

                } while (var61_65 < var57_58);
                ++var60_64;
lbl326:
                // 2 sources

                if (var60_64 < var34_22[0]) continue block29;
                System.out.println(n.a((String)"\tb\u0019UV\u0014n\u0016MUA'") + var55_56);
                var24_10 = var55_56;
                var60_64 = 0;
                v20 = 0;
            } while (var67_1);
            break;
        }
        var61_65 = v20;
        if (!var67_1) ** GOTO lbl371
        block32: while (true) {
            v21 = var33_21[var60_64++];
            do {
                var62_66 = v21;
                var63_67 = new int[var62_66];
                var65_70 = 0;
                if (!var67_1) ** GOTO lbl354
                block34: while (true) {
                    var64_69 = 0;
                    do {
                        var66_71 = 0;
                        if (!var67_1) ** GOTO lbl350
                        block36: while (true) {
                            var64_69 += var33_21[var60_64 + (var66_71 + 1 + var65_70) % var62_66] - var33_21[var60_64 + (var66_71 + var65_70) % var62_66];
                            do {
                                ++var66_71;
lbl350:
                                // 2 sources

                                if (var66_71 < var62_66 - 1) continue block36;
                                var63_67[var65_70] = var64_69;
                            } while (var67_1);
                            break;
                        }
                        ++var65_70;
lbl354:
                        // 2 sources

                        if (var65_70 < var62_66) continue block34;
                        var64_69 = 0;
                        var65_70 = 0;
                    } while (var67_1);
                    break;
                }
                if (!var67_1) ** GOTO lbl364
                block38: while (true) {
                    if (var63_67[var65_70] < var63_67[var64_69]) {
                        var64_69 = var65_70;
                    }
                    do {
                        ++var65_70;
lbl364:
                        // 2 sources

                        if (var65_70 < var62_66) continue block38;
                        System.arraycopy(var33_21, var60_64, var63_67, 0, var62_66);
                        System.arraycopy(var63_67, var64_69, var33_21, var60_64, var62_66 - var64_69);
                        System.arraycopy(var63_67, 0, var33_21, var60_64 + var62_66 - var64_69, var64_69);
                        var60_64 += var62_66;
                    } while (var67_1);
                    break;
                }
                ++var61_65;
lbl371:
                // 2 sources

                if (var61_65 < var34_22[0]) continue block32;
                System.out.println(n.a((String)"T(W\u0016\tT(W\u0016T\u001et\rUR\b(W\u0016\tT(W"));
                System.out.println(n.a((String)"\u000bh\u0011WR\b=X") + var24_10 + "/" + var30_18);
                System.out.println(n.a((String)"\u000bh\u0014@UA'") + var34_22[0] + "/" + var31_19);
                System.out.println(n.a((String)"\u0016f\u0000Z\u001c[") + var52_53[0]);
                var23_9.writeChars(n.a((String)">V \u0018"));
                var23_9.writeInt(var35_23[0]);
                var23_9.writeInt(var24_10);
                v21 = 0;
            } while (var67_1);
            break;
        }
        var61_65 = v21;
        if (!var67_1) ** GOTO lbl389
        block40: while (true) {
            var23_9.writeFloat(var26_11[var61_65]);
            var23_9.writeFloat(var27_12[var61_65]);
            var23_9.writeFloat(var28_13[var61_65]);
            do {
                ++var61_65;
lbl389:
                // 2 sources

                if (var61_65 < var24_10) continue block40;
                var23_9.writeChars(n.a((String)">V \u0018"));
                var23_9.writeInt(var34_22[0]);
                var23_9.writeByte(var52_53[0]);
                var61_65 = var24_10 % 23 * (var34_22[0] % 23);
                var62_66 = 0;
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl403
        block42: while (true) {
            if (var61_65 * var62_66 % 23 == 17) {
                var61_65 = var62_66;
                var62_66 = 24;
            }
            do {
                ++var62_66;
lbl403:
                // 2 sources

                if (var62_66 < 23) continue block42;
                var23_9.writeShort(var61_65);
                var62_66 = 0;
                var64_69 = 0;
            } while (var67_1);
            break;
        }
        if (!var67_1) ** GOTO lbl420
        do {
            var63_68 = var33_21[var64_69++];
            if (var52_53[0] > 3) {
                var23_9.writeByte(var63_68 + var64_69 % 57);
            }
            var65_70 = 0;
            if (!var67_1) ** GOTO lbl419
            do {
                var23_9.writeShort(var33_21[var64_69] - var62_66);
                var62_66 = var33_21[var64_69++];
                ++var65_70;
lbl419:
                // 2 sources

            } while (var65_70 < var63_68);
lbl420:
            // 2 sources

        } while (var64_69 < var35_23[0]);
        var22_8.close();
        var23_9.close();
        System.out.println(n.a((String)"\u001ch\u0017]"));
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
                        n5 = 123;
                        break;
                    }
                    case 1: {
                        n5 = 7;
                        break;
                    }
                    case 2: {
                        n5 = 120;
                        break;
                    }
                    case 3: {
                        n5 = 57;
                        break;
                    }
                    default: {
                        n5 = 38;
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

