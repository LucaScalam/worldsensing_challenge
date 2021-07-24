#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec 10 09:57:20 2020

@author: lucascalam
"""
import numpy as np
import csv
import matplotlib.pyplot as plt

reader = csv.reader(open("server.csv"), delimiter=",")

ct_server = 1
arr_size = 240 * 2
index = np.arange(arr_size)
index_server = np.arange(arr_size+ct_server)
server = np.zeros(arr_size+ct_server)
client_1 = np.zeros(arr_size)
client_end = np.zeros(arr_size)


i=0
for row in reader:
    if(len(row) == 1):
        # i = i + 1
        pass
    else:
        print(row)
        server[i] = int(row[1])
        i = i + 1
print(f'muestras totales: {i}')
#%%
reader = csv.reader(open("client_i.csv"), delimiter=",")
i=0
for row in reader:
    if(len(row) == 1):
        # i = i + 1
        pass
    else:
        print(row)
        client_1[i] = int(row[1])
        i = i + 1
print(f'muestras totales: {i}')
    
#%%
reader = csv.reader(open("client_end.csv"), delimiter=",")
i=0
for row in reader:
    if(len(row) == 1):
        # i = i + 1
        pass
    else:
        print(row)
        client_end[i] = int(row[1])
        i = i + 1
print(f'muestras totales: {i}')

#%%

### Constelación


fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
# for i in range(len(time[:64])):
#     ax.scatter(x[i],y[i],color='b')

dif_1 = client_1 - server[:len(server)-ct_server]
dif_end = client_end - server[:len(server)-ct_server]

aux = dif_1[240:] - 250000
dif_1[240:] = aux

ax.scatter(index,dif_1,color='#7f7f7f')
# ax.scatter(index,dif_end,color='#17becf')
# ax.scatter(index_server,server,color='#7f7f7f')
# ax.scatter(index,client_end)

# ax.set_yscale('log')
ax.grid()
plt.title('Simulation',fontsize=14)
ax.legend(['Server','Client_1','Client_end'],fontsize=12,shadow=True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('index',fontsize=14);
plt.ylabel('time [us]',fontsize=14);
# plt.savefig('EJ1_b.pdf')
plt.show()

#%%


### pulsos
limit_inf = 0
limit_sup = 100
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.plot(time[limit_inf:limit_sup],x[limit_inf:limit_sup])
ax.plot(time2[limit_inf:limit_sup],y[limit_inf:limit_sup])
ax.grid()
plt.title('Rx',fontsize=14)
ax.legend(['inphase','qua'],fontsize=12,shadow=True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
plt.axhline(color='black', lw=0.5)
plt.axvline(color='black', lw=0.5)
plt.xlabel('time',fontsize=14);
plt.ylabel('Amp',fontsize=14);
# plt.savefig('EJ1_b.pdf')
plt.show()
    
time#%%

### Filtro
k   = np.arange(-53,54)
lam = 1550
c   = 300e3
Ts  = 25
D   = 16.75
L   = 500

c_k = np.ones(107).astype('complex')
c_k = np.sqrt(1j*c*Ts*Ts/(D*lam*lam*L))*np.exp(-1j*np.pi*c*Ts*Ts*k*k/(D*lam*lam*L))

# eq_signal = np.convolve(x,c,mode = 'full') + 1j*np.convolve(y,c,mode = 'full')
eq_signal = np.convolve(x+1j*y,c_k,mode = 'full')

#%%

### Sin eq vs eq --- In phase
time3 = np.arange(eq_signal.shape[0])

zer_1 = -0
# zer = 1500
zer = 25000
### pulsos eq
limit_inf = int(k.shape[0]/2 - zer_1 + zer)
limit_inf2 = zer
num_points = 1500
limit_sup = int(k.shape[0]/2 - zer_1 +zer )+num_points
limit_sup2 = num_points+limit_inf2


max_eq = np.max(np.abs(eq_signal.real[limit_inf:limit_sup]))
max_x = np.max(np.abs(x[limit_inf:limit_sup]))
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

###Plot de la comp in phase
ax.plot(time[limit_inf2:limit_sup2]/1e-9,x[limit_inf2:limit_sup2]/max_x,color = '#7f7f7f')

###Plot de eq_signal en los tiemps de las 65000 muestras
ax.plot(time[limit_inf2:limit_sup2]/1e-9,eq_signal.real[limit_inf:limit_sup]/max_eq,color = '#17becf')

ax.grid()
plt.title('Rama "Inphase"',fontsize=14)
ax.legend(['sin ecu','con ecu'],fontsize=12,shadow=True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
plt.axhline(color='black', lw=0.7)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('Tiempo [ns]',fontsize=14);
plt.ylabel('Amp [norm]',fontsize=14);
plt.xlim(time[zer]/1e-9,time[zer+num_points]/1e-9)
# plt.savefig('EJ1_b.pdf')
plt.show()

#%%

### Sin eq vs eq --- Quadrature
time3 = np.arange(eq_signal.shape[0])

zer_1 = 0
zer = 25000
### pulsos eq
limit_inf = int(k.shape[0]/2 - zer_1 + zer)
limit_inf2 = zer
num_points = 1500
limit_sup = int(k.shape[0]/2 - zer_1 +zer )+num_points
limit_sup2 = num_points+limit_inf2


max_eq = np.max(np.abs(eq_signal.imag[limit_inf:limit_sup]))
max_x = np.max(np.abs(y[limit_inf2:limit_sup2]))
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

###Plot de la comp in phase
ax.plot(time[limit_inf2:limit_sup2]/1e-9,y[limit_inf2:limit_sup2]/max_x,color = '#7f7f7f')

###Plot de eq_signal en los tiemps de las 65000 muestras
ax.plot(time[limit_inf2:limit_sup2]/1e-9,eq_signal.imag[limit_inf:limit_sup]/max_eq,color = '#17becf')

ax.grid()
plt.title('Rama "Quadrature"',fontsize=14)
ax.legend(['sin ecu','con ecu'],fontsize=12,shadow=True)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
plt.axhline(color='black', lw=0.7)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('Tiempo [ns]',fontsize=14);
plt.ylabel('Amp [norm]',fontsize=14);
plt.xlim(time[zer]/1e-9,time[zer+num_points]/1e-9)
# plt.savefig('EJ1_b.pdf')
plt.show()
#%%

# ### SSolo equ -- Plot ayuda
# time3 = np.arange(eq_signal.shape[0])


# ### pulsos eq
# a = int(k.shape[0]/2+250)
# limit_inf = a
# limit_inf2 = 0
# # limit_inf = 10000
# # limit_sup = k.shape[0] + x.shape[0]
# num_points = 2000
# limit_sup = a+num_points
# limit_sup2 = num_points


# max_eq = np.max(np.abs(eq_signal.real[limit_inf:limit_sup]))
# max_x = np.max(np.abs(x[limit_inf:limit_sup]))
# fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)

# ###Plot de la comp in phase
# # ax.plot(time[limit_inf2:limit_sup2],x[limit_inf2:limit_sup2]/max_x)

# ###Plot de eq_signal en los tiemps de las 65000 muestras
# ax.plot(time3[limit_inf:limit_sup],eq_signal.real[limit_inf:limit_sup]/max_eq)

# ### Plot de eq_signal en todos los tiempos(90000)
# # ax.plot(time3[limit_inf:limit_sup],eq_signal.real[limit_inf:limit_sup]/max_eq)

# # ax.plot(time2[limit_inf:limit_sup],eq_signal.imag[limit_inf:limit_sup])

# ax.grid()
# plt.title('Rama inphase',fontsize=14)
# # ax.legend(['sin equ','con equ'],fontsize=12,shadow=True)
# plt.xticks(fontsize=14)
# plt.yticks(fontsize=14)
# plt.axhline(color='black', lw=0.5)
# plt.axvline(color='black', lw=0.5)
# plt.xlabel('time',fontsize=14);
# plt.ylabel('Amp',fontsize=14);
# plt.xlim(limit_inf,limit_sup)
# # plt.savefig('EJ1_b.pdf')
# plt.show()

# #%%

# ### Constelación con ecu

# zer_1 = -0
# zer = 512
# ### pulsos eq
# limit_inf = int(k.shape[0]/2 - zer_1 + zer)
# num_points = int(65536)
# limit_sup = int(k.shape[0]/2 - zer_1 +zer )+num_points


# max_eq_real = np.max(np.abs(eq_signal.real[limit_inf:limit_sup:128]))
# max_eq_imag = np.max(np.abs(eq_signal.imag[limit_inf:limit_sup:128]))

# max_x = np.max(np.abs(x[64::128]))
# max_y = np.max(np.abs(y[64::128]))


# fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)
# # for i in range(len(time[:64])):
# #     ax.scatter(x[i],y[i],color='b')

# ax.scatter(x[64::128]/max_x,y[64::128]/max_y,color='#7f7f7f')
# # ax.scatter(x[64::128],y[64::128],color='#17becf')

# ###Plot de la comp in phase
# ax.scatter(eq_signal.real[limit_inf:limit_sup:128]/max_eq_real,
#             eq_signal.imag[limit_inf:limit_sup:128]/max_eq_imag,
#             color = '#17becf')

# ax.grid()
# plt.title('Constelación',fontsize=14)
# ax.legend(['Sin ecu','Con ecu'],fontsize=12,shadow=True)
# plt.xticks(fontsize=14)
# plt.yticks(fontsize=14)
# plt.axhline(color='black', lw=0.8)
# plt.axvline(color='black', lw=0.8)
# plt.xlabel('In phase',fontsize=14);
# plt.ylabel('Quadrature',fontsize=14);
# # plt.savefig('EJ1_b.pdf')
# plt.show()

#%%

### Usando modo 'same' en la convolución

c_k = np.ones(107).astype('complex')
c_k = np.sqrt(1j*c*Ts*Ts/(D*lam*lam*L))*np.exp(-1j*np.pi*c*Ts*Ts*k*k/(D*lam*lam*L))

# eq_signal = np.convolve(x,c,mode = 'full') + 1j*np.convolve(y,c,mode = 'full')
eq_signal = np.convolve(x+1j*y,c_k,mode = 'same')


#%%

### Sin eq vs eq --- In phase
time3 = np.arange(eq_signal.shape[0])

zer_1 = -0
# zer = 1500
zer = 100
### pulsos eq

limit_inf2 = zer
num_points = 100
limit_sup2 = num_points+limit_inf2


# max_eq = np.max(np.abs(eq_signal.real[limit_inf:limit_sup]))
max_eq = 1e-3
# max_x = np.max(np.abs(x[limit_inf:limit_sup]))
max_x = 1e-3
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

###Plot de la comp in phase
ax.plot(time[limit_inf2:limit_sup2]/1e-9,x[limit_inf2:limit_sup2]/max_x,color = '#7f7f7f')

###Plot de eq_signal en los tiemps de las 65000 muestras
ax.plot(time[limit_inf2:limit_sup2]/1e-9,eq_signal.real[limit_inf2:limit_sup2]/max_eq,color = '#17becf')

ax.grid()
plt.title('Rama "Inphase"',fontsize=14)
ax.legend(['Sin ecu','Con ecu'],fontsize=12,shadow=True)
plt.xticks(fontsize=11)
plt.yticks(fontsize=11)
plt.axhline(color='black', lw=0.7)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('Tiempo [ns]',fontsize=12);
plt.ylabel('Amp [mV]',fontsize=12);
plt.xlim(time[zer]/1e-9,time[zer+num_points]/1e-9)
plt.savefig('EJ3_a_2.pdf')
plt.show()


#%%

### Sin eq vs eq --- Quadrature
time3 = np.arange(eq_signal.shape[0])

zer_1 = -0
# zer = 1500
zer = 100
### pulsos eq

limit_inf2 = zer
num_points = 100
limit_sup2 = num_points+limit_inf2


# max_eq = np.max(np.abs(eq_signal.real[limit_inf:limit_sup]))
max_eq = 1e-3
# max_x = np.max(np.abs(x[limit_inf:limit_sup]))
max_x = 1e-3
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

###Plot de la comp in phase
ax.plot(time[limit_inf2:limit_sup2]/1e-9,y[limit_inf2:limit_sup2]/max_x,color = '#7f7f7f')

###Plot de eq_signal en los tiemps de las 65000 muestras
ax.plot(time[limit_inf2:limit_sup2]/1e-9,eq_signal.imag[limit_inf2:limit_sup2]/max_eq,color = '#17becf')

ax.grid()
plt.title('Rama "Quadrature"',fontsize=14)
ax.legend(['Sin ecu','Con ecu'],fontsize=12,shadow=True)
plt.xticks(fontsize=11)
plt.yticks(fontsize=11)
plt.axhline(color='black', lw=0.7)
# plt.axvline(color='black', lw=0.5)
plt.xlabel('Tiempo [ns]',fontsize=12);
plt.ylabel('Amp [mV]',fontsize=12);
plt.xlim(time[zer]/1e-9,time[zer+num_points]/1e-9)
plt.savefig('EJ3_b_2.pdf')
plt.show()

#%%

# max_eq_real = np.max(np.abs(eq_signal.real[64::128]))
max_eq_real = 1e-3
# max_eq_imag = np.max(np.abs(eq_signal.imag[64::128]))
max_eq_imag = 1e-3

# max_x = np.max(np.abs(x[64::128]))
max_x = 1e-3
# max_y = np.max(np.abs(y[64::128]))
max_y = 1e-3


fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
inicio = 7
### Sin Ecu
ax.scatter(x[inicio::8]/max_x,y[inicio::8]/max_y,color='#7f7f7f')
### Con Ecu
ax.scatter(eq_signal.real[inicio::8]/max_eq_real,
           eq_signal.imag[inicio::8]/max_eq_imag,
           color = '#17becf')

ax.grid()
plt.title('Constelación con y sin ecualización',fontsize=14)
ax.legend(['Sin ecu','Con ecu'],fontsize=12,shadow=True)
# plt.xticks([-.002,-.001,0,.001,.002],fontsize=14)
# plt.yticks([-.002,-.001,0,.001,.002],fontsize=14)
plt.xticks(fontsize=11)
plt.yticks(fontsize=11)
# ax.set_yticklabels([])
# ax.set_xticklabels([])
plt.axhline(color='black', lw=0.8)
plt.axvline(color='black', lw=0.8)
plt.xlabel('In phase [mV]',fontsize=12);
plt.ylabel('Quadrature [mV]',fontsize=12);
plt.savefig('EJ3_c_2.pdf')
plt.show()
