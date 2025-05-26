package fr.acth2.a2wm.utils.x11;

public class MinimizedWindow {
    public String windowId;
    public String title;
    public String iconPath;

    public MinimizedWindow(String windowId, String title) {
        this.windowId = windowId;
        this.title = title;
        this.iconPath = null;
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

    @Override
    public String toString() {
        return "MinimizedWindow{ID=" + windowId + ", title=\"" + title + "\"}";
    }
}