package fr.acth2.a2wm.utils.x11;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.List;

import static fr.acth2.a2wm.utils.References.runCommand;

public class WindowIconExtractor {
    public static ImageIcon getWindowIcon(String windowId, int width, int height) {
        try {
            // First try to get WM_CLASS to find the application name
            List<String> wmClass = runCommand("xprop", "-id", windowId, "WM_CLASS");
            if (!wmClass.isEmpty()) {
                String[] parts = wmClass.get(0).split("=");
                if (parts.length > 1) {
                    String className = parts[1].trim().split(",")[0].replace("\"", "").trim();
                    if (!className.isEmpty()) {
                        String[] paths = {
                                System.getProperty("user.home") + "/.nix-profile/share/pixmaps/" + className + ".xpm",
                                System.getProperty("user.home") + "/.nix-profile/share/icons/hicolor/48x48/apps/" + className + ".xpm",
                                System.getProperty("user.home") + "/.nix-profile/share/icons/gnome/48x48/apps/" + className + ".xpm",
                                System.getProperty("user.home") + "/.nix-profile/share/icons/hicolor/48x48/apps/" + className + ".xpm",
                                System.getProperty("user.home") + "/.nix-profile/share/pixmaps/" + className + ".png",
                                System.getProperty("user.home") + "/.nix-profile/share/icons/hicolor/48x48/apps/" + className + ".png"
                        };

                        for (String path : paths) {
                            File iconFile = new File(path);
                            if (iconFile.exists()) {
                                ImageIcon icon = new ImageIcon(path);
                                return new ImageIcon(icon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH));
                            }
                        }
                    }
                }
            }

            List<String> iconData = runCommand("xwininfo", "-id", windowId, "-wm");
            if (!iconData.isEmpty()) {
                for (String line : iconData) {
                    if (line.contains("Icon:")) {
                        String iconPath = line.split("Icon:")[1].trim();
                        File iconFile = new File(iconPath);
                        if (iconFile.exists()) {
                            ImageIcon icon = new ImageIcon(iconPath);
                            return new ImageIcon(icon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH));
                        }
                    }
                }
            }

            return null;
        } catch (Exception e) {
            System.err.println("Error getting window icon: " + e.getMessage());
            return null;
        }
    }
}