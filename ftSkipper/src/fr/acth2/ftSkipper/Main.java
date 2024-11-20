package fr.acth2.ftSkipper;
import oracle.jrockit.jfr.JFR;

import javax.swing.*;
import java.awt.*;

import static fr.acth2.ftSkipper.logger.Logger.*;

public class Main extends Thread {
    public static void main(String[] args) {
        log("FtSkipper: Initialization");
        Main mt = new Main();
        mt.start();
    }

    @Override
    public void run() {
        Robot bot = null;
        try { bot = new Robot();} catch (AWTException e) { throw new RuntimeException(e); }
        while (true) {
            PointerInfo a = MouseInfo.getPointerInfo();
            Point b = a.getLocation();
            bot.mouseMove((int) b.getX(), (int) b.getY());
        }
    }
}
