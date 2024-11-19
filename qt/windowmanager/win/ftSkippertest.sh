from pynput.keyboard import Controller, Key

keyboard = Controller()


def main():
    key_to_hold = Key.pause

    keyboard.press(key_to_hold)
    print(f"La touche '{key_to_hold}' est maintenant maintenue enfoncee.")
    print("Appuyez sur Ctrl+C pour arrêter le script.")

    try:
        while True:
            pass
    except KeyboardInterrupt:
        keyboard.release(key_to_hold)
        print("\nTouche relachee. Arrêt du script.")


if __name__ == "__main__":
    main()
