package fr.acth2.a2wm.components.background.context;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class ContextMenu extends JFrame {
    private JPanel panel1;
    private JButton settingsButton;
    private JButton terminalButton;
    private static ContextMenu currentInstance;

    public ContextMenu(int posX, int posY) {
        setContentPane(panel1);
        setSize(255, 355);
        setAlwaysOnTop(true);
        setLocation(posX, posY);
        setUndecorated(true);

        settingsButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }
        });

        terminalButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
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
