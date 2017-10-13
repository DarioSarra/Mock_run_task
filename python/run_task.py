import datetime
import threading
import serial
import time
# import sys
import openpyxl
import os
import preprocessing
import pandas as pd


#####################################################################################
# This is what sets communication with arduino
def runSerial(fname, box_number, box_port, parameters):
    ##    fname = file location
    ##    box = what box you are using
    ##    highprob = e.g. 45 or 90
    ##    flippinggamma = e.g. 15
    ##    rewardsize  = 2 or 4
    ##    box = e.g. 2 or 4
    ##    stimulation = 0 or 1
    ##    protocol = highprob, flippinggamma, rewardsize, stimulation
    ##    parameters = protocol e box number alla fine

    #print(fname)
    baud = '115200'         #set frequency of communication, must be in agreement with arduino
    fmode = 'ab'            #Opens a file for appending in binary format, begin the txt raw data

    port = serial.Serial(box_port, baud)
    
    # open(fname,fmode) as outf: technicality for pythin to write arduino's messages
    outf = open(fname,fmode)

    #open port serial.Serial(addr,baud): 
    if port.isOpen()==False:
        port.open()

    print("WAIT FOR IT!!!!\n\n")

    time.sleep(2.5)

    #send parameters to start script:
    for parameter in parameters:
        time.sleep(0.5)
        signal = str(chr(int(parameter))).encode('ascii')
        port.write(signal)
    global stopper #Boolean array set on false for each box, 
    #when is true it stops writing the txt file and close communication with arduino
    stopper[box_number] = False
    #Main loop: as long stopper is false python keep on writing arduino's messages
    while port.isOpen() & ~stopper[box_number]:
        if port.inWaiting()>0:
            try:
                x = port.readline()
                if b'-666' in x:
                    print("All is well in box %d!!!\n" %box_number)
                outf.write(x)
                outf.flush()

            except:
                print("Error in box %d!!!\n" %box_number)
                outf.write("Error")
                outf.flush()
    #When stopper is true Python exits the main loop and proceed to preprocessing            
    # Call preprocessing module to get csv version of dataframe
    dataframe = preprocessing.preprocess_data(fname)#fname is the txt file where we are writing
    csv_fname = fname[:-4]+'.csv' #change extension to create the name for the csv
    dataframe.to_csv(csv_fname)#save the preprocess data in a csv file
    print("I'm done in box %d!!!\n" %box_number)
################################################################################
def start_box(box_number):
    # Get keyboard input
    animal = input("What is name of the animal?")
    weight = input("What's its weight?")

    protocol_number = input("Protocol? \n" \
    "1) Low Protocol and delta = 0 \n" + \
    "2) High Protocol and delta = 0 \n" + \
    "3) Random Protocol and delta = 0.05 \n" + \
    "4) Random Protocol and delta = 0.5\n" + \
    "5) Initial training Baseline\n"+ \
    "6) Low Protocol with barrier and delta =0 \n" + \
    "7) High Protocol with barrier and delta =0 \n")

    SessionStim = input("Stimulation?\n" \
    "0) No \n" \
    "1) Yes\n" )
    #SessionStim = 0

    # get timestamp and set filename
    year = datetime.datetime.now().year
    month = datetime.datetime.now().month
    day = datetime.datetime.now().day
    timestamp = (year % 100)*(10**4) + month*(10**2) + day
    #Check if file already exists... otherwise append 'b' or 'c' etc to Session ID

    iC = 97 #char(97): 'a'
    searching = True
    while searching:
        session = chr(iC)
        filename = animal+'_'+str(timestamp) + session + '.txt'#build file name plus char
        fname = os.path.join(preprocessing.raw_data, filename)
        iC += 1 #increment IC to use othe potential char
        searching = os.path.isfile(fname)#search if the filename exist if doesn't exits loop
    parameters = [int(protocol_number), int(box_number), int(SessionStim)] #these will be used by runSerial function
    # update data library
    newRow = [filename,filename[:-4]+'.csv', animal, int(timestamp), session, int(weight)] + parameters
    xfile = openpyxl.load_workbook(preprocessing.datalibrary)
    xfile.active.append(newRow)
    xfile.save(preprocessing.datalibrary)
    # Write latest filename
    global fnames
    global procs
    fnames[box_number] = fname #update box file name
    df = pd.DataFrame({'name' : fnames})
    df.to_csv(preprocessing.csv_address)
    #initiate run serial on a separate thread to keep control of the console, or initiate more boxes
    procs[box_number] = threading.Thread(target=runSerial, args=([fname, box_number, preprocessing.boxes[box_number], parameters]))
    procs[box_number].start()
############################################
def close_all():
    #set true in all boxes and close them
    global stopper
    for i in range(len(stopper)):
        stopper[i] = True
###########################################
def stop_box(i):
    #set true on the box in use to close
    global stopper
    stopper[i] = True
######################$######################
#in case of serious debugging about multithreading
#def check_process(i):
#    procs[i].isAlive()
############################################
#when you send runtask the first time it erases all previous setting
#!!!!do not run "runtask" to solve issue on 1 box if others are running!!!!!
if __name__ == '__main__':
    global stopper
    global fnames
    global procs
    stopper = [False, False, False, False, False]
    fnames = ['x', 'x', 'x', 'x', 'x']
    procs = [None]*5

