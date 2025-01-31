package fr.acth2.a2wm.utils.x11;

public class MinimizedWindow {
    public String windowId;
    public String title;

    public MinimizedWindow(String windowId, String title) {
        this.windowId = windowId;
        this.title = title;
    }

    public String getTitle() {
        return title;
    }

    public String getWindowId() {
        return windowId;
    }

    @Override
    public String toString() {
        return "MinimizedWindow{ID=" + windowId + ", title=\"" + title + "\"}";
    }
}