from __future__ import print_function


import os
os.environ["KERAS_BACKEND"] = "plaidml.keras.backend"


import keras
from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
from keras import backend as K
from keras.models import model_from_json
from keras.models import load_model

from keras.datasets import mnist 
from keras.utils import np_utils 
from keras import layers 
from keras import models 
from keras.utils import to_categorical

from matplotlib.image import imread
import numpy as np

(X_train, y_train), (X_test, y_test) = mnist.load_data()

train_images = X_train.reshape((60000, 28, 28, 1)) 
train_images = train_images.astype('float32') / 255
test_images = X_test.reshape((10000, 28, 28, 1)) 
test_images = test_images.astype('float32') / 255

train_labels = to_categorical(y_train) 
test_labels = to_categorical(y_test)

model = models.Sequential() 
model.add(layers.Conv2D(32, (3, 3), activation='relu', input_shape=(28, 28, 1))) 
model.add(layers.MaxPooling2D((2, 2)))

model.add(layers.Conv2D(64, (3, 3), activation='relu', input_shape=(28, 28, 1))) 
model.add(layers.MaxPooling2D((2, 2)))

model.add(layers.Conv2D(128, (3, 3), activation='relu'))
model.add(layers.Flatten())
model.add(layers.Dense(10, activation='softmax'))
model.compile(optimizer='rmsprop', loss='categorical_crossentropy', metrics=['accuracy'])
model.fit(train_images, train_labels, epochs=1, batch_size=64)
test_loss, test_acc = model.evaluate(test_images, test_labels) 
print(test_loss, test_acc)

#zapis modelu sieci do pliku 

# serialize model to JSON
model_json = model.to_json()
with open("model.json", "w") as json_file:
    json_file.write(model_json)
# serialize weights to HDF5
model.save_weights("model.h5")
print("Saved model to disk")