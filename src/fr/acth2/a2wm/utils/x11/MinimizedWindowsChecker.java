package fr.acth2.a2wm.utils.x11;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class MinimizedWindowsChecker {
    public static List<MinimizedWindow> findMinimizedWindowsICCCM() {
        List<MinimizedWindow> minimizedList = new ArrayList<>();

        List<String> wmctrlLines = runCommand("wmctrl", "-l");
        if (wmctrlLines.isEmpty()) {
            return minimizedList;
        }

        for (String line : wmctrlLines) {
            String[] parts = line.split("\\s+", 5);
            if (parts.length < 1) {
                continue;
            }

            String windowId = parts[0];
            List<String> xpropOutput = runCommand("xprop", "-id", windowId, "WM_STATE");
            boolean isIconic = false;
            for (String xLine : xpropOutput) {
                if (xLine.contains("window state: Iconic")) {
                    isIconic = true;
                    break;
                }
            }

            if (isIconic) {
                String title = "(untitled window)";
                if (parts.length >= 5) {
                    title = parts[4];
                }
                minimizedList.add(new MinimizedWindow(windowId, title));
            }
        }

        return minimizedList;
    }

    private static List<String> runCommand(String... command) {
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
}
