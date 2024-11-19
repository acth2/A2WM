import threading
import time
import pynput.keyboard
from pynput.keyboard import Controller

# Initialisation du contrôleur clavier
keyboard = Controller()


def hold_key(key):
    """Appuie sur une touche de manière prolongée."""
    while True:
        keyboard.press(key)  # Maintient la touche enfoncée
        time.sleep(0.1)


def main():
    # Choisir une touche à maintenir (par exemple 'a')
    key_to_hold = pynput.keyboard.Key.pause
    print(f"La touche '{key_to_hold}' est maintenant maintenue enfoncée.")
    print("Appuyez sur Ctrl+C pour arrêter le script.")

    try:
        # Garde le script actif sans consommer trop de ressources
        while True:
            keyboard.press(key_to_hold)  # Maintient la touche enfoncée
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("\nArrêt du script.")


if __name__ == "__main__":
    main()
