# -*- coding: utf-8 -*-
"""
Created on Mon Oct 24 14:00:00 2016

@author: pietro
"""

import numpy as np
import pandas as pd

def int2bin(i, n):
    v = np.zeros(n, dtype = int)
    curr_i = i
    for j in range(0,n):
        remainder = curr_i % 2
        v[j] = remainder
        curr_i = (curr_i-remainder)//2
    return v

#'DR4_160525a.txt'
# 1 is left!!!!
def preprocess_data(filename):
    n = 6 #num variables!
    proc_data = np.zeros((0,n+7), dtype = float)
    cols = ['PokeIn', 'PokeOut', 'Reward', 'Side', 'SideHigh', 'Protocollo', 'Stim', 'Wall', 'ProbVec0','ProbVec1', 'GamVec0','GamVec1', 'delta']
    data = pd.read_csv(filename, delimiter = '\t', dtype = int, names = ['code','time'])
    data_end = data[data['code'] == -1].index[-1]
    b = data.ix[6:data_end, :]
    params = 100+np.array(data.code[1:6]) # That's how stuff is encoded in arduino
    pokein = b[b['code'] >= 0].index
    pokeout = b[b['code'] == -1].index
    for ind,val in enumerate(pokein):
        next_row = np.zeros((1,n+7),dtype=float)
        next_row[0,0] = data.ix[val,'time']/1000
        next_row[0,1] = data.ix[pokeout[ind],'time']/1000
        next_row[0,2:(2+n)] = int2bin(data.ix[val,'code'],n) # Side 1 is left!
        next_row[0,(2+n):] = params
        proc_data = np.append(proc_data, next_row, axis = 0)
    dataframe = pd.DataFrame(proc_data, columns = cols)
    return dataframe
