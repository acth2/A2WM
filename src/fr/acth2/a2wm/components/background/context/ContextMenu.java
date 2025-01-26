package fr.acth2.a2wm.components.background.context;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class ContextMenu extends JFrame{
    private JPanel panel1;
    private JButton settingsButton;
    private JButton terminalButton;

    public ContextMenu() {
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

    public void showContext(int posX, int posY){
        ContextMenu cm = new ContextMenu();
        cm.setContentPane(panel1);
        cm.setSize(255, 355);
        cm.setAlwaysOnTop(true);
        cm.setLocation(posX, posY);
        cm.setUndecorated(true);
        cm.setVisible(true);
    }

    private void createUIComponents() {

    }
}
