import numpy as ny
import numpy as np
 
print("########################################################")
print("##################### ZAD1 #############################")
print("########################################################")

A = ny.array([[1, 3, 7]])
B = ny.array([[1, 4, 8]])
print(A)
print(B)
C = A + B
D = A - B
E = A * B
F = A / B
G = A ** B
H = A % B
print("\n Dodwanie \n")
print(C)
print("\n Odejmowanie \n")
print(D)
print("\n Mnożenie \n")
print(E)
print("\n Dzielenie \n")
print(F)
print("\n Potęgowanie \n")
print(G)
print("\n Reszta z dzielenia \n")
print(H)
 


print("########################################################")
print("##################### ZAD2 #############################")
print("########################################################")


A = np.matrix('1 2 3; 3 4 3; 3 3 3')
B = np.matrix('1 2 3; 3 4 3; 3 3 3')
print(A+B, "A+B")
print(A-B, "A-B")
print(A*B, "A*B")
print(A/B, "A/B")
print(A**3, "A**3")
print(A%B, "A%B")


print("########################################################")
print("##################### ZAD3 #############################")
print("########################################################")

 
print("Otwieranie pliku :\n \n")
try:
    f = open("plik.txt", "r")
    print(f.read())
except:
    print("plik nie istnieje \n \n")




print("########################################################")
print("##################### ZAD4 #############################")
print("########################################################")

print("dzielenie przez 0 : \n \n")
def funkcja():
    2/0
try:
    funkcja()
except:
    print("nie dziel przez zero")