#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec 10 09:57:20 2020
Reading data_5 file
@author: lucascalam
"""
import numpy as np
import csv
import matplotlib.pyplot as plt

micro_in_seconds = 1000000


#%%

# reader = csv.reader(open("data_5/client_end.csv"), delimiter=",")
reader = csv.reader(open("client_end.csv"), delimiter=",")
aux = []
aux_idx = []
i=0
i=0
flag = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        aux.append(int(row[3]))
        aux_idx.append(int(row[2]) + (int(row[1][-5:]))*micro_in_seconds)  
        i = i + 1
client_end = np.asarray(aux)
index_cli_end = np.asarray(aux_idx)
index_cli_end -= index_cli_end[0]
print(len(client_end))
print(len(index_cli_end))
#%%


# reader = csv.reader(open("data_5/client_1.csv"), delimiter=",")
reader = csv.reader(open("client_1.csv"), delimiter=",")
aux = []
aux_idx = []
i=0
flag = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        # print(row)        
        aux.append(int(row[3]))
        aux_idx.append(int(row[2]) + (int(row[1][-5:]))*micro_in_seconds)  
        i = i + 1
client_1 = np.asarray(aux)
index_cli_1 = np.asarray(aux_idx)
index_cli_1 -= index_cli_1[0]
print(len(client_1))
print(len(index_cli_1))
#%%


# reader = csv.reader(open("data_5/client_i.csv"), delimiter=",")
reader = csv.reader(open("client_i.csv"), delimiter=",")
aux = []
aux_idx = []
i=0
flag = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        # print(row)        
        aux.append(int(row[3]))
        aux_idx.append(int(row[2]) + (int(row[1][-5:]))*micro_in_seconds)  
        i = i + 1
client_i = np.asarray(aux)
index_cli_i = np.asarray(aux_idx)
index_cli_i -= index_cli_i[0]
print(len(client_i))
print(len(index_cli_i))

#%%


# reader = csv.reader(open("data_5/client_ii.csv"), delimiter=",")
reader = csv.reader(open("client_ii.csv"), delimiter=",")
aux = []
aux_idx = []
i=0
flag = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        # print(row)        
        aux.append(int(row[3]))
        aux_idx.append(int(row[2]) + (int(row[1][-5:]))*micro_in_seconds)  
        i = i + 1
client_ii = np.asarray(aux)
index_cli_ii = np.asarray(aux_idx)
index_cli_ii -= index_cli_ii[0]
print(len(client_ii))
print(len(index_cli_ii))

#%%


# reader = csv.reader(open("data_5/server.csv"), delimiter=",")
reader = csv.reader(open("server.csv"), delimiter=",")
aux = []
aux_idx = []
i=0
flag = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        # print(row)        
        aux.append(int(row[3]))
        aux_idx.append(int(row[2]) + (int(row[1][-5:]))*micro_in_seconds)  
        i = i + 1
server = np.asarray(aux)
index_server = np.asarray(aux_idx)
index_server -= index_server[0]
print(len(server))
print(len(index_server))

#%%

# server_tmp = np.linspace(server[0],server[len(client_1)-1],len(client_1))
# index = np.arange(0,len(client_1))

limit = 9000

dif_1_1 = client_1[:limit] - server[:limit]
index_1_1 = index_cli_1[:limit]*1e-6

dif_end_1 = client_end[:limit] - server[:limit]
index_end_1 = index_cli_end[:limit]*1e-6

dif_i_1 = client_i[:limit] - server[:limit]
index_i_1 = index_cli_i[:limit]*1e-6

dif_ii_1 = client_ii[:limit] - server[:limit]
index_ii_1 = index_cli_ii[:limit]*1e-6


#%%

fig = plt.figure(figsize=(12,6))
ax = fig.add_subplot(1, 1, 1)

ax.scatter(index_1_1,dif_1_1,color='#17becf',s=70)
ax.scatter(index_end_1,dif_end_1,color='#7f7f7f',s=40)
ax.scatter(index_i_1,dif_i_1,s=20)
ax.scatter(index_ii_1,dif_ii_1,s=10)
list_legend = ['Client 1', 'Client i', 'Client ii','Client end']
ax.grid()
plt.title('Difference of simulated clocks with respect to the time source(first node)',fontsize=18)
ax.legend(list_legend,fontsize=14,shadow=True)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
plt.xlabel("Real time from laptop clock [s]",fontsize=18);
plt.ylabel('Difference time [us]',fontsize=18);
plt.savefig('dif_time.pdf')
plt.show()


#%%

limit_acc_inf = 3500
limit_acc = 3520

client_1_2 = client_1[limit_acc_inf:limit_acc]*1e-6
index_1_2 = index_cli_1[limit_acc_inf:limit_acc]*1e-6

client_end_2 = client_end[limit_acc_inf:limit_acc]*1e-6
index_end_2 = index_cli_end[limit_acc_inf:limit_acc]*1e-6

client_i_2 = client_i[limit_acc_inf:limit_acc]*1e-6
index_i_2 = index_cli_i[limit_acc_inf:limit_acc]*1e-6

client_ii_2 = client_ii[limit_acc_inf:limit_acc]*1e-6
index_ii_2 = index_cli_ii[limit_acc_inf:limit_acc]*1e-6

server_1 = server[limit_acc_inf:limit_acc]
index_server_1 = index_server[limit_acc_inf:limit_acc]*1e-6

### Accuracy

fig = plt.figure(figsize=(12,6))
ax = fig.add_subplot(1, 1, 1)

ax.scatter(index_1_2,client_1_2,color='#17becf',s=150)
ax.scatter(index_end_2,client_end_2,color='#7f7f7f',s=100)
ax.scatter(index_i_2,client_i_2,s=70)
ax.scatter(index_ii_2,client_ii_2,s=30)
# ax.scatter(server_1,index_server_1,s=10)
list_legend = ['Client 1', 'Client i', 'Client ii','Client end']
ax.grid()
plt.title('Timestamps from simulated node clocks',fontsize=18)
ax.legend(list_legend,fontsize=14,shadow=True)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
plt.xlabel("Real time from laptop clock [s]",fontsize=18);
plt.ylabel('Simulated clock [us]',fontsize=18);
# plt.savefig('clock_acc.pdf')
plt.show()




#%%
### Accuracy 2

limit_acc_inf = 0
limit_acc = 15

client_1_3 = client_1[limit_acc_inf:limit_acc]*1e-6
index_1_3 = index_cli_1[limit_acc_inf:limit_acc]*1e-6

client_end_3 = client_end[limit_acc_inf:limit_acc]*1e-6
index_end_3 = index_cli_end[limit_acc_inf:limit_acc]*1e-6

client_i_3 = client_i[limit_acc_inf:limit_acc]*1e-6
index_i_3 = index_cli_i[limit_acc_inf:limit_acc]*1e-6

client_ii_3 = client_ii[limit_acc_inf:limit_acc]*1e-6
index_ii_3 = index_cli_ii[limit_acc_inf:limit_acc]*1e-6

server_1_3 = server[limit_acc_inf:limit_acc]*1e-6
index_server_1_3 = index_server[limit_acc_inf:limit_acc]*1e-6


#%%


fig = plt.figure(figsize=(12,6))
ax = fig.add_subplot(1, 1, 1)

ax.scatter(index_1_3,client_1_3,color='#17becf',s=400)
ax.scatter(index_i_3,client_i_3,s=250)
ax.scatter(index_ii_3,client_ii_3,s=100)
ax.scatter(index_end_3,client_end_3,color='#7f7f7f',s=70)
ax.scatter(index_server_1_3,server_1_3,color ='#e377c2',s=50)
list_legend = ['Client 1', 'Client i', 'Client ii','Client end','Server']
ax.grid()
plt.title('Timestamps from simulated node clocks',fontsize=18)
ax.legend(list_legend,fontsize=14,shadow=True)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
plt.xlabel("Real time from laptop clock [s]",fontsize=18);
plt.ylabel('Simulated clock [s]',fontsize=18);
plt.savefig('clock_acc_2.pdf')
plt.show()

