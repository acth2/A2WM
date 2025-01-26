package fr.acth2.a2wm.components.background.context;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class ContextMenu extends JFrame {
    private JPanel panel1;
    private JButton settingsButton;
    private JButton terminalButton;

    private void createUIComponents() {
        settingsButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }
        });

        terminalButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        });
    }
}
