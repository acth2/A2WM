package fr.acth2.a2wm.components.taskbar;

import fr.acth2.a2wm.utils.finders.FontManager;
import fr.acth2.a2wm.utils.finders.ImageManager;
import fr.acth2.a2wm.utils.settings.SettingsManager;
import fr.acth2.a2wm.utils.swing.AntiAliasingLabel;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class TaskbarWindow extends JFrame {
    private JLabel timeLabel;
    private JLabel dateLabel;

    public SettingsManager settingsInstance = new SettingsManager();

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

        ImageIcon favicon = ImageManager.loadImage(settingsInstance.isDarkmode() ? settingsInstance.get("fav-dark-path", System.getProperty("user.home") + "/.a2wm/favicon-dark.png") : settingsInstance.get("fav-white-path", System.getProperty("user.home") + "/.a2wm/favicon-white.png"), 32, 28);
        JButton startButton = new JButton(favicon);
        startButton.setBorderPainted(false);
        startButton.setContentAreaFilled(false);
        startButton.setFocusPainted(false);
        startButton.setOpaque(false);

        startButton.setVisible(true);
        startButton.setAlignmentX(Component.LEFT_ALIGNMENT);

        startButton.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseEntered(MouseEvent e) {
                updateStartJBState(true, startButton);
            }

            @Override
            public void mouseExited(MouseEvent e) {
                updateStartJBState(false, startButton);
            }
        });
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

    private void updateStartJBState(boolean isHovered, JButton instance) {
        String key = "";
        String defaultPath = "";

        if (isHovered) {
            key = settingsInstance.isDarkmode() ? "fav-hovered-dark-path" : "fav-hovered-white-path";
            defaultPath = settingsInstance.isDarkmode()
                    ? System.getProperty("user.home") + "/.a2wm/fav-hovered-dark-path.png"
                    : System.getProperty("user.home") + "/.a2wm/fav-hovered-white-path.png";
        } else {
            key = settingsInstance.isDarkmode() ? "fav-dark-path" : "fav-white-path";
            defaultPath = settingsInstance.isDarkmode()
                    ? System.getProperty("user.home") + "/.a2wm/favicon-dark.png"
                    : System.getProperty("user.home") + "/.a2wm/favicon-white.png";
        }

        String imagePath = settingsInstance.get(key, defaultPath);
        System.out.println("Loading image from: " + imagePath);

        ImageIcon favicon = ImageManager.loadImage(imagePath, 32, 28);

        if (favicon != null) {
            instance.setIcon(favicon);
        } else {
            System.err.println("Failed to load image: " + imagePath);
        }
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
