package fr.acth2.a2wm.components.taskbar;

import fr.acth2.a2wm.components.taskbar.menu.StartMenu;
import fr.acth2.a2wm.utils.manager.FontManager;
import fr.acth2.a2wm.utils.manager.ImageManager;
import fr.acth2.a2wm.utils.settings.SettingsManager;
import fr.acth2.a2wm.utils.swing.AntiAliasingLabel;
import fr.acth2.a2wm.utils.x11.MinimizedWindow;
import fr.acth2.a2wm.utils.x11.MinimizedWindowsChecker;

import javax.swing.*;
import javax.swing.Timer;
import java.awt.*;
import java.awt.event.*;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.List;

import static fr.acth2.a2wm.utils.logger.Logger.*;

public class TaskbarWindow extends JFrame {
    private Map<String, JButton> minimizedButtons = new HashMap<>();
    private static JPanel buttonsPane = new JPanel();
    private JLabel timeLabel;
    private JLabel dateLabel;
    private StartMenu startMenu;

    public SettingsManager settingsInstance = new SettingsManager();

    public static boolean isStartMenuActive = false;

    public TaskbarWindow() {
        super("A2WM-TASKBAR");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setUndecorated(true);
        setAlwaysOnTop(true);

        initializeWindow();
        loopManager();
        setVisible(true);
    }

    private void initializeWindow() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int width = screenSize.width;
        int height = Integer.parseInt(settingsInstance.get("taskBar-height", "32"));

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

            @Override
            public void mouseClicked(MouseEvent e) {
                StartMenu.getInstance().toggleVisibility(!StartMenu.getInstance().isVisible());
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
        rootPanel.add(timeDatePanel, BorderLayout.EAST);
        rootPanel.add(buttonsPane, null);
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
        log("Loading image from: " + imagePath);

        ImageIcon favicon = ImageManager.loadImage(imagePath, 32, 28);

        if (favicon != null) {
            instance.setIcon(favicon);
        } else {
            err("Failed to load image: " + imagePath);
        }
    }

    private void loopManager() {
        SimpleDateFormat timeFormat = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
        timeFormat.setTimeZone(TimeZone.getDefault());

        SimpleDateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy", Locale.getDefault());
        dateFormat.setTimeZone(TimeZone.getDefault());

        mainLoop(timeFormat, dateFormat);
        updateWindow();

        Timer timer = new Timer(1000, e -> mainLoop(timeFormat, dateFormat));
        timer.start();
    }

    private void mainLoop(SimpleDateFormat timeFormat, SimpleDateFormat dateFormat) {
        String currentTime = timeFormat.format(new Date());
        String currentDate = dateFormat.format(new Date());

        timeLabel.setText(currentTime);
        dateLabel.setText(currentDate);

        List<MinimizedWindow> minimized = MinimizedWindowsChecker.findMinimizedWindowsICCCM();
        Set<String> currentWindowIds = new HashSet<>();

        for (MinimizedWindow w : minimized) {
            currentWindowIds.add(w.getWindowId());
            if (!minimizedButtons.containsKey(w.getWindowId())) {
                JButton button = new JButton(w.getTitle());

                final MinimizedWindow window = w;

                button.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        restoreWindow(window);
                    }
                });

                buttonsPane.add(button);
                minimizedButtons.put(w.getWindowId(), button);
            }
        }

        Iterator<Map.Entry<String, JButton>> iterator = minimizedButtons.entrySet().iterator();
        while (iterator.hasNext()) {
            Map.Entry<String, JButton> entry = iterator.next();
            if (!currentWindowIds.contains(entry.getKey())) {
                buttonsPane.remove(entry.getValue());
                iterator.remove();
            }
        }

        buttonsPane.revalidate();
        buttonsPane.repaint();
    }

    public void updateWindow() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int width = screenSize.width;
        int height = 32;

        setSize(width, height);
        setLocation(0, screenSize.height - height);
        revalidate();
        repaint();

        log("Taskbar window updated to: " + width + "x" + height);
    }

    private void restoreWindow(MinimizedWindow window) {
        MinimizedWindowsChecker.restoreMinimized(window);
    }

    public void setTaskbarLabel(String text) {
        timeLabel.setText(text);
    }
}
