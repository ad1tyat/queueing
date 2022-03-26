import matplotlib.pyplot as plt
import seaborn as sns

myfile = open("out_KOUQ_1_buf_2_k_1.txt", "rt")
contents = myfile.readlines()
myfile.close()               


myfile3 = open("out_KOUQ_1_buf_3_k_1.txt", "rt")
contents3 = myfile3.readlines()
myfile3.close()               


myfile4 = open("out_KOUQ_1_buf_4_k_1.txt", "rt")
contents4 = myfile4.readlines()
myfile4.close()               

ns = []
vals = []
vals3 = []
vals4 = []
for line in contents:
    arr = line.split('\t')
    ns.append(int(arr[0]))    
    vals.append(float(arr[3]))
for line in contents3:
    arr = line.split('\t')
    vals3.append(float(arr[3]))
for line in contents4:
    arr = line.split('\t')    
    vals4.append(float(arr[3]))

# print(ns)
# print(vals)

plt.plot(ns, vals)
plt.plot(ns, vals3)
plt.plot(ns, vals4)
# fig, ax = plt.subplots()
# sns.kdeplot(vals,ax = ax )
# sns.kdeplot(vals2,color ="red",ax = ax)


plt.xlabel("N, Number of Ports",  fontsize=15)
plt.ylabel("Average Delay (in timeslots)", fontsize=15)
plt.title("KOUQ, K=N, p = 1, T = 10000", fontsize=15)

plt.legend(['Buffer Size = 2','Buffer Size = 3', 'Buffer Size = 4'], fontsize=15)
plt.show()  