/*
 * Decompiled with CFR 0.152.
 */
public class f
extends YmMain {
    public String en;
    public int em;

    /*
     * Enabled aggressive block sorting
     */
    public final int al(String string) {
        this.em = 0;
        int n = 0;
        while (n < string.length()) {
            this.em += string.charAt(n);
            ++n;
        }
        return this.em;
    }

    public final String ak() {
        return this.en;
    }

    public f() {
        f.ai("}\u0011Kd");
        this.em = 1985;
        this.en = f.ai("l\bW~\u0013f\u0001\fg\u0011h\u0017GcPmWLr\t");
        if (this.z != 1985) {
            this.aj = null;
        }
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
                    n3 = 9;
                    break;
                }
                case 1: {
                    n3 = 121;
                    break;
                }
                case 2: {
                    n3 = 34;
                    break;
                }
                case 3: {
                    n3 = 23;
                    break;
                }
                default: {
                    n3 = 125;
                }
            }
            cArray[n4] = (char)(c2 ^ n3);
            ++n2;
        }
        return new String(cArray);
    }
}

