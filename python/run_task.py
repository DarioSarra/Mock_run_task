import datetime
import threading
import serial
import time
# import sys
import openpyxl
import os
import preprocessing
import pandas as pd
import matplotlib.pyplot as plt

class Box(object):
    def __init__(self, port, number):
        self.port = port
        self.number = number

def runSerial(fname, box, parameters):

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
    baud = '115200'
    fmode = 'ab'            #Opens a file for appending in binary format

    port = serial.Serial(box.port, baud)
    outf = open(fname,fmode)

    #with serial.Serial(addr,baud) as port, open(fname,fmode) as outf:
    if port.isOpen()==False:
        port.open()

    print("WAIT FOR IT!!!!\n\n")

    time.sleep(2.5)

    #send parameters to start script and assign reward probabilities and amount etc.
    for parameter in parameters:
        time.sleep(0.5)
        signal = str(chr(int(parameter))).encode('ascii')
        port.write(signal)
    global stopper
    stopper[box.number] = False
    while port.isOpen() & ~stopper[box.number]:
        if port.inWaiting()>0:
            try:
                x = port.readline()
                if b'-666' in x:
                    print("All is well in box %d!!!\n" %box.number)
                outf.write(x)
                outf.flush()

            except:
                print("Error in box %d!!!\n" %box.number)
                outf.write("Error")
                outf.flush()
    # Add preprocessing to get csv version of dataframe
    dataframe = preprocessing.preprocess_data(fname)
    csv_fname = fname[:-4]+'.csv'
    dataframe.to_csv(csv_fname)
    print("I'm done in box %d!!!\n" %box.number)

def start_box(box_number):
    # Get keyboard input
    animal = input("What is name of the animal?")
    weight = input("What's its weight?")
    #box_number = int(input("What box?"))

    protocol_number = input("Protocol? \n" \
    "1) Low Protocol and delta = 0 \n" + \
    "2) High Protocol and delta = 0 \n " + \
    "3) Random Protocol and delta = 0.05 \n" + \
    "4) Random Protocol and delta = 0.5\n" + \
    "5) Initial training Baseline\n"+ \
    "6) ILow Protocol with barrier and delta =0 \n")

    #SessionStim = input("Stimulation?\n" \
    #"0) No \n" \
    #"1) Yes\n" )
    SessionStim = 0

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
        filename = animal+'_'+str(timestamp) + session + '.txt'
        fname = '/Users/dariosarra/Google Drive/Flipping/Mock_run_task/raw_data/' + filename
        iC += 1
        searching = os.path.isfile(fname)
    parameters = [int(protocol_number), int(box_number), int(SessionStim)]
    # update data library
    newRow = [filename,filename[:-4]+'.csv', animal, int(timestamp), session, int(weight)] + parameters
    xfile = openpyxl.load_workbook('/Users/dariosarra/Google Drive/Flipping/Mock_run_task/datalibrary.xlsx')
    xfile.active.append(newRow)
    xfile.save('/Users/dariosarra/Google Drive/Flipping//Mock_run_task/datalibrary.xlsx')
    # Write latest filename
    global fnames
    global procs
    fnames[box_number] = fname
    df = pd.DataFrame({'name' : fnames})
    df.to_csv('/Users/dariosarra/Google Drive/Flipping/Mock_run_task/raw_data/names.csv')
    #Connect to arduino, reset, give parameters, save data!

    #runSerial(fname, box[box_number], parameters)
    procs[box_number] = threading.Thread(target=runSerial, args=([fname, box[box_number], parameters]))
    procs[box_number].start()


#
#
def close_all():
    global stopper
    for i in range(len(stopper)):
        stopper[i] = True
#
def stop_box(i):
    global stopper
    stopper[i] = True

#
#def check_process(i):
#    procs[i].isAlive()

if __name__ == '__main__':
    global stopper
    global fnames
    global procs
    stopper = [False, False, False, False, False]
    fnames = ['x', 'x', 'x', 'x', 'x']
    procs = [None]*5
    box = [None]*5
    box[3] = Box('/dev/tty.usbmodem1421', 3)
#    procs[0] = threading.Thread(target=visualize_data, args=([]))
#    procs[2] = threading.Thread(target=run_box, args=([box2]))
#    procs[4] = threading.Thread(target=run_box, args=([box4]))
