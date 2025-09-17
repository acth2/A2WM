package fr.acth2.a2wm.utils.logger;

import fr.acth2.a2wm.Wrapper;
import fr.acth2.a2wm.utils.References;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class Logger {
    private static final String LOG_DIR = System.getProperty("user.home") + "/.a2wm";
    private static final String LOG_FILE = LOG_DIR + "/a2wm.log";
    private static String lastLogContent = null;
    private static int repeatCount = 0;

    static {
        new File(LOG_DIR).mkdirs();
    }

    private static void checkLogFileSize() {
        File logFile = new File(LOG_FILE);
        if (logFile.length() > 1024 * 2048) {
            String backupFile = LOG_DIR + "/a2wm_" + System.currentTimeMillis() + ".log";
            logFile.renameTo(new File(backupFile));
        }
    }

    public static void log(String log) {
        handleLog(log, false);
    }

    public static void debugLog(String log) {
        if(Wrapper.atomicDebug.get()) {
            handleLog(References.YELLOW + "[DEBUG] " + References.RESET + log, false);
        }
    }

    public static void err(String err) {
        handleLog(err, true);
    }

    private static void handleLog(String message, boolean isError) {
        String formattedLog = formatLog(message);
        String logContent = message.trim();

        if (logContent.equals(lastLogContent)) {
            repeatCount++;
        } else {
            if (repeatCount > 0) {
                String repeatedLog = formatLog(lastLogContent) + " [REPEATED " + repeatCount + " TIMES]";
                writeToFile(repeatedLog);
            }

            writeToFile(formattedLog);
            lastLogContent = logContent;
            repeatCount = 0;
        }

        if (isError) {
            System.err.println(formattedLog);
        } else {
            System.out.println(formattedLog);
        }
    }

    private static String formatLog(String message) {
        DateTimeFormatter dtf = DateTimeFormatter.ofPattern("dd/MM/yyyy HH:mm:ss");
        LocalDateTime now = LocalDateTime.now();
        return "[" + dtf.format(now) + "] " + message;
    }

    private static void writeToFile(String log) {
        String cleanedLog = log.replaceAll("\u001B\\[[;\\d]*m", "");
        try (FileWriter writer = new FileWriter(LOG_FILE, true)) {
            writer.write(cleanedLog + System.lineSeparator());
            checkLogFileSize();
        } catch (IOException e) {
            System.err.println("Failed to write to log file: " + e.getMessage());
        }
    }

}