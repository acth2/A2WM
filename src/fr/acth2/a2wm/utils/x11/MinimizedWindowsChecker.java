package fr.acth2.a2wm.utils.x11;

import fr.acth2.a2wm.utils.References;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import static fr.acth2.a2wm.utils.References.runCommand;
import static fr.acth2.a2wm.utils.logger.Logger.*;

public class MinimizedWindowsChecker {

    private static AtomicBoolean atomicNLWarner = new AtomicBoolean(true);

    public static List<MinimizedWindow> findMinimizedWindowsICCCM() {
        List<MinimizedWindow> minimizedList = new ArrayList<>();

        if (!References.isLinux()) {
            if (atomicNLWarner.getAndSet(false)) {
                log("Your operating system is not a linux based one.");
                log("The MinimizedWindowsChecker class is then not going to work.");
            }
            return minimizedList;
        }

        List<String> wmctrlLines = runCommand("wmctrl", "-l");
        if (wmctrlLines.isEmpty()) {
            return minimizedList;
        }

        for (String line : wmctrlLines) {
            try {
                String[] parts = line.split("\\s+", 5);
                if (parts.length < 5) continue;

                String windowId = parts[0];
                String title = parts[4];

                List<String> xpropOutput = runCommand("xprop", "-id", windowId, "WM_STATE");
                boolean isMinimized = false;
                for (String xLine : xpropOutput) {
                    if (xLine.contains("window state: Iconic")) {
                        isMinimized = true;
                        break;
                    }
                }

                if (isMinimized) {
                    minimizedList.add(new MinimizedWindow(windowId, title));
                }
            } catch (Exception e) {
                System.err.println("Error processing window: " + line);
                e.printStackTrace();
            }
        }

        return minimizedList;
    }

    public static void restoreMinimized(MinimizedWindow window) {
        runCommand("wmctrl", "-i", "-R", window.getWindowId());
    }
}