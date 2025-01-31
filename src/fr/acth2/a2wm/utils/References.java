package fr.acth2.a2wm.utils;

import java.util.Locale;
import java.util.Objects;

public class References {
    public static final String NAME = "A2WM";
    public static final int VERSION = 1;
    public static final String GITHUB_LINK = "https://github.com/acth2/A2WM";
    public static final String GITHUB_ISSUE = GITHUB_LINK + "/issues";

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
