package fr.acth2.a2wm.components.background.context;


import fr.acth2.a2wm.utils.settings.SettingsManager;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;

public class ContextMenu extends JFrame {
    private JPanel panel1;
    private JButton settingsButton;
    private JButton terminalButton;
    private static ContextMenu currentInstance;

    public ContextMenu(int posX, int posY) {
        super("A2WM-CONTEXTMENU");
        SettingsManager settings = SettingsManager.getInstance();

        setContentPane(panel1);
        setSize(255, 355);
        setAlwaysOnTop(true);
        setLocation(posX, posY);
        setUndecorated(true);

        settingsButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                // nothing for now
            }
        });

        terminalButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Runtime.getRuntime().exec(settings.get("terminal", "xterm"));
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
                dispose();
            }
        });

        this.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosed(java.awt.event.WindowEvent windowEvent) {
                if (currentInstance == ContextMenu.this) {
                    currentInstance = null;
                }
            }
        });

        this.addWindowFocusListener(new WindowAdapter() {
            @Override
            public void windowLostFocus(WindowEvent e) {
                dispose();
            }
        });

        Toolkit.getDefaultToolkit().addAWTEventListener(event -> {
            if (event instanceof MouseEvent) {
                MouseEvent mouseEvent = (MouseEvent) event;
                if (mouseEvent.getID() == MouseEvent.MOUSE_PRESSED) {
                    Component clickedComponent = SwingUtilities.getDeepestComponentAt(mouseEvent.getComponent(), mouseEvent.getX(), mouseEvent.getY());
                    if (clickedComponent == null || !SwingUtilities.isDescendingFrom(clickedComponent, this)) {
                        dispose();
                    }
                }
            }
        }, AWTEvent.MOUSE_EVENT_MASK);
    }


    public static void showContext(int posX, int posY) {
        if (currentInstance != null && currentInstance.isShowing()) {
            currentInstance.dispose();
        }

        currentInstance = new ContextMenu(posX, posY);
        currentInstance.setVisible(true);
    }

    public static void hideContext() {
        if (currentInstance != null && currentInstance.isShowing()) {
            currentInstance.dispose();
            currentInstance = null;
        }
    }

    private void createUIComponents() {

    }
}
