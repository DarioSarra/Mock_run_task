# -*- coding: utf-8 -*-
"""
Created on Tue Nov 22 18:45:28 2016

@author: pietro
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import preprocessing
import pandas as pd
import os

def plotter(ax1, data, dt):
    recent_data = data.ix[data.PokeIn > (data.iloc[-1].PokeIn-dt),:]
    ax1.clear()

    for i in recent_data.index:
        if recent_data.ix[i, 'Reward'] == 1:
            color = "orange"
        else:
            color = "gray"
        fill = (recent_data.ix[i,'SideHigh'] == recent_data.ix[i,'Side'])
        if recent_data.ix[i, 'Protocollo'] == 1:
            hatch = 'x'
        else:
            hatch = ''
        y = recent_data.ix[i,'Side']
        x = recent_data.ix[i,'PokeIn']
        height = 0.5
        width = recent_data.ix[i,'PokeOut'] - recent_data.ix[i,'PokeIn']
        ax1.set_yticks([])
        ax1.add_patch(
        patches.Rectangle(
            (x, y),   # (x,y)
            width,          # width
            height,          # height
            hatch = hatch,

            facecolor = color,
            fill = fill
            )
        )
    ax1.set_xlim([data.iloc[-1].PokeIn-dt,data.iloc[-1].PokeOut])
    ax1.set_ylim([0,1.5])

def water(ax2,data):
    note = 'Rewards = %d' %sum(data.Reward)
    ax2.clear()
    ax2.set_xticks([])
    ax2.set_yticks([])
    if (data.shape[0] > 0) :
        ax2.annotate(note, xy = (0.1,0.5))
    else:
        ax2.annotate('READY', xy = (0.1,0.5))

if __name__ == '__main__':
    #plt.ion()
    f, axarr = plt.subplots(2,2, gridspec_kw = {'width_ratios':[3, 1]})
    fnames = ['x','x','x','x','x']
    foldername = os.path.dirname(os.path.dirname(__file__))
    while True:
        try:
            df = pd.read_csv(os.path.join(foldername, 'raw_data', 'names.csv'))
            fnames = df.name
        except:
            pass
        i = 3
        if fnames[i] != 'x':
            try:
                data = preprocessing.preprocess_data(fnames[i])
                plotter(axarr[i//2-1,0],data,60)
                water(axarr[i//2-1,1],data)
            except:
                pass
        plt.show()
        plt.pause(0.05)
