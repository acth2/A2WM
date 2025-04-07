package fr.acth2.a2wm;

import com.formdev.flatlaf.FlatDarkLaf;
import fr.acth2.a2wm.components.background.BackgroundWindow;
import fr.acth2.a2wm.components.taskbar.TaskbarWindow;
import javax.swing.*;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;

import static fr.acth2.a2wm.utils.References.*;
import static fr.acth2.a2wm.utils.logger.Logger.*;

public class Wrapper {
    public static void main(String[] args) {
        AtomicBoolean startgui = new AtomicBoolean(true);
        AtomicBoolean force = new AtomicBoolean(false);
        for (String arg : args) {
            if ("--help".equalsIgnoreCase(arg)) {
                log(YELLOW + "A2WM Help page ---- [1]" + RESET);
                log("--no-gui: Does not start the gui");
                log("--force:  Force the start of the WM");
                System.exit(0);
            }

            if ("--no-gui".equalsIgnoreCase(arg)) {
                log(YELLOW + "--no-gui argument found" + RESET);
                startgui.set(false);
            }

            if ("--force".equalsIgnoreCase(arg)) {
                log(YELLOW + "--force argument found" + RESET);
                force.set(true);
            }
        }

        if(!isLinux()) {
            err("The window-manager is not compatible with non-unix os.");
            System.exit(0);
        }

        if (anotherDesktopEnvironmentRunning()) {
            err("Another desktop environment is already running in your system.");
            err("That can cause major issues, if you still want to continue, use the argument --force");
            if (!force.get()) {
                System.exit(0);
            }
        }

        verifyFiles();

        String displayEnv = System.getenv("DISPLAY");
        if (displayEnv != null && !displayEnv.isEmpty()) {
            startWM(startgui.get());
        } else {
            String sessionType = System.getenv("XDG_SESSION_TYPE");
            if ("x11".equalsIgnoreCase(sessionType)) {
                startWM(startgui.get());
            } else if ("wayland".equalsIgnoreCase(sessionType)) {
                log("A2WM is not likely to work on wayland.\nPlease use X11, however you can still trick the wm and start it anyway by defining the variable DISPLAY");
                System.exit(0);
            } else {
                err("ERR: None DISPLAY + None x11 or wayland in $XDG_SESSION_TYPE. Do you have a started X11 session?");
                System.exit(1);
            }
        }
    }

    private static void verifyFiles() {
        if (!mainDir.exists()) {
            createDir(desktopDir);
        }else {
            if (!desktopDir.exists()) {
                createDir(desktopDir);
            }
        }
    }

    private static boolean anotherDesktopEnvironmentRunning() {
        String xdgDesktop = System.getenv("XDG_CURRENT_DESKTOP");
        String desktopSession = System.getenv("DESKTOP_SESSION");
        String gdmSession = System.getenv("GDMSESSION");

        if (xdgDesktop != null || desktopSession != null || gdmSession != null) {
            return true;
        }

        try {
            Process dbusCheck = Runtime.getRuntime().exec(new String[]{"sh", "-c", "dbus-send --session --dest=org.freedesktop.DBus --type=method_call --print-reply /org/freedesktop/DBus org.freedesktop.DBus.ListNames"});
            dbusCheck.waitFor();
            if (dbusCheck.exitValue() == 0) {
                return true;
            }
        } catch (Exception ignored) { }

        Set<String> deProcesses = new HashSet<>(Arrays.asList(
                "gnome-shell", "plasmashell", "xfce4-panel", "cinnamon", "mate-panel", "lxqt-panel", "budgie-panel"
        ));

        try {
            Process psProcess = Runtime.getRuntime().exec(new String[]{"sh", "-c", "ps -e -o comm="});
            BufferedReader reader = new BufferedReader(new InputStreamReader(psProcess.getInputStream()));
            String line;
            while ((line = reader.readLine()) != null) {
                if (deProcesses.contains(line.trim())) {
                    return true;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }
    private static void startWM(boolean startgui) {
        if (startgui) {
            if (isAppAvailable("wmctrl")) {
                log(YELLOW + "STARTING " + RESET + NAME);
                log(YELLOW + "VERSION " + RESET + VERSION);

                try {
                    UIManager.setLookAndFeel(new FlatDarkLaf());
                    SwingUtilities.invokeLater(() -> {
                        BackgroundWindow backgroundWindow = new BackgroundWindow();
                    });

                    SwingUtilities.invokeLater(() -> {
                        TaskbarWindow taskbarWindow = new TaskbarWindow();
                    });
                } catch (Exception exception) {
                    err(NAME + " CRASHED!\n");
                    exception.printStackTrace();
                }
            } else {
                err("The application 'wmctrl' is not found in your PATH.");
                err("Please install this software and restart the window-manager");
            }
        }
    }
}
