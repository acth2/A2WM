package fr.acth2.a2wm.utils.x11;

import java.util.ArrayList;
import java.util.List;

import static fr.acth2.a2wm.utils.References.*;

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

    public static void restoreMinimized(MinimizedWindow window) {
        runCommand("wmctrl", "-i", "-R", window.getWindowId());
    }
}
