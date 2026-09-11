"""
QR Scanner — ESP32 ArduCAM + Python + Symfony + Arduino R4 Barrière
Mode : Veille active — scan QR déclenché par bouton poussoir
"""

import requests
import time
import cv2
import numpy as np
from PIL import Image
from pyzbar import pyzbar
from io import BytesIO
from datetime import datetime

# ─── CONFIGURATION ───────────────────────────────────────────
ESP32_URL       = "http://192.168.4.1/capture"
BUTTON_URL      = "http://192.168.4.1/button-status"
SYMFONY_URL     = "http://127.0.0.1:8000/qr-check"
ARDUINO_URL     = "http://192.168.4.2/open"
SCAN_INTERVAL   = 0.5
SEND_TO_SYMFONY = True
SEND_TO_ARDUINO = True
SHOW_PREVIEW    = True
COOLDOWN_SEND   = 3.0
QR_SCAN_TIMEOUT = 20.0   # secondes avant retour en veille si aucun QR scanné
BUTTON_POLL     = 0.5    # secondes entre chaque vérification bouton
# ─────────────────────────────────────────────────────────────

last_qr_data  = ""
last_sent_at  = 0


def fetch_frame():
    try:
        response = requests.get(ESP32_URL, timeout=5)
        if response.status_code == 200:
            return Image.open(BytesIO(response.content))
        return None
    except:
        print("[ESP32] Connexion impossible")
        return None


def check_button():
    """Interroge l'ESP32 pour savoir si le bouton est appuyé."""
    try:
        response = requests.get(BUTTON_URL, timeout=2)
        if response.status_code == 200:
            data = response.json()
            return data.get("pressed", False)
        return False
    except:
        return False


def decode_qr(image):
    gray  = image.convert("L")
    codes = pyzbar.decode(gray)
    return [c.data.decode("utf-8") for c in codes if c.type == "QRCODE"]


def send_to_symfony(qr_data):
    try:
        response = requests.post(
            SYMFONY_URL,
            json={"qr_code": qr_data},
            headers={"Content-Type": "application/json"},
            timeout=3
        )
        return response.json()
    except Exception as e:
        print(f"[Symfony] Erreur : {e}")
        return {"status": "error"}


def open_barrier():
    """Envoie POST /open à l'Arduino R4 pour ouvrir la barrière entrée."""
    try:
        response = requests.post(ARDUINO_URL, timeout=3)
        result   = response.json()
        print(f"[Arduino] ✅ Barrière ouverte ({result.get('duration_ms', '?')}ms)")
        return True
    except requests.exceptions.ConnectionError:
        print("[Arduino] ❌ Connexion impossible — Arduino allumé et connecté à ParkingCam ?")
        return False
    except Exception as e:
        print(f"[Arduino] ❌ Erreur : {e}")
        return False


