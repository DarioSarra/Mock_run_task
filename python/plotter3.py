# -*- coding: utf-8 -*-
"""
Created on Tue Nov 22 18:45:28 2016

@author: pietro
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import preprocessing
import pandas as pd

####### tocalculate amount of events
def counter(df,col):
    streak = [1]
    for i in range(1,df.shape[0]):
        if df[col][i]!=df[col][i-1]:
            streak.append(streak[i-1]+1)
        else:
            streak.append(streak[i-1])
    value = max(streak)
    return value

#############
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
        if recent_data.ix[i, 'Stim'] == 1:
            stimolato = 'blue'
        else:
            stimolato = 'black'
        if recent_data.ix[i, 'Wall'] == 1:
            hatch = '-'
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
            edgecolor = stimolato,
            facecolor = color,
            fill = fill,
            linewidth = 1.3
            )
        )
    ax1.set_xlim([data.iloc[-1].PokeIn-dt,data.iloc[-1].PokeOut])
    ax1.set_ylim([0,1.5])

def water(ax2,data):
    note1 = 'Rewards = %d' %sum(data.Reward)
    note2 = 'Streak = %d' %counter(data,'Side')
    note3 = 'Block = %d' %counter(data,'Wall')
    ax2.clear()
    ax2.set_xticks([])
    ax2.set_yticks([])
    if (data.shape[0] > 0) :
        ax2.annotate(note1, xy = (0.1,0.75))
        ax2.annotate(note2, xy = (0.1,0.5))
        ax2.annotate(note3, xy = (0.1,0.25))
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
