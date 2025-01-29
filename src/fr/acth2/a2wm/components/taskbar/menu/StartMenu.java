package fr.acth2.a2wm.components.taskbar.menu;

import fr.acth2.a2wm.components.background.BackgroundWindow;
import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.swing.*;
import java.awt.*;

import static fr.acth2.a2wm.utils.logger.Logger.*;

public class StartMenu extends JFrame {

    private static StartMenu instance = null;
    public SettingsManager settings = new SettingsManager();

    private StartMenu() {
        super("A2WM-STARTMENU");
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setResizable(false);
        setUndecorated(true);
        setAlwaysOnTop(true);

        log("Initializing StartMenu...");
        initializeWindow();
        setVisible(true);
        log("StartMenu is now visible.");
    }

    public static StartMenu getInstance() {
        if (instance == null) {
            instance = new StartMenu();
        }
        return instance;
    }

    private void initializeWindow() {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int width;
        int height;
        try {
            width = Integer.parseInt(settings.get("start-menu-width", "650"));
        } catch (NumberFormatException e) {
            width = 650;
            err("Invalid start-menu-width. Using default: 650");
        }

        try {
            height = Integer.parseInt(settings.get("start-menu-height", "513"));
        } catch (NumberFormatException e) {
            height = 513;
            err("Invalid start-menu-height. Using default: 513");
        }

        log("BackgroundWindow.yAxisReducer: " + BackgroundWindow.yAxisReducer);

        int posX = 0;
        int posY = screenSize.height - BackgroundWindow.yAxisReducer - height;
        log("Calculated StartMenu Position: (" + posX + ", " + posY + ")");

        setSize(width, height);
        setLocation(posX, posY);

        JPanel rootPanel = new JPanel(new BorderLayout());
        rootPanel.setBackground(Color.DARK_GRAY);

        JLabel label = new JLabel("Start Menu Content Here", SwingConstants.CENTER);
        label.setForeground(Color.WHITE);
        rootPanel.add(label, BorderLayout.CENTER);

        setContentPane(rootPanel);
    }

    public void toggleVisibility() {
        setVisible(!isVisible());
        log("StartMenu visibility toggled. Now visible: " + isVisible());
    }

    @Override
    public void dispose() {
        super.dispose();
        instance = null;
        log("StartMenu disposed and instance reset.");
    }
}