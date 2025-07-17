package fr.acth2.a2wm.components.background;

import fr.acth2.a2wm.components.background.context.ContextMenu;
import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.swing.*;
import javax.swing.Timer;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.nio.file.*;
import java.util.*;

import static fr.acth2.a2wm.utils.References.*;
import static fr.acth2.a2wm.utils.logger.Logger.*;

public class BackgroundWindow extends JFrame {
    private final SettingsManager settings = SettingsManager.getInstance();
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
        setAutoRequestFocus(false);

        setType(Type.NORMAL);
        setResizable(false);

        Dimension newSize = Toolkit.getDefaultToolkit().getScreenSize();
        setBounds(0, yAxisReducer, newSize.width, newSize.height);
        setVisible(true);

        initComponents();

        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    setAlwaysOnTop(true);
                    showCustomContextMenu(e.getX(), e.getY() + yAxisReducer);
                    EventQueue.invokeLater(() -> {
                        setAlwaysOnTop(false);
                        toBack();
                    });
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    setAlwaysOnTop(true);
                    showCustomContextMenu(e.getX(), e.getY() + yAxisReducer);
                    EventQueue.invokeLater(() -> {
                        setAlwaysOnTop(false);
                        toBack();
                    });
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

        addWindowListener(new WindowAdapter() {
            @Override
            public void windowGainedFocus(WindowEvent e) {
                EventQueue.invokeLater(() -> {
                    setFocusableWindowState(false);
                    toBack();
                });
            }

            @Override
            public void windowActivated(WindowEvent e) {
                EventQueue.invokeLater(() -> {
                    setAlwaysOnTop(false);
                    toBack();
                });
            }
        });

        toBack();
        SwingUtilities.invokeLater(this::toBack);

        setFocusable(false);
        initGridOverlay();
        mainLoop();
    }

    private void initComponents() {
        setLayout(new BorderLayout());
        String imagePath = settings.get("imagePath", mainDir.getPath() + "/base.png");
        currentImagePath = imagePath;

        backgroundLabel = new JLabel();
        backgroundLabel.setHorizontalAlignment(JLabel.CENTER);
        backgroundLabel.setVerticalAlignment(JLabel.CENTER);

        updateBackgroundImage(imagePath);
        add(backgroundLabel, BorderLayout.CENTER);
    }

    private void mainLoop() {
        int delay = 250;
        Timer timer = new Timer(delay, e -> {
            SettingsManager settings = SettingsManager.getInstance();
            String imagePath = settings.get("imagePath", mainDir.getPath() + "/base.png");

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
        toBack();
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
        toBack();
    }

    @Override
    protected boolean requestFocus(boolean temporary) {
        return false;
    }

    private final int gridRows = Integer.parseInt(settings.get("desktopGridRows", "16"));
    private final int gridCols = Integer.parseInt(settings.get("desktopGridCols", "16"));
    private JPanel gridOverlayPanel;

    private void initGridOverlay() {
        gridOverlayPanel = new JPanel(null);
        gridOverlayPanel.setOpaque(false);
        gridOverlayPanel.setBounds(0, 0, getWidth(), getHeight());
        getLayeredPane().add(gridOverlayPanel, JLayeredPane.PALETTE_LAYER);

        for (int i = 0; i < gridRows; i++) {
            for (int j = 0; j < gridCols; j++) {
                JPanel cell = new JPanel(new BorderLayout());
                cell.setOpaque(false);
                cell.setBorder(null);
                gridOverlayPanel.add(cell);
            }
        }
    }
}