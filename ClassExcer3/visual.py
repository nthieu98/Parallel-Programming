import matplotlib.pyplot as plt
import numpy as np

f = open("ViberStringC1.txt", "r")
x = np.arange(0, 6, 0.1)
print(x)
line = f.readline()
y = []
while line:
  num = float(line[:-2])
  y.append(num)
  line = f.readline()
print(y)
plt.plot(x, y[:60])
plt.show()

