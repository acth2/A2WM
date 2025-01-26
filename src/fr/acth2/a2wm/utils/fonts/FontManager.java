package fr.acth2.a2wm.utils.fonts;

import java.awt.*;

public class FontManager {
    public static Font loadFont(String path, int style, float size) {
        try {
            Font font = Font.createFont(Font.TRUETYPE_FONT, FontManager.class.getResourceAsStream(path)).deriveFont(style, size);
            GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
            ge.registerFont(font);
            return font;
        } catch (Exception e) {
            e.printStackTrace();
            return new Font("SansSerif", style, (int) size);
        }
    }
}
