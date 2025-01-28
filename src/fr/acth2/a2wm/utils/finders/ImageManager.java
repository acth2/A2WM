package fr.acth2.a2wm.utils.finders;

import com.sun.deploy.ui.ImageLoader;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

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
