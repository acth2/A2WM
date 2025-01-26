package fr.acth2.a2wm;

import fr.acth2.a2wm.backend.background.BackgroundWindow;
import fr.acth2.a2wm.backend.taskbar.TaskbarWindow;

import javax.swing.*;

import static fr.acth2.a2wm.utils.References.*;
import static fr.acth2.a2wm.utils.logger.Logger.*;

public class Main {
    public static void main(String[] args) {
        if(!isLinux()) {
            err("The window-manager is not compatible with non-unix os.");
            System.exit(0);
        }

        SwingUtilities.invokeLater(() -> {
            BackgroundWindow backgroundWindow = new BackgroundWindow();
        });

        SwingUtilities.invokeLater(() -> {
            TaskbarWindow backgroundWindow = new TaskbarWindow();
        });
    }
}
