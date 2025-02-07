package fr.acth2.a2wm.components.background;

import fr.acth2.a2wm.components.background.context.ContextMenu;
import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.swing.*;
import javax.swing.Timer;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.util.*;
import java.util.List;

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
        mainLoop();
    }

    private void initComponents() {
        setLayout(new BorderLayout());
        String imagePath = settings.get("imagePath", System.getProperty("user.home") + "/.a2wm/base.png");
        currentImagePath = imagePath;

        backgroundLabel = new JLabel();
        backgroundLabel.setHorizontalAlignment(JLabel.CENTER);
        backgroundLabel.setVerticalAlignment(JLabel.CENTER);

        updateBackgroundImage(imagePath);
        add(backgroundLabel, BorderLayout.CENTER);
    }


    private void mainLoop() {
        int delay = 25;

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
            Color fileColor = new Color(3, 30, 82, 255);
            Color dirColor = new Color(14, 128, 110, 255);

            for (File file : desktopDir.listFiles()) {
                String absolutePath = file.getAbsolutePath();

                if (!addedFilePaths.contains(absolutePath)) {
                    JButton button = new JButton(file.getName());
                    button.setOpaque(true);
                    button.setContentAreaFilled(true);
                    button.setBackground(file.isFile() ? fileColor : dirColor);
                    button.setBorder(null);

                    button.setBounds(intFromRange(0, getToolkit().getScreenSize().width), intFromRange(0, getToolkit().getScreenSize().height), 120, 40);
                    Point offset = new Point();
                    button.addMouseListener(new MouseAdapter() {
                        @Override
                        public void mousePressed(MouseEvent e) {
                            offset.x = e.getX();
                            offset.y = e.getY();
                        }
                    });
                    button.addMouseMotionListener(new MouseMotionAdapter() {
                        @Override
                        public void mouseDragged(MouseEvent e) {
                            int newX = button.getX() + e.getX() - offset.x;
                            int newY = button.getY() + e.getY() - offset.y;
                            button.setLocation(newX, newY);
                        }
                    });
                    gridOverlayPanel.add(button);

                    addedFilePaths.add(absolutePath);
                    pathToButtonMap.put(absolutePath, button);

                }
            }

            Iterator<String> iterator = addedFilePaths.iterator();
            while (iterator.hasNext()) {
                String path = iterator.next();
                File file = new File(path);

                if (!file.exists()) {
                    JButton button = pathToButtonMap.get(path);
                    Container parent = button.getParent();
                    if (parent != null) {
                        parent.remove(button);
                        parent.revalidate();
                        parent.repaint();
                    }
                    addedFilePaths.remove(path);
                    iterator.remove();
                }
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

    private final int gridRows = Integer.parseInt(settings.get("desktopGridRows", String.valueOf(16)));
    private final int gridCols = Integer.parseInt(settings.get("desktopGridCols", String.valueOf(16)));;
    private JPanel[][] gridCells;
    private JPanel gridOverlayPanel;

    private void initGridOverlay() {
        gridOverlayPanel = new JPanel(null);
        gridOverlayPanel.setOpaque(false);
        gridOverlayPanel.setBounds(0, 0, getWidth(), getHeight());
        getLayeredPane().add(gridOverlayPanel, JLayeredPane.PALETTE_LAYER);

        gridCells = new JPanel[gridRows][gridCols];

        for (int i = 0; i < gridRows; i++) {
            for (int j = 0; j < gridCols; j++) {
                JPanel cell = new JPanel(new BorderLayout());
                cell.setOpaque(false);
                cell.setBorder(null);
                gridCells[i][j] = cell;
                gridOverlayPanel.add(cell);
            }
        }
    }

    public boolean addButtonToRandomFreeCell(JButton button, boolean doRemove) {
        List<Point> freeCells = new ArrayList<>();
        for (int i = 0; i < gridRows; i++) {
            for (int j = 0; j < gridCols; j++) {
                if (gridCells[i][j].getComponentCount() == 0) {
                    freeCells.add(new Point(i, j));
                }
            }
        }
        if (freeCells.isEmpty()) {
            return false;
        }
        Point chosen = freeCells.get(rand.nextInt(freeCells.size()));
        if (doRemove) {
            gridCells[chosen.x][chosen.y].remove(button);
        } else {
            gridCells[chosen.x][chosen.y].add(button, BorderLayout.CENTER);
        }
        gridCells[chosen.x][chosen.y].revalidate();
        gridCells[chosen.x][chosen.y].repaint();
        return true;
    }
}
