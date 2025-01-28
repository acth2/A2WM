package fr.acth2.a2wm;

import com.formdev.flatlaf.FlatDarkLaf;
import fr.acth2.a2wm.components.background.BackgroundWindow;
import fr.acth2.a2wm.components.taskbar.TaskbarWindow;
import fr.acth2.a2wm.utils.References;
import javafx.scene.control.TableView;

import javax.swing.*;

import static fr.acth2.a2wm.utils.References.*;
import static fr.acth2.a2wm.utils.logger.Logger.*;

public class Wrapper {
    public static void main(String[] args) {
        if(!isLinux()) {
            err("The window-manager is not compatible with non-unix os.");
            System.exit(0);
        }

        log("STARTING " + NAME);
        log("VERSION  " + VERSION);

        try {
            UIManager.setLookAndFeel(new FlatDarkLaf());
            SwingUtilities.invokeLater(() -> {
                BackgroundWindow backgroundWindow = new BackgroundWindow();
            });

            SwingUtilities.invokeLater(() -> {
                TaskbarWindow taskbarWindow = new TaskbarWindow();
            });
        } catch (Exception exception) { err(NAME + " CRASHED!\n"); exception.printStackTrace(); }
    }
}
