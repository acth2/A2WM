package fr.acth2.a2wm.utils.x11;

import fr.acth2.a2wm.utils.manager.ImageManager;

import javax.swing.*;

import java.io.File;

import static fr.acth2.a2wm.utils.References.mainDir;

public class MinimizedWindow {
    public String windowId;
    public String title;
    public String iconPath;
    private ImageIcon icon;

    public MinimizedWindow(String windowId, String title) {
        this.windowId = windowId;
        this.title = title;

        // to modify later...
        this.icon = null;

        if (this.icon == null || this.icon.getImage() == null) {
            File defaultIconFile = new File(mainDir.getPath() + "/icon.xpm");
            if (!defaultIconFile.exists()) {
                defaultIconFile = new File(mainDir.getPath() + "/icon.png");
            }
            this.icon = ImageManager.loadImage(defaultIconFile.getPath(), 24, 24);
        }
    }


    public MinimizedWindow(String windowId, String title, String iconPath) {
        this.windowId = windowId;
        this.title = title;
        this.iconPath = iconPath;
    }

    public String getTitle() {
        return title;
    }

    public String getWindowId() {
        return windowId;
    }

    public String getIconPath() {
        return iconPath;
    }

    public ImageIcon getIcon() {
        return icon;
    }

    @Override
    public String toString() {
        return "MinimizedWindow{ID=" + windowId + ", title=\"" + title + "\"}";
    }
}