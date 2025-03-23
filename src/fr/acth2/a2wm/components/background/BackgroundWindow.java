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
    private final Set<String> addedFilePaths = new HashSet<>();
    private final Map<String, JButton> pathToButtonMap = new HashMap<>();
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
        initGridOverlay();
        startFileSystemWatcher();
        mainLoop();
    }

    private void initComponents() {
        setLayout(new BorderLayout());
        String imagePath = settings.get("imagePath", mainDir.getPath() + "/.a2wm/base.png");
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
            String imagePath = settings.get("imagePath", mainDir.getPath() + "/.a2wm/base.png");

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

            for (Map.Entry<String, JButton> entry : pathToButtonMap.entrySet()) {
                JButton button = entry.getValue();
                String filePath = entry.getKey();
                Rectangle buttonBounds = button.getBounds();

                for (Map.Entry<String, JButton> otherEntry : pathToButtonMap.entrySet()) {
                    JButton otherButton = otherEntry.getValue();
                    if (button == otherButton) {
                        continue;
                    }

                    Rectangle otherButtonBounds = otherButton.getBounds();
                    if (buttonBounds.intersects(otherButtonBounds)) {
                        log("Collision detected between " + entry.getKey() + " and " + otherEntry.getKey());

                        if (otherButton.getText().endsWith("/")) {
                            File sourceFile = new File(filePath);
                            File targetFolder = new File(otherEntry.getKey());

                            if (moveFileToFolder(sourceFile, targetFolder, button)) {
                                log("File moved and UI updated for: " + filePath);
                            } else {
                                err("Failed to move file: " + filePath);
                            }
                        }
                    }
                }
            }

            updateDesktopIcons();
            toBack();
        });
        timer.start();
    }

    private void updateDesktopIcons() {
        log("Updating desktop icons...");
        Color fileColor = new Color(3, 30, 82, 255);
        Color dirColor = new Color(14, 128, 110, 255);

        File[] files = desktopDir.listFiles();
        if (files == null) {
            err("Failed to list files in desktop directory: " + desktopDir.getAbsolutePath());
            return;
        }

        Set<String> currentFilePaths = new HashSet<>();
        for (File file : files) {
            currentFilePaths.add(file.getAbsolutePath());
        }

        Iterator<String> iterator = addedFilePaths.iterator();
        while (iterator.hasNext()) {
            String path = iterator.next();
            if (!currentFilePaths.contains(path)) {
                log("Detected deleted or moved file: " + path);
                JButton button = pathToButtonMap.get(path);
                if (button != null) {
                    gridOverlayPanel.remove(button);
                    pathToButtonMap.remove(path);
                }
                iterator.remove();
            }
        }

        for (File file : files) {
            String absolutePath = file.getAbsolutePath();
            if (!addedFilePaths.contains(absolutePath)) {
                log("Detected new or renamed file: " + absolutePath);
                JButton button = createIconButton(file, file.isFile() ? fileColor : dirColor);
                gridOverlayPanel.add(button);
                addedFilePaths.add(absolutePath);
                pathToButtonMap.put(absolutePath, button);
            }
        }

        gridOverlayPanel.revalidate();
        gridOverlayPanel.repaint();
        log("Updated desktop icons. Total files: " + currentFilePaths.size());
    }

    private JButton createIconButton(File file, Color color) {
        JButton button = new JButton(file.isFile() ? file.getName() : file.getName() + "/");
        button.setOpaque(true);
        button.setContentAreaFilled(true);
        button.setBackground(color);
        button.setBorder(null);

        button.setBounds(
                intFromRange(0, getToolkit().getScreenSize().width - 120),
                intFromRange(0, getToolkit().getScreenSize().height - 40),
                120,
                40
        );

        log("Creating button for file: " + file.getName());

        Point offset = new Point();
        button.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                offset.x = e.getX();
                offset.y = e.getY();
                log("Mouse pressed on button: " + button.getText());
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                log("Mouse released on button: " + button.getText());
            }
        });

        button.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                int newX = button.getX() + e.getX() - offset.x;
                int newY = button.getY() + e.getY() - offset.y;
                button.setLocation(newX, newY);
                log("Button dragged: " + button.getText() + " to (" + newX + ", " + newY + ")");
            }
        });

        return button;
    }

    private String getFilePathFromButton(JButton button) {
        return desktopDir.getAbsolutePath() + button.getText();
    }

    private boolean moveFileToFolder(File sourceFile, File targetFolder, JButton fileButton) {
        toBack();
        System.out.println("Attempting to move file: " + sourceFile.getName() + " to folder: " + targetFolder.getName());

        File newFile = new File(targetFolder, sourceFile.getName());
        if (sourceFile.renameTo(newFile)) {
            log("Moved file: " + sourceFile.getName() + " to folder: " + targetFolder.getName());
            gridOverlayPanel.remove(fileButton);
            addedFilePaths.remove(sourceFile.getAbsolutePath());
            pathToButtonMap.remove(sourceFile.getAbsolutePath());

            gridOverlayPanel.revalidate();
            gridOverlayPanel.repaint();

            updateDesktopIcons();
            return true;
        } else {
            err("Failed to move file: " + sourceFile.getName() + " to folder: " + targetFolder.getName());
            return false;
        }
    }

    private void showCustomContextMenu(int x, int y) {
        new ContextMenu(x, y).showContext(x, y);
    }

    private void startFileSystemWatcher() {
        Thread watcherThread = new Thread(() -> {
            toBack();
            try {
                WatchService watchService = FileSystems.getDefault().newWatchService();
                Path desktopPath = Paths.get(desktopDir.getAbsolutePath());
                desktopPath.register(watchService, StandardWatchEventKinds.ENTRY_CREATE, StandardWatchEventKinds.ENTRY_DELETE, StandardWatchEventKinds.ENTRY_MODIFY);

                while (true) {
                    WatchKey key = watchService.take();
                    for (WatchEvent<?> event : key.pollEvents()) {
                        WatchEvent.Kind<?> kind = event.kind();

                        if (kind == StandardWatchEventKinds.OVERFLOW) {
                            continue;
                        }

                        SwingUtilities.invokeLater(this::updateDesktopIcons);
                    }

                    boolean valid = key.reset();
                    if (!valid) {
                        break;
                    }
                }
            } catch (Exception e) {
                err("File system watcher error: " + e.getMessage());
            }
        });

        watcherThread.setDaemon(true);
        watcherThread.start();
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