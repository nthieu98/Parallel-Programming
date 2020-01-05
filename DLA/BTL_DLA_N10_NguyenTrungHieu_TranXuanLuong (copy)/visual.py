import matplotlib as mpl
from matplotlib import pyplot
plt = pyplot
import numpy as np
mpl.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as manimation

# numIteration = 1000
# epsilon = 1e-3
# eps = 1e-10
# omega = "1.90"
# eta = "2.00"
# N = 128
FPS = 10

f = open("config.txt", "r")
# print(f.readline())
N = int(f.readline())
# print(N)
numIteration = int(f.readline())
# print(numIteration)
eta = float(f.readline())
eta = "{0:.2f}".format(eta)
# print(eta)
omega = float(f.readline())
omega = "{0:.2f}".format(omega)

f.close()
# print(omega)

C = np.zeros((N, N))
B = np.zeros((N, N))

FFMpegWriter = manimation.writers['ffmpeg']
metadata = dict(title='DLA', artist='HieuNT')
writer = FFMpegWriter(fps=5, metadata=metadata)

fig = plt.figure()
with writer.saving(fig, "./video/video_N=" + str(N) + "_eta=" + str(eta) + "_iter" + str(numIteration) + ".mp4", 100):
	for i in range(numIteration + 1):
		if i % FPS == 0:
			file_name = "./log/log_N=" + str(N) + "_eta=" + str(eta) + "_iter=" + str(i) + ".log"
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
			pyplot.savefig("./image/log_N=" + str(N) + "_eta=" + str(eta) + "_iter=" + str(i) + ".jpg")
			print("./image/log_N=" + str(N) + "_eta=" + str(eta) + "_iter=" + str(i) + ".jpg")
			writer.grab_frame()
			# pyplot.show()