package fr.acth2.a2wm.utils.settings;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

import java.io.*;

public class SettingsManager {
    private static final String SETTINGS_DIR = "/usr/cydra/settings";
    private static final String SETTINGS_FILE = SETTINGS_DIR + "/settings.json";
    private static SettingsManager instance;
    private JSONObject settings;

    private SettingsManager() {
        File dir = new File(SETTINGS_DIR);
        File file = new File(SETTINGS_FILE);

        if (!dir.exists()) {
            boolean dirCreated = dir.mkdirs();
            if (!dirCreated) {
                System.err.println("Failed to create settings directory.");
            }
        }

        if (!file.exists()) {
            try (FileWriter writer = new FileWriter(file)) {
                settings = new JSONObject();
                settings.put("imagePath", "/usr/cydra/settings/default.png");
                writer.write(settings.toString());
                writer.flush();
                System.out.println("Created default settings.json.");
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            JSONParser parser = new JSONParser();
            try (FileReader reader = new FileReader(file)) {
                Object obj = parser.parse(reader);
                settings = (JSONObject) obj;
            } catch (Exception e) {
                e.printStackTrace();
                settings = new JSONObject();
            }
        }
    }

    public static SettingsManager getInstance() {
        if (instance == null) {
            instance = new SettingsManager();
        }
        return instance;
    }

    public String get(String key, String defaultValue) {
        Object value = settings.get(key);
        return value != null ? value.toString() : defaultValue;
    }

    public void set(String key, String value) {
        settings.put(key, value);
        saveSettings();
    }

    public void remove(String key) {
        settings.remove(key);
        saveSettings();
    }

    private void saveSettings() {
        try (FileWriter writer = new FileWriter(SETTINGS_FILE)) {
            writer.write(settings.toString());
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}