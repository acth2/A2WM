package fr.acth2.a2wm.utils.settings;

import fr.acth2.a2wm.Wrapper;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import static fr.acth2.a2wm.utils.logger.Logger.*;

import java.io.*;
import java.util.Objects;

public class SettingsManager {
    private static final String SETTINGS_DIR = System.getProperty("user.home") + "/.a2wm";
    private static final String SETTINGS_FILE = SETTINGS_DIR + "/settings.json";
    private static SettingsManager instance;
    private JSONObject settings;

    public SettingsManager() {
        File dir = new File(SETTINGS_DIR);
        File file = new File(SETTINGS_FILE);

        if (!dir.exists()) {
            boolean dirCreated = dir.mkdirs();
            if (dirCreated) {
                log("Created settings directory at: " + SETTINGS_DIR);
            } else {
                err("Failed to create settings directory at: " + SETTINGS_DIR);
            }
        } else {
            log("Settings directory already exists at: " + SETTINGS_DIR);
        }

        if (!file.exists()) {
            try {
                if (file.createNewFile()) {
                    log("Settings file created successfully");
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        } else {
            JSONParser parser = new JSONParser();
            try (FileReader reader = new FileReader(file)) {
                Object obj = parser.parse(reader);
                if (obj instanceof JSONObject) {
                    settings = (JSONObject) obj;
                    log("Loaded settings from: " + SETTINGS_FILE);
                    log("Settings content: " + settings.toJSONString());
                } else {
                    err("settings.json does not contain a valid JSON object. Initializing with empty settings.");
                    settings = new JSONObject();
                }
            } catch (IOException | ParseException e) {
                err("Error reading settings.json. Initializing with empty settings.");
                if (Wrapper.atomicDebug.get()) {
                    e.printStackTrace();
                }
                settings = new JSONObject();
            }
        }
    }

    public static synchronized SettingsManager getInstance() {
        if (instance == null) {
            instance = new SettingsManager();
        }
        return instance;
    }

    public boolean isDarkmode() {
        return Objects.equals(this.get("darkmode", "false"), "true");
    }

    public String get(String key, String defaultValue) {
        Object value = settings.get(key);
        String result = value != null ? value.toString() : defaultValue;
        log("Get key: '" + key + "' with value: '" + result + "'");
        return result;
    }

    public void set(String key, String value) {
        settings.put(key, value);
        log("Set key: '" + key + "' with value: '" + value + "'");
        saveSettings();
    }

    public void remove(String key) {
        settings.remove(key);
        log("Removed key: '" + key + "'");
        saveSettings();
    }

    private void saveSettings() {
        try (FileWriter writer = new FileWriter(SETTINGS_FILE)) {
            String jsonString = settings.toJSONString();
            writer.write(jsonString);
            writer.flush();
            log("Settings saved to: " + SETTINGS_FILE);
            log("Settings content: " + jsonString);
        } catch (IOException e) {
            err("Error saving settings.json.");
            e.printStackTrace();
        }
    }
}
