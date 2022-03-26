import matplotlib.pyplot as plt
import seaborn as sns

myfile = open("out_ISLIP_bursty2.txt", "rt")
contents = myfile.readlines()
myfile.close()               

ns = []
vals = []

for line in contents:
    arr = line.split('\t')
    ns.append(float(arr[1]))    
    vals.append(float(arr[-2]))

plt.plot(ns, vals)


plt.xlabel("p, Packet generation probability",  fontsize=15)
# plt.ylabel("Average Delay (in timeslots)", fontsize=15)
plt.ylabel("Link Utilisation", fontsize=15)
plt.title("ISLIP, N = 5, B = 15, T = 10000", fontsize=15)

# plt.legend(['Buffer Size = 2','Buffer Size = 3', 'Buffer Size = 4'], fontsize=15)
plt.show()  