package fr.acth2.a2wm.components.background;

import fr.acth2.a2wm.components.background.context.ContextMenu;
import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

import static fr.acth2.a2wm.utils.logger.Logger.*;

public class BackgroundWindow extends JFrame {
    private String currentImagePath = "";
    private JLabel backgroundLabel;
    private int currentWidth;
    private int currentHeight;

    public static final int yAxisReducer = -32;

    public BackgroundWindow() {
        super("A2WM-BACKGROUND");
        setUndecorated(true);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        setAlwaysOnTop(false);
        setFocusable(false);
        setFocusableWindowState(false);

        toBack();
        setType(Type.UTILITY);
        setResizable(false);

        Dimension newSize = Toolkit.getDefaultToolkit().getScreenSize();
        setBounds(0, yAxisReducer, newSize.width, newSize.height);
        setVisible(true);

        initComponents();
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showCustomContextMenu(e.getX(), e.getY() + yAxisReducer);
                    toBack();
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showCustomContextMenu(e.getX(), e.getY() + yAxisReducer);
                    toBack();
                }
            }
        });

        addWindowFocusListener(new WindowFocusListener() {
            @Override
            public void windowGainedFocus(WindowEvent e) {
                toBack();
            }

            @Override
            public void windowLostFocus(WindowEvent e) {}
        });

        toBack();
        SwingUtilities.invokeLater(this::toBack);

        setFocusable(false);
        mainLoop();
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


    private void mainLoop() {
        int delay = 100;

        Timer timer = new Timer(delay, e -> {
            SettingsManager settings = SettingsManager.getInstance();
            String imagePath = settings.get("imagePath", System.getProperty("user.home") + "/.a2wm/base.png");

            if (!imagePath.equals(currentImagePath)) {
                File imageFile = new File(imagePath);
                if (imageFile.exists() && imageFile.isFile()) {
                    log("Detected new image path: " + imagePath);
                    updateBackgroundImage(imagePath);
                    currentImagePath = imagePath;
                } else {
                    err("New image file does not exist at: " + imagePath);
                }
            }

            Dimension newSize = Toolkit.getDefaultToolkit().getScreenSize();
            if (newSize.width != currentWidth || newSize.height != currentHeight) {
                log("Detected screen resolution change to: " + newSize.width + "x" + newSize.height);
                currentWidth = newSize.width;
                currentHeight = newSize.height;
                updateBackgroundImage(currentImagePath);
                toBack();
            }

            toBack();
        });
        timer.start();
    }

    private void showCustomContextMenu(int x, int y) {
        new ContextMenu(x, y).showContext(x, y);
    }

    private void updateBackgroundImage(String imagePath) {
        File imageFile = new File(imagePath);
        if (!imageFile.exists()) {
            err("Image file does not exist at: " + imagePath);
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

        log("Background image updated to: " + imagePath);
    }

    @Override
    public void toFront() {
        toBack();
        toBack();
    }

    @Override
    protected boolean requestFocus(boolean temporary) {
        return false;
    }
}
