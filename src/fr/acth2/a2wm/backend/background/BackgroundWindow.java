package fr.acth2.a2wm.backend.background;

import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

public class BackgroundWindow extends JFrame {

    public BackgroundWindow() {
        setUndecorated(true);
        setAlwaysOnTop(true);

        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        GraphicsDevice gd = ge.getDefaultScreenDevice();

        if (gd.isFullScreenSupported()) {
            gd.setFullScreenWindow(this);
        } else {
            setExtendedState(JFrame.MAXIMIZED_BOTH);
            setVisible(true);
        }

        initComponents();
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

        addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.isControlDown() && e.getKeyCode() == KeyEvent.VK_Q) {
                    System.exit(0);
                }
            }
        });

        setFocusable(true);
        requestFocusInWindow();
    }

    private void initComponents() {
        setLayout(new BorderLayout());

        SettingsManager settings = SettingsManager.getInstance();
        String imagePath = settings.get("backgroundPath", "/usr/cydra/backgrounds/current.png");

        ImageIcon backgroundImage = new ImageIcon(imagePath);

        if (backgroundImage.getIconWidth() == -1) {
            System.err.println("Image not found at: " + imagePath);
            getContentPane().setBackground(Color.BLACK);
            return;
        }

        Image scaledImage = backgroundImage.getImage().getScaledInstance(
                Toolkit.getDefaultToolkit().getScreenSize().width,
                Toolkit.getDefaultToolkit().getScreenSize().height,
                Image.SCALE_SMOOTH
        );
        backgroundImage = new ImageIcon(scaledImage);

        JLabel backgroundLabel = new JLabel(backgroundImage);
        backgroundLabel.setHorizontalAlignment(JLabel.CENTER);
        backgroundLabel.setVerticalAlignment(JLabel.CENTER);

        add(backgroundLabel, BorderLayout.CENTER);
    }
}