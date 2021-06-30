import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
df = pd.read_excel("C:\\Users\\User\\Desktop\\ES\\finalprojects//data.xlsx")
num1 = 0
num2 = 0
num3 = 0
num4 = 0
num5 = 0
num6 = 0

for x in df["Temperature"]:
    if x >= 35.5 and x < 36:
        num1 = num1 + 1
    elif x >= 36 and x < 36.5:
        num2 = num2 + 1
    elif x >= 36.5 and x < 37:
        num3 = num3 + 1
    elif x >= 37 and x < 37.5:
        num4 = num4 + 1
    elif x >= 37.5 and x < 38:
        num5 = num5 + 1
    else:
        num6 = num6 + 1


left = np.array(['35.5~36','36~36.5','36.5~37','37~37.5','37.5~38','38 up'])
height = np.array([num1,num2,num3,num4,num5,num6])
plt.bar(left,height)
plt.xlabel("Temperature")
plt.ylabel("Number Of People")
plt.show()