def draw_preview(image, qr_results, last_response, mode="veille"):
    frame = cv2.cvtColor(np.array(image), cv2.COLOR_RGB2BGR)
    gray  = np.array(image.convert("L"))
    codes = pyzbar.decode(gray)

    # Bandeau mode en haut
    color_mode = (0, 200, 255) if mode == "veille" else (0, 255, 100)
    label_mode = "EN ATTENTE — Appuyez sur le bouton" if mode == "veille" else "SCAN QR ACTIF"
    cv2.rectangle(frame, (0, 0), (frame.shape[1], 30), (20, 20, 20), -1)
    cv2.putText(frame, label_mode, (10, 20),
                cv2.FONT_HERSHEY_SIMPLEX, 0.55, color_mode, 1)

    for code in codes:
        if code.type != "QRCODE":
            continue

        status = last_response.get("status", "")
        color  = (0, 255, 100) if status == "found" else \
                 (0, 0, 255)   if status == "not_found" else (200, 200, 0)

        pts = np.array([[p.x, p.y] for p in code.polygon], dtype=np.int32)
        cv2.polylines(frame, [pts], True, color, 2)

        x, y = code.rect.left, code.rect.top
        cv2.putText(frame, code.data.decode("utf-8"),
                    (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.55, color, 2)

        label = "ACCES AUTORISE — BARRIERE OUVERTE" if status == "found" else \
                "ACCES REFUSE" if status == "not_found" else "Verification..."
        cv2.putText(frame, label,
                    (x, y - 30), cv2.FONT_HERSHEY_SIMPLEX, 0.55, color, 2)

        if status == "found":
            user = last_response.get("user", {})
            name = f"{user.get('prenom', '')} {user.get('nom', '')}"
            cv2.putText(frame, name,
                        (x, y - 50), cv2.FONT_HERSHEY_SIMPLEX, 0.55, color, 1)

    ts = datetime.now().strftime("%H:%M:%S")
    cv2.putText(frame, ts, (10, frame.shape[0] - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (100, 100, 100), 1)

    return frame


def mode_veille():
    """
    Mode veille : attend l'appui du bouton poussoir.
    Affiche un preview de la caméra en arrière-plan.
    Retourne True quand le bouton est pressé.
    """
    print("[Veille] En attente du bouton poussoir...")

    while True:
        # Vérification bouton
        if check_button():
            print("[Bouton] Appui détecté → activation scan QR")
            return True

        # Preview caméra en veille
        if SHOW_PREVIEW:
            image = fetch_frame()
            if image:
                frame = draw_preview(image, [], {}, mode="veille")
                cv2.imshow("QR Scanner — Davinci Custom Garage", frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    return False

        time.sleep(BUTTON_POLL)


def mode_scan():
    """
    Mode scan actif : lit les QR codes pendant QR_SCAN_TIMEOUT secondes.
    Retourne True si QR valide scanné, False si timeout.
    """
    global last_qr_data, last_sent_at

    print(f"[Scan] Mode scan actif — {QR_SCAN_TIMEOUT}s pour scanner")
    scan_start    = time.time()
    last_response = {}

    while time.time() - scan_start < QR_SCAN_TIMEOUT:
        loop_start = time.time()

        image = fetch_frame()
        if image is None:
            time.sleep(1)
            continue

        qr_results = decode_qr(image)

        if qr_results:
            for qr_data in qr_results:
                now       = time.time()
                is_new    = qr_data != last_qr_data
                is_cooled = (now - last_sent_at) >= COOLDOWN_SEND

                if is_new or is_cooled:
                    last_qr_data = qr_data
                    last_sent_at = now
                    print(f"[QR] Détecté : {qr_data}")

                    if SEND_TO_SYMFONY:
                        last_response = send_to_symfony(qr_data)

                        if last_response.get("status") == "found":
                            user = last_response.get("user", {})
                            print(f"\n  ✅ ACCES AUTORISE")
                            print(f"  ├─ Nom    : {user.get('prenom')} {user.get('nom')}")
                            print(f"  ├─ Email  : {user.get('email')}")
                            print(f"  └─ QR     : {qr_data}\n")

                            if SEND_TO_ARDUINO:
                                open_barrier()

                            # QR valide → retour veille après passage
                            time.sleep(2)
                            return True

                        elif last_response.get("status") == "not_found":
                            print(f"\n  ❌ ACCES REFUSE — QR inconnu : {qr_data}\n")

        # Affichage temps restant dans le terminal
        remaining = QR_SCAN_TIMEOUT - (time.time() - scan_start)
        print(f"[Scan] En attente QR... {remaining:.1f}s restantes", end="\r")

        if SHOW_PREVIEW:
            frame = draw_preview(image, qr_results, last_response, mode="scan")
            cv2.imshow("QR Scanner — Davinci Custom Garage", frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                return False

        elapsed = time.time() - loop_start
        time.sleep(max(0, SCAN_INTERVAL - elapsed))

    print("\n[Scan] Timeout — retour en veille")
    return False


def main():
    print("=" * 60)
    print("  QR Scanner — ArduCAM + Python + Symfony + Arduino R4")
    print("  Mode : Veille + Bouton poussoir")
    print("=" * 60)
    print(f"  ESP32   : {ESP32_URL}")
    print(f"  Bouton  : {BUTTON_URL}")
    print(f"  Symfony : {SYMFONY_URL}")
    print(f"  Arduino : {ARDUINO_URL}")
    print(f"  Timeout : {QR_SCAN_TIMEOUT}s")
    print("  Appuie sur Q pour quitter")
    print("=" * 60 + "\n")

    while True:
        # ── Mode veille ──
        continuer = mode_veille()
        if not continuer:
            print("\n[Scan] Arrêt.")
            break

        # ── Mode scan actif ──
        continuer = mode_scan()
        if not continuer:
            print("\n[Scan] Arrêt.")
            break

    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
