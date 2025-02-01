package fr.acth2.a2wm.utils;

import java.io.File;
import java.util.Locale;
import java.util.Objects;

public class References {
    public static final String NAME = "A2WM";
    public static final int VERSION = 1;
    public static final String GITHUB_LINK = "https://github.com/acth2/A2WM";
    public static final String GITHUB_ISSUE = GITHUB_LINK + "/issues";

    public static final String RESET = "\u001B[0m";
    public static final String YELLOW = "\u001B[33m";

    private static String OS = null;

    private static String getOsName() {
        if (OS == null) {
            OS = System.getProperty("os.name");
        }
        return OS;
    }

    public static boolean isAppAvailable(String appName) {
        String pathEnv = System.getenv("PATH");
        if (pathEnv == null) {
            return false;
        }
        String[] paths = pathEnv.split(":");
        for (String path : paths) {
            File file = new File(path, appName);
            if (file.exists() && file.canExecute()) {
                return true;
            }
        }
        return false;
    }

    public static boolean isLinux() {
        String os = getOsName().toLowerCase(Locale.ENGLISH);
        return os.contains("linux");
    }
}
