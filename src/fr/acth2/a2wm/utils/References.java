package fr.acth2.a2wm.utils;

import java.util.Locale;
import java.util.Objects;

public class References {
    public static final String NAME = "A2WM";
    public static final int VERSION = 1;

    private static String OS = null;

    private static String getOsName() {
        if (OS == null) {
            OS = System.getProperty("os.name");
        }
        return OS;
    }

    public static boolean isLinux() {
        String os = getOsName().toLowerCase(Locale.ENGLISH);
        return os.contains("linux");
    }
}
