import matplotlib.pyplot as plt


f = open("ViberStringC1.txt", "r")

y = f.readline()
plt.plot(y)
# plt.show()

print(y)