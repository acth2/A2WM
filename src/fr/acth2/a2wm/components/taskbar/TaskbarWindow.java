package fr.acth2.a2wm.components.taskbar;

import fr.acth2.a2wm.utils.finders.FontManager;
import fr.acth2.a2wm.utils.finders.ImageManager;
import fr.acth2.a2wm.utils.settings.SettingsManager;
import fr.acth2.a2wm.utils.swing.AntiAliasingLabel;

import javax.swing.*;
import java.awt.*;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class TaskbarWindow extends JFrame {
    private JLabel timeLabel;
    private JLabel dateLabel;

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

        JPanel timeDatePanel = new JPanel();
        timeDatePanel.setLayout(new BoxLayout(timeDatePanel, BoxLayout.Y_AXIS));
        timeDatePanel.setOpaque(false);

        ImageIcon favicon = ImageManager.loadImage(new SettingsManager().get("fav-path", System.getProperty("user.home") + "/.a2wm/favicon-dark.png"), 24, 24);
        JButton startButton = new JButton(favicon);
        startButton.setBorderPainted(false);
        startButton.setContentAreaFilled(false);
        startButton.setFocusPainted(false);
        startButton.setOpaque(false);

        startButton.setVisible(true);
        startButton.setAlignmentX(Component.LEFT_ALIGNMENT);

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
        rootPanel.add(timeDatePanel, BorderLayout.CENTER);
        rootPanel.add(startButton, BorderLayout.WEST);

        setContentPane(rootPanel);
    }

    private void initTimer() {
        SimpleDateFormat timeFormat = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
        timeFormat.setTimeZone(TimeZone.getDefault());

        SimpleDateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy", Locale.getDefault());
        dateFormat.setTimeZone(TimeZone.getDefault());

        updateTime(timeFormat, dateFormat);
        updateWindow();

        Timer timer = new Timer(1000, e -> updateTime(timeFormat, dateFormat));
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
}
