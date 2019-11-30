import os
import os.path
import cv2
import glob
import imutils


IMAGES_FOLDER = "obrazy"
OUTPUT_FOLDER = "przetworzone_obrazy"

PIXEL_MARGIN = 3

# przygotowanie listy plików w określonym folderze
image_files = glob.glob(os.path.join(IMAGES_FOLDER, "*"))

# licznik okreslonych znakow
counts = {}

files_failed = 0

for (i, image_file) in enumerate(image_files):
    print("Przetwarzanie obrazu {}/{}".format(i + 1, len(image_files)))

    # Zakladamy, ze nazywy plikow okreslaja znaki jakie sa na obrazie 
    # Pobranie nazwy pliku 
    filename = os.path.basename(image_file)
    text = os.path.splitext(filename)[0]

    # Odczyt pliku za pomoca opencv 
    # TODO:1.1

    # Przetworzenie obrazu z RBG do odcieni szarosci
    # TODO:1.2

    # Progowanie zdjęcia tak aby było czarno biale a nie w odcieniach szarosci
    thresh = cv2.threshold(image, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]

    # Wykrywanie konturow na zdjeciu 
    contours = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours = contours[0] 

    letter_image_regions = []

    # Przetwarzanie konturow na regiony ze znakami zdjec
    for contour in contours:
        # Pobranie danych o wielkosci i polozeniu  konturu
        (x, y, w, h) = cv2.boundingRect(contour)
        letter_image_regions.append((x, y, w, h))

    # Zakladamy, ze nasze obrazy posiadaja taka sama liczbe znakow.
    # Sprawdzenie czy udalo sie wykryc 4 kontury
    if len(letter_image_regions) != 4:
        continue

    # Sortowanie wykrytych znakow tak aby isc od lewej do prawej
    letter_image_regions = sorted(letter_image_regions, key=lambda x: x[0])

    # Zapisywanie poszczegolnych znakow z obrazu do osobnych plikow
    for letter_bounding_box, letter_text in zip(letter_image_regions, text):
        
        # Pobranie danych o wielkosci i polozeniu znaku
        x, y, w, h = letter_bounding_box

        # Wyciecie znaku z obrazu wraz z dodatkowym marginesem 
        letter_image = image[y - PIXEL_MARGIN:y + h + PIXEL_MARGIN, x - PIXEL_MARGIN:x + w + PIXEL_MARGIN]

        # Stworzenie sciezki do zapisu 
        save_path = os.path.join(OUTPUT_FOLDER, letter_text)

        # Stworzenie folderu jeżeli nie istatnie
        if not os.path.exists(save_path):
            os.makedirs(save_path)

        # Stworzenie nazwy pliku (sciezki) do zapisu 
        count = counts.get(letter_text, 1)
        p = os.path.join(save_path, "{}.png".format(str(count).zfill(6)))
        
        
        # Zapis znaku pliku
        # Czasami obrazy sa zle wycinane, try/except zabezpiecza przed blednym zapisaem pliku
        try:
            # TODO:1.3
        except:
            files_failed = files_failed + 1
            pass
        # inkrementacja licznika okreslonego typu znakow
        counts[letter_text] = count + 1

print("{0} files failed".format(files_failed))
