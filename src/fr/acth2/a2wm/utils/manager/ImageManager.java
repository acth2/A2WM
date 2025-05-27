package fr.acth2.a2wm.utils.manager;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;

public class ImageManager {
    public static ImageIcon loadImage(String path, int width, int height) {
        try {
            String expandedPath = path.replace("~", System.getProperty("user.home"));
            ImageIcon icon = new ImageIcon(expandedPath);

            if (width > 0 && height > 0) {
                return new ImageIcon(icon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH));
            }
            return icon;
        } catch (Exception e) {
            System.err.println("Error loading image: " + path);
            e.printStackTrace();

            BufferedImage blankImage = new BufferedImage(
                    Math.max(1, width),
                    Math.max(1, height),
                    BufferedImage.TYPE_INT_ARGB
            );
            return new ImageIcon(blankImage);
        }
    }

}
