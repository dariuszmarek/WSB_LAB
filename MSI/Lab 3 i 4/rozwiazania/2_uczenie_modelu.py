import cv2
import pickle
import os.path
import numpy as np
from imutils import paths
from sklearn.preprocessing import LabelBinarizer
from sklearn.model_selection import train_test_split
from keras.models import Sequential
from keras.layers.convolutional import Conv2D, MaxPooling2D
from keras.layers.core import Flatten, Dense
from resize_image import resize_image
from keras.utils import np_utils 


LETTER_IMAGES_FOLDER = "przetworzone_obrazy"
MODEL_FILENAME = "model_nn.hdf5"
MODEL_LABELS_FILENAME = "model_labels.dat"

IMAGE_SIZE_X = 20
IMAGE_SIZE_Y = 20

data = []
labels = []

for image_file in paths.list_images(LETTER_IMAGES_FOLDER):
    
    # Odczyt pliku za pomoca opencv 
    # TODO:2.1
    image = cv2.imread(image_file)

    # Sprawdzenie czy udalo sie zaladowac plik
    try:
        if image.size == 0 :
            continue
    except:
        continue
    # Przetworzenie obrazu z RBG do odcieni szarosci
    # TODO:2.2
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Skalowanie zdjecia do wielkosci 20x20
    image = resize_image(image, IMAGE_SIZE_X, IMAGE_SIZE_Y)

    # Przetworzenie zdjecia tak aby ostatecznie znajdowalo sie w 3 wymiarach (20,20,1)
    image = np.expand_dims(image, axis=2)

    # Pobranie nazwy pliku ze sciezki 
    label = image_file.split(os.path.sep)[-2]

    # Dodanie obrazu do listy 
    # TODO:2.3
    data.append(image)
    # Dodanie etykiety do listy
    # TODO:2.4
    labels.append(label)


# Normalizacja pikseli
data = np.array(data, dtype="float") / 255.0

# Przekstałcenie etykiet do tablicy z numpy
labels = np.array(labels)

# Podział danych na uczace i treningowe
# TODO:2.5 - https://scikit-learn.org/stable/modules/generated/sklearn.model_selection.train_test_split.html
(X_train, X_test, Y_train, Y_test) = train_test_split(data, labels, test_size=0.25, random_state=0)

# Przeksztalcenie etykiet do listy i zapis modelu etykiet do pliku
lb = LabelBinarizer().fit(Y_train)
Y_train = lb.transform(Y_train)
Y_test = lb.transform(Y_test)

with open(MODEL_LABELS_FILENAME, "wb") as f:
    pickle.dump(lb, f)

# Tworzenie modelu sieci neuronowej
model = Sequential()
model.add(Conv2D(20, (5, 5), padding="same", input_shape=(IMAGE_SIZE_X, IMAGE_SIZE_Y, 1), activation="relu"))
model.add(MaxPooling2D(pool_size=(2, 2), strides=(2, 2)))
model.add(Flatten())
model.add(Dense(500, activation="relu"))
model.add(Dense(32, activation="softmax"))
model.compile(loss="categorical_crossentropy", optimizer="adam", metrics=["accuracy"])

# Trenowanie seci
model.fit(X_train, Y_train, validation_data=(X_test, Y_test), batch_size=32, epochs=5, verbose=1)

# Zapis modelu sieci do pliku
# TODO:2.5
model.save(MODEL_FILENAME)
