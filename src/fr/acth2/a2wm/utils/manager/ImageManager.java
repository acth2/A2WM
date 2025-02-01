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
            BufferedImage image = ImageIO.read(new File(expandedPath));

            Image scaledImage = image.getScaledInstance(width, height, Image.SCALE_SMOOTH);

            return new ImageIcon(scaledImage);
        } catch (Exception e) {
            e.printStackTrace();

            return new ImageIcon(new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB));
        }
    }

}
