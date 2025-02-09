package fr.acth2.a2wm.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Random;

import static fr.acth2.a2wm.utils.logger.Logger.err;
import static fr.acth2.a2wm.utils.logger.Logger.log;

public class References {
    public static final String NAME = "A2WM";
    public static final int VERSION = 1;
    public static final String GITHUB_LINK = "https://github.com/acth2/A2WM";
    public static final String GITHUB_ISSUE = GITHUB_LINK + "/issues";

    public static final String RESET = "\033[0m";
    public static final String YELLOW = "\033[33m";
    public static final Random rand = new Random();
    public static final File mainDir = new File(System.getProperty("user.home") + "/.a2wm");
    public static final File desktopDir = new File(mainDir.getAbsolutePath() + "/desktop");

    private static String OS = null;

    private static String getOsName() {
        if (OS == null) {
            OS = System.getProperty("os.name");
        }
        return OS;
    }

    public static void createDir(File directory) {
        if (directory.mkdir()) {
            log(YELLOW + directory.getName() + RESET + " created!");
        }else {
            err(directory.getName() + " failed to create!");
        }
    }

    public static List<String> runCommand(String... command) {
        List<String> lines = new ArrayList<>();
        ProcessBuilder builder = new ProcessBuilder(command);
        try {
            Process process = builder.start();
            try (BufferedReader reader = new BufferedReader(
                    new InputStreamReader(process.getInputStream()))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    lines.add(line);
                }
            }
            process.waitFor();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
        return lines;
    }

    public static int intFromRange(int min, int max) {
        return rand.nextInt(max - min + 1) + min;
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
