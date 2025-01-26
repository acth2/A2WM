package fr.acth2.a2wm.utils.swing;

import javax.swing.*;
import java.awt.*;

public class AntiAliasingLabel extends JLabel {
    private final Font customFont;
    private final Color textColor;

    public AntiAliasingLabel(String text, Font font, Color color) {
        super(text);
        this.customFont = font;
        this.textColor = color;
        setFont(customFont);
        setForeground(textColor);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_LCD_HRGB);

        // Now let the default text rendering occur with anti-aliasing
        g2d.setFont(getFont());
        g2d.setColor(getForeground());
        g2d.drawString(getText(), 0, getHeight() / 2 + 5);  // Adjust vertical position if necessary
    }
}