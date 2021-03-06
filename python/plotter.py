# -*- coding: utf-8 -*-
"""
Created on Tue Nov 22 18:45:28 2016

@author: pietro
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import preprocessing
import pandas as pd

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
    ordered_keys = sorted(preprocessing.boxes.keys())
    n_boxes = len(ordered_keys)
    f, axarr = plt.subplots(n_boxes,2, gridspec_kw = {'width_ratios':[3, 1]})
    fnames = ['x','x','x','x','x']
    while True:
        try:
            df = pd.read_csv(preprocessing.csv_address)
            fnames = df.name
        except:
            pass
        for i in range(n_boxes):
            box_number = ordered_keys[i]
            if fnames[box_number] != 'x':
                try:
                    data = preprocessing.preprocess_data(fnames[box_number])
                    if n_boxes > 1:
                        plotter(axarr[i,0],data,60)
                        water(axarr[i,1],data)
                    else:
                        plotter(axarr[0],data,60)
                        water(axarr[1],data)
                except:
                    pass
        plt.show()
        plt.pause(0.05)
