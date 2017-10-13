# -*- coding: utf-8 -*-
"""
Created on Fri May  5 14:06:14 2017

@author: Serotonin
"""
import preprocessing

fname = 'C:\\Users\\Serotonin\\Google Drive\\Flipping\\run_task_photo\\raw_data\\DN3_170818a.txt'
dataframe = preprocessing.preprocess_data(fname)
csv_fname = fname[:-4]+'.csv'
dataframe.to_csv(csv_fname)
