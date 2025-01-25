package fr.acth2.a2wm.utils.settings;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.*;

public class SettingsManager {
    private static final String SETTINGS_DIR = System.getProperty("user.home") + "/.a2wm";
    private static final String SETTINGS_FILE = SETTINGS_DIR + "/settings.json";
    private static SettingsManager instance;
    private JsonObject settings;
    private final Gson gson = new Gson();

    private SettingsManager() {
        File dir = new File(SETTINGS_DIR);
        File file = new File(SETTINGS_FILE);

        if (!dir.exists()) {
            boolean dirCreated = dir.mkdirs();
            if (dirCreated) {
                System.out.println("Created settings directory at: " + SETTINGS_DIR);
            } else {
                System.err.println("Failed to create settings directory at: " + SETTINGS_DIR);
            }
        } else {
            System.out.println("Settings directory already exists at: " + SETTINGS_DIR);
        }

        if (!file.exists()) {
            try (FileWriter writer = new FileWriter(file)) {
                settings = new JsonObject();
                settings.addProperty("imagePath", SETTINGS_DIR + "/base.png");
                String jsonString = gson.toJson(settings);
                writer.write(jsonString);
                writer.flush();
                System.out.println("Created default settings.json at: " + SETTINGS_FILE);
                System.out.println("Settings content: " + jsonString);
            } catch (IOException e) {
                System.err.println("Error creating settings.json.");
                e.printStackTrace();
            }
        } else {
            try (FileReader reader = new FileReader(file)) {
                settings = JsonParser.parseReader(reader).getAsJsonObject();
                System.out.println("Loaded settings from: " + SETTINGS_FILE);
                System.out.println("Settings content: " + gson.toJson(settings));
            } catch (IOException | IllegalStateException e) {
                System.err.println("Error reading settings.json. Initializing with empty settings.");
                e.printStackTrace();
                settings = new JsonObject();
            }
        }
    }

    public static synchronized SettingsManager getInstance() {
        if (instance == null) {
            instance = new SettingsManager();
        }
        return instance;
    }

    public String get(String key, String defaultValue) {
        String result = settings.has(key) ? settings.get(key).getAsString() : defaultValue;
        System.out.println("Get key: '" + key + "' with value: '" + result + "'");
        return result;
    }

    public void set(String key, String value) {
        settings.addProperty(key, value);
        System.out.println("Set key: '" + key + "' with value: '" + value + "'");
        saveSettings();
    }

    public void remove(String key) {
        settings.remove(key);
        System.out.println("Removed key: '" + key + "'");
        saveSettings();
    }

    private void saveSettings() {
        try (FileWriter writer = new FileWriter(SETTINGS_FILE)) {
            String jsonString = gson.toJson(settings);
            writer.write(jsonString);
            writer.flush();
            System.out.println("Settings saved to: " + SETTINGS_FILE);
            System.out.println("Settings content: " + jsonString);
        } catch (IOException e) {
            System.err.println("Error saving settings.json.");
            e.printStackTrace();
        }
    }
}
