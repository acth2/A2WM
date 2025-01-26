package fr.acth2.a2wm.backend.taskbar;

import javax.swing.*;
import java.awt.*;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class TaskbarWindow extends JFrame {
    private JPanel rootPanel;
    private JLabel timeLabel;
    private JLabel dateLabel;
    private JPanel timeDatePanel;
    private Timer timer;

    public TaskbarWindow() {
        super("Taskbar");
        setContentPane(rootPanel);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setUndecorated(true);
        setAlwaysOnTop(true);

        initializeWindow();

        initClock();
        setVisible(true);
    }

    private void initializeWindow() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int width = screenSize.width;
        int height = 32;

        setSize(width, height);
        setLocation(0, screenSize.height - height);
    }
    private void initClock() {
        SimpleDateFormat timeFormat = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
        timeFormat.setTimeZone(TimeZone.getDefault());

        SimpleDateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy", Locale.getDefault());
        dateFormat.setTimeZone(TimeZone.getDefault());

        updateTime(timeFormat, dateFormat);

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

    private void createUIComponents() {
    }
}
