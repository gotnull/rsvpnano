/*
 * Decompiled with CFR 0.152.
 */
public class e {
    public final void aj(String[] stringArray) {
        stringArray[0] = e.ai(" 9ONe\u0010x\r\u000f7LvaHpC\u0017ODt");
        stringArray[1] = e.ai("3/Nbx\u000f9QR9\u0019?S");
        stringArray[2] = e.ai(" 9SQr\u0011x\r\u000f7LvaHpC\u0017ODt");
        stringArray[3] = e.ai(" 9SQr\u0011xYHg");
        stringArray[4] = e.ai("13BM~\u0017/\u0003\u000e7!?D\u0001V\u000f3@");
        stringArray[5] = e.ai("13BM~\u0017/\r[~\u0013");
        stringArray[6] = e.ai("0\"BOs\u0001/\u0003\u000e7!?D\u0001V\u000f3@");
        stringArray[7] = e.ai("0\"BOs\u0001/\r[~\u0013");
        stringArray[8] = e.ai("\":ONnC\u0004VO7Lvn@sC\u001bBY");
        stringArray[9] = e.ai("6\tNDy\u0016xYHg");
        stringArray[10] = e.ai(".9MFx\u000f?B\u00018C\u001bBE7.7[");
        stringArray[11] = e.ai(".9MFx\u000f?B\u000fm\n&");
        stringArray[12] = e.ai("-3BS{\u001avpQ\u007f\u0006$JBv\u000fv\f\u0001Z\u00022\u0003lv\u001b");
        stringArray[13] = e.ai("-3PQ\u007f\u0006$\r[~\u0013");
        stringArray[14] = e.ai("0/MUv\u001bvwDe\u00119Q\u00018C\u001bBE7.7[");
        stringArray[15] = e.ai("0/MUv\u001be\r[~\u0013");
        stringArray[16] = e.ai("68JNyC\u0015LQ~\u0006$\u0003\u000e7.7G\u0001Z\u0002.");
        stringArray[17] = e.ai("6\t@Ng\n3Q\u000fm\n&");
        stringArray[18] = e.ai("0?G\u0001Z\u0016%JB7Qv\f\u0001Z\u00022\u0003lv\u001b");
        stringArray[19] = e.ai("0?Glb\u0010?@\u00139\u0019?S");
        stringArray[20] = e.ai("5?QUb\u0002:\u0003dd\u00007SD7&8G\u00018C\u001bBE7.7[");
        stringArray[21] = e.ai("53\u0010\u000fm\n&");
        stringArray[22] = e.ai("+/AS~\u0010v\f\u0001T\u000b$JRg\u001avmNx\u0007:F");
        stringArray[23] = e.ai("+/AS~\u0010xYHg");
        stringArray[24] = e.ai(" /ADe\r9JE7Qv\f\u0001S\u0002 JE74>JUc\u00025HDe");
        stringArray[25] = e.ai(" /ADe\r9JE%M,JQ");
        stringArray[26] = e.ai("0&FDs\u00017OM7Lvg@a\n2\u0003v\u007f\n\"W@t\b3Q");
        stringArray[27] = e.ai("0&FDs\u00017OM&M,JQ");
        stringArray[28] = e.ai(";3MNyCy\u0003ev\u0015?G\u0001@\u000b?WUv\u0000=FS");
        stringArray[29] = e.ai(";3MNyR{\u0012\u000fm\n&");
        stringArray[30] = e.ai("!#AC{\u0006vaNu\u0001:F\u00018C\u0002JL7%9OM~\r");
        stringArray[31] = e.ai("!#AC{\u0006\u0014LCu\u000f3\r[~\u0013");
    }

    private static String ai(String string) {
        char[] cArray = string.toCharArray();
        int n = cArray.length;
        int n2 = 0;
        while (n2 < n) {
            int n3;
            int n4 = n2;
            char c2 = cArray[n4];
            switch (n2 % 5) {
                case 0: {
                    n3 = 99;
                    break;
                }
                case 1: {
                    n3 = 86;
                    break;
                }
                case 2: {
                    n3 = 35;
                    break;
                }
                case 3: {
                    n3 = 33;
                    break;
                }
                default: {
                    n3 = 23;
                }
            }
            cArray[n4] = (char)(c2 ^ n3);
            ++n2;
        }
        return new String(cArray);
    }
}

