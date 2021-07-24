#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec 10 09:57:20 2020

@author: lucascalam
"""
import numpy as np
import csv
import matplotlib.pyplot as plt

reader = csv.reader(open("data_4/server.csv"), delimiter=",")
# reader = csv.reader(open("server.csv"), delimiter=",")

NUM_ITER_SIM = 1
loops_for_drift = 1200
micro_in_seconds = 1000000
ct_server = 1
arr_size = loops_for_drift * NUM_ITER_SIM
index = np.arange(arr_size)
# index_server = np.arange(arr_size+ct_server)
server_index = np.zeros(arr_size+NUM_ITER_SIM-1)
server_time = np.zeros(arr_size+NUM_ITER_SIM-1)
client_1_index = np.zeros(arr_size)
client_1_time = np.zeros(arr_size)
client_end_index = np.zeros(arr_size)
client_end_time = np.zeros(arr_size)


i=0
flag = 0
offset = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        if(offset == 0):
            offset = int(row[1][:-1] + '0')
        print(row)        
        server_index[i] = int(row[2]) + (int(row[1]) - offset)*micro_in_seconds
        server_time[i] = int(row[3])
        i = i + 1
print(f'muestras totales: {i}')


#%%
reader = csv.reader(open("data_4/client_i.csv"), delimiter=",")
# reader = csv.reader(open("client_i.csv"), delimiter=",")
i=0
flag = 0
# offset = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        # if(offset == 0):
            # offset = int(row[1][:-1] + '0')
        print(row)        
        client_1_index[i] = int(row[2]) + (int(row[1]) - offset)*micro_in_seconds
        client_1_time[i] = int(row[3])
        i = i + 1
print(f'muestras totales: {i}')
    
#%%
reader = csv.reader(open("data_4/client_end.csv"), delimiter=",")
# reader = csv.reader(open("client_end.csv"), delimiter=",")
i=0
flag = 0
# offset = 0
for row in reader:
    if(flag == 0):
        flag = 1
        pass
    else:
        # if(offset == 0):
            # offset = int(row[1][:-1] + '0')
        print(row)        
        client_end_index[i] = int(row[2]) + (int(row[1]) - offset)*micro_in_seconds
        client_end_time[i] = int(row[3])
        i = i + 1
print(f'muestras totales: {i}')

#%%

samples = 1100

lim_inf_server = 34
lim_sup_server = lim_inf_server + samples
lim_sup_server_for_end_1 = lim_inf_server + int(samples/2)
lim_sup_server_for_end_2 = lim_sup_server_for_end_1 + int(samples/2)
lim_inf_1 = 7
lim_sup_1 = lim_inf_1 + samples
lim_inf_end_1 = 0
lim_sup_end_1 = lim_inf_end_1 + int(samples/2)
lim_inf_end_2 = lim_inf_end_1 + int(samples/2)
lim_sup_end_2 = lim_inf_end_2 + int(samples/2)


fig = plt.figure(figsize=(12,8))
ax = fig.add_subplot(1, 1, 1)

# dif_1 = client_1_time[:240] - server_time[:240]
# dif_end = client_end - server[:len(server)-ct_server]

# aux = dif_1[240:] - 250000
# dif_1[240:] = aux

# ax.scatter(server_index,server_time,color='#7f7f7f')
# ax.scatter(client_1_index,client_1_time)

# ax.scatter(server_index[0:245],server_time[0:245],color='k',s=100)
# ax.scatter(client_1_index[235:245],client_1_time[235:245],color = 'r')

### End and server
ax.scatter(server_index[lim_inf_server:lim_sup_server],server_time[lim_inf_server:lim_sup_server],color='k',s=120)
ax.scatter(client_1_index[lim_inf_1:lim_sup_1],client_1_time[lim_inf_1:lim_sup_1],color = 'g',s=60)
ax.scatter(client_end_index[lim_inf_end:lim_sup_end],client_end_time[lim_inf_end:lim_sup_end],color = 'r')

# ax.scatter(server_index[:240],dif_1,color='#17becf')
# ax.scatter(index_server,server,color='#7f7f7f')


# ax.set_yscale('log')
ax.grid()
plt.title('Simulation',fontsize=14)
ax.legend(['Server','Client_1','Client_end'],fontsize=12,shadow=True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('Real time [us]',fontsize=14);
plt.ylabel('Simulated time [us]',fontsize=14);
# plt.savefig('EJ1_b.pdf')
plt.show()

#%%

### Difference

lim_inf = 0
lim_sup = 10

# tmp_client_1_time = np.insert(client_1_time,loops_for_drift,server_time[loops_for_drift])
# tmp_client_end_time = np.insert(client_end_time,loops_for_drift,server_time[loops_for_drift])
# dif_1 = tmp_client_1_time[:-10] - server_time[10:]
# dif_2 = tmp_client_end_time[:-18] - server_time[18:]

dif_1 = client_1_time[lim_inf_1:lim_sup_1] - server_time[lim_inf_server:lim_sup_server]
dif_2 = client_end_time[lim_inf_end_1:lim_sup_end_1] - server_time[lim_inf_server:lim_sup_server_for_end]
dif_3 = client_end_time[lim_inf_end_2:lim_sup_end_2] - server_time[lim_sup_server_for_end_1:lim_sup_server_for_end_2]

idx = server_index[lim_inf_server:lim_sup_server]
idx2 = server_index[lim_inf_server:lim_sup_server_for_end_1]
idx3 = server_index[lim_sup_server_for_end_1:lim_sup_server_for_end_2]

#%%
fig = plt.figure(figsize=(12,8))
ax = fig.add_subplot(1, 1, 1)

# ax.scatter(idx,dif_1,color='#7f7f7f')
# ax.scatter(idx2,dif_2)

ax.scatter(idx3,dif_3)


# ax.set_yscale('log')
ax.grid()
plt.title('Difference',fontsize=14)
ax.legend(['Client_1 - Server','Client_1','Client_end'],fontsize=12,shadow=True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('Real time',fontsize=14);
plt.ylabel('Simulated time [ms]',fontsize=14);
# plt.savefig('EJ1_b.pdf')
plt.show()
