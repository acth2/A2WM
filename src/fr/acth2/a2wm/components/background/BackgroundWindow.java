package fr.acth2.a2wm.components.background;

import fr.acth2.a2wm.components.background.context.ContextMenu;
import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.naming.Context;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

public class BackgroundWindow extends JFrame {
    private String currentImagePath = "";
    private JLabel backgroundLabel;
    private int currentWidth;
    private int currentHeight;

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
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) { }
        });

        addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.isControlDown() && e.getKeyCode() == KeyEvent.VK_Q) {
                    System.exit(0);
                }
            }
        });

        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showCustomContextMenu(e.getXOnScreen(), e.getYOnScreen());
                }
            }
        });


        setFocusable(true);
        requestFocusInWindow();
        initSettingsMonitor();
        initResolutionMonitor();
    }

    private void initComponents() {
        setLayout(new BorderLayout());

        SettingsManager settings = SettingsManager.getInstance();
        String imagePath = settings.get("imagePath", System.getProperty("user.home") + "/.a2wm/base.png");
        currentImagePath = imagePath;

        backgroundLabel = new JLabel();
        backgroundLabel.setHorizontalAlignment(JLabel.CENTER);
        backgroundLabel.setVerticalAlignment(JLabel.CENTER);

        updateBackgroundImage(imagePath);

        add(backgroundLabel, BorderLayout.CENTER);
    }

    private void initResolutionMonitor() {
        int delay = 2000;

        Timer timer = new Timer(delay, e -> {
            Dimension newSize = Toolkit.getDefaultToolkit().getScreenSize();
            if (newSize.width != currentWidth || newSize.height != currentHeight) {
                System.out.println("Detected screen resolution change to: " + newSize.width + "x" + newSize.height);
                currentWidth = newSize.width;
                currentHeight = newSize.height;
                updateBackgroundImage(currentImagePath);
            }
        });
        timer.start();
    }

    private void initSettingsMonitor() {
        int delay = 2000;

        Timer timer = new Timer(delay, e -> {
            SettingsManager settings = SettingsManager.getInstance();
            String imagePath = settings.get("imagePath", System.getProperty("user.home") + "/.a2wm/base.png");

            if (!imagePath.equals(currentImagePath)) {
                File imageFile = new File(imagePath);
                if (imageFile.exists() && imageFile.isFile()) {
                    System.out.println("Detected new image path: " + imagePath);
                    updateBackgroundImage(imagePath);
                    currentImagePath = imagePath;
                } else {
                    System.err.println("New image file does not exist at: " + imagePath);
                }
            }
        });

        timer.start();
    }

    private void showCustomContextMenu(int x, int y) {
        ContextMenu contextMenu = new ContextMenu();
        contextMenu.setLocation(x, y);
        contextMenu.setVisible(true);
    }

    private void updateBackgroundImage(String imagePath) {
        File imageFile = new File(imagePath);
        if (!imageFile.exists()) {
            System.err.println("Image file does not exist at: " + imagePath);
            getContentPane().setBackground(Color.BLACK);
            backgroundLabel.setIcon(null);
            return;
        }

        ImageIcon backgroundImage = new ImageIcon(imagePath);
        Image scaledImage = backgroundImage.getImage().getScaledInstance(
                Toolkit.getDefaultToolkit().getScreenSize().width,
                Toolkit.getDefaultToolkit().getScreenSize().height,
                Image.SCALE_SMOOTH
        );
        backgroundImage = new ImageIcon(scaledImage);

        backgroundLabel.setIcon(backgroundImage);
        backgroundLabel.revalidate();
        backgroundLabel.repaint();

        System.out.println("Background image updated to: " + imagePath);
    }
}
