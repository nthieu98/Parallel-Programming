import matplotlib as mpl
from matplotlib import pyplot
plt = pyplot
import numpy as np
mpl.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as manimation

numIteration = 600
epsilon = 1e-3
eps = 1e-10
omega = "1.90"
eta = "1.00"
N = 128
FPS = 20

C = np.zeros((N, N))
B = np.zeros((N, N))

fig = plt.figure()

file_name = "./log/log_N=" + str(N) + "_eta=" + eta + "_iter=" + str(numIteration) + ".log"
file = open(file_name, "r")
for l in range(N):
    line = file.readline()
    line = line.split(' ')
    for m in range(N):
        C[l, m] = float(line[m])

file.readline()

for l in range(N):
    line = file.readline()
    line = line.split(' ')
    # print(line[126])
    for m in range(N):
        # print(":"+line[m]+":", l, ' ', m)
        B[l, m] = float(line[m])
        if B[l, m] == 1:
            C[l, m] = -1

img = pyplot.imshow(C, interpolation='nearest', cmap='coolwarm', vmin=-1, vmax=1)
# img2 = pyplot.imshow(B, cmap='Greys')
pyplot.savefig("./image/log_N=" + str(N) + "_eta=" + eta + "_iter=" + str(numIteration) + ".jpg")
print("./image/log_N=" + str(N) + "_eta=" + eta + "_iter=" + str(numIteration) + ".jpg")
# writer.grab_frame()
			# pyplot.show()