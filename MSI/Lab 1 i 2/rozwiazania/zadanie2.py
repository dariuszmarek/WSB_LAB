
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

#odczyt modelu sieci z pliku
 
# load json and create model
json_file = open('model.json', 'r')
loaded_model_json = json_file.read()
json_file.close()
loaded_model = model_from_json(loaded_model_json)
# load weights into new model
loaded_model.load_weights("model.h5")
print("Loaded model from disk")

img = imread('zdjecie_do_zadania1.png')

def rgb2gray(rgb):
    return np.dot(rgb[...,:3], [0.2989, 0.5870, 0.1140])

gray = rgb2gray(img) 
gray = gray.reshape((1, 28, 28, 1)) 
ynew = loaded_model.predict_classes(gray)
# show the inputs and predicted outputs
print(ynew)