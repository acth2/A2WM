package fr.acth2.a2wm.utils.process;

import java.io.IOException;

public class ProcessManager {
    public static void startApplication(String application) {
        new Thread() {
            @Override
            public void run() {
                setDaemon(true);
                try {
                    Runtime.getRuntime().exec(application);
                } catch (IOException exp) {
                    exp.printStackTrace();
                }

                super.run();
            }
        }.start();
    }
}
