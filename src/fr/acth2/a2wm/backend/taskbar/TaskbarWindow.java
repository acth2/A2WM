package fr.acth2.a2wm.backend.taskbar;

import fr.acth2.a2wm.utils.fonts.FontManager;

import javax.swing.*;
import java.awt.*;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class TaskbarWindow extends JFrame {
    private JLabel timeLabel;
    private JLabel dateLabel;
    private JPanel timeDatePanel;
    private Timer timer;

    public TaskbarWindow() {
        super("Taskbar");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setUndecorated(true);
        setAlwaysOnTop(true);

        initializeWindow();
        initTimer();
        setVisible(true);
    }

    private void initializeWindow() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int width = screenSize.width;
        int height = 32;

        setSize(width, height);
        setLocation(0, screenSize.height - height);

        JPanel rootPanel = new JPanel(new BorderLayout());
        rootPanel.setBackground(Color.DARK_GRAY);

        timeDatePanel = new JPanel();
        timeDatePanel.setLayout(new BoxLayout(timeDatePanel, BoxLayout.Y_AXIS));
        timeDatePanel.setOpaque(false);

        Font timeFont = FontManager.loadFont("/fonts/Roboto-Light.ttf", Font.PLAIN, 14);
        Font dateFont = FontManager.loadFont("/fonts/Roboto-Medium.ttf", Font.PLAIN, 12);

        timeLabel = new AntiAliasingLabel("00:00:00", timeFont, Color.WHITE);
        timeLabel.setAlignmentX(Component.RIGHT_ALIGNMENT);

        dateLabel = new AntiAliasingLabel("01/01/2025", dateFont, Color.WHITE);
        dateLabel.setAlignmentX(Component.RIGHT_ALIGNMENT);

        timeDatePanel.add(Box.createVerticalGlue());
        timeDatePanel.add(timeLabel);
        timeDatePanel.add(dateLabel);
        timeDatePanel.add(Box.createVerticalGlue());

        timeDatePanel.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 10));
        rootPanel.add(timeDatePanel, BorderLayout.EAST);

        setContentPane(rootPanel);
    }

    private void initTimer() {
        SimpleDateFormat timeFormat = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
        timeFormat.setTimeZone(TimeZone.getDefault());

        SimpleDateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy", Locale.getDefault());
        dateFormat.setTimeZone(TimeZone.getDefault());

        updateTime(timeFormat, dateFormat);
        updateWindow();

        timer = new Timer(1000, e -> updateTime(timeFormat, dateFormat));
        timer.start();
    }

    private void updateTime(SimpleDateFormat timeFormat, SimpleDateFormat dateFormat) {
        String currentTime = timeFormat.format(new Date());
        String currentDate = dateFormat.format(new Date());

        timeLabel.setText(currentTime);
        dateLabel.setText(currentDate);
    }

    public void updateWindow() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int width = screenSize.width;
        int height = 32;

        setSize(width, height);
        setLocation(0, screenSize.height - height);
        revalidate();
        repaint();

        System.out.println("Taskbar window updated to: " + width + "x" + height);
    }

    public void setTaskbarLabel(String text) {
        timeLabel.setText(text);
    }

    private static class AntiAliasingLabel extends JLabel {
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
            g2d.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

            g2d.setFont(getFont());
            g2d.setColor(getForeground());
            g2d.drawString(getText(), 0, getHeight() / 2 + 5);
        }
    }
}
