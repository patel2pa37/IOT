import pandas as pd
import math
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage.filters import gaussian_filter1d
import statistics
import serial
import time

'''
Python scripts for finding steps,
some assumptions:
    1)The structure of the data is going to be zero acceleration 
    followed by the step data
    2)The step data is continuous, not break or no interruption  
'''

def get_Data():
    portName='COM11'
    baudRate = 9600

    ser = serial.Serial(portName,baudRate)
    print("Opening port "+ ser.name)

    if ser.is_open==True:
        print("Success!")
    else:
        print("Unable to open port :(")

    print("Running a demo for 10s...")
    startTime = currentTime = int(time.time())

        #decode bytes into string
    '''
    lineData = []
    while abs(currentTime-startTime) < 20:
        bytes = ser.readline()
        line = bytes.decode('utf-8')
        line = line.replace('\r','').replace('\n','')
        print(line)
        lineData.append(line)
        currentTime=int(time.time())
    x = []
    for d in lineData:
        if 'n' in d:
            pass
        else:
            try:
                x.append(float(d))
            except Exception:
                pass
    '''
    lineData = []
    while abs(currentTime-startTime)<20:
        bytes = ser.readline()
        line = bytes.decode('utf-8')
        line = line.replace('\r','').replace('\n','')
        print(line)
        lineData.append(line)
        currentTime=int(time.time())
    x = []
    for d in lineData:
        if len(d)>5:
            x.append(d.split('//'))

    x_1 = []
    for num in x:
        try:
            x_1.append(math.sqrt((int(num[0])*int(num[0]))+(int(num[1])*int(num[1]))+(int(num[2])*int(num[2]))))
        except Exception:
            pass

    plt.plot(x_1)
    plt.show()
    arr = np.array(x_1)
    np.savetxt('array3.csv', [arr],delimiter=',',fmt='%d')
    
    return x_1




#gets the data
def getData(fileName):
    data = pd.read_csv(fileName)
    StepData = []
    for d in range(len(data['x'])):
        magnitudeData = (data['x'][d])**2 + (data['y'][d])**2 + (data['z'][d])**2
        StepData.append(math.sqrt(magnitudeData))
    return StepData


#finds the starting and stopping positions
def StartingandStopingPointFinder(SmoothedArray):
    slopChanges = []
    count = 0
    
    while count < (len(SmoothedArray)-5):
        slopChanges.append(statistics.stdev(SmoothedArray[count:count+5]))
        count = count + 5
        
    standerDv = np.array(slopChanges)
    meanOfArray = standerDv.mean()
    
    NonZeros = [0,0]
    zeros = [0,0]
    startingPointFound = False
    endingPointFound = False
    starting = 0
    ending = 0
    
    for num in range(len(slopChanges)):
        if slopChanges[num]<2:          
            if zeros[0] == 2:
                NonZeros[0] = 0
                NonZeros[1] = 0 
            if zeros[1] == 0:    
                zeros[1] = num
            zeros[0] = zeros[0] + 1
            
        elif slopChanges[num]>2:          
            if NonZeros[0] == 2:
                zeros[0] = 0
                zeros[1] = 0
            if NonZeros[1] == 0:
                NonZeros[1] = num
            NonZeros[0] = NonZeros[0] + 1
            
        if NonZeros[0]>10 and startingPointFound == False:
            starting = NonZeros[1]
            startingPointFound = True

        if zeros[0] > 3 and ((((standerDv[num:]).mean())/meanOfArray) < .70) and startingPointFound == True:
            ending = zeros[1]
            endingPointFound = True
            
        if endingPointFound == True:
            break

    
    if ending == 0:
        ending = len(slopChanges)-1
    
    return SmoothedArray[starting*5:ending*5], slopChanges, meanOfArray


#positive slop is 1 and negative slop is -1
def getSlop(inputArray):
    array_ = []
    for t in range(len(inputArray)):
        if (inputArray[t] - inputArray[t-1] > 0):
            array_.append(1)
        else:
            array_.append(-1)
    return array_


#Counts the steps
def CountTheOnes(inputArray):
    CountOfOnes = 0
    iterationPlace = 0
    consecutiveOnes  = 0
    
    while iterationPlace < len(inputArray):
        if inputArray[iterationPlace] == 1:
            consecutiveOnes = consecutiveOnes + 1
            iterationPlace = iterationPlace + 1
        elif inputArray[iterationPlace] == -1:
            if consecutiveOnes >= 6:
                CountOfOnes = CountOfOnes + 1
            consecutiveOnes = 0
            iterationPlace = iterationPlace + 1
    return CountOfOnes

arr_data_1 = [453,370,282,193,261,376,402,283,330,276,385,532,448,306,268,257,277,369,409,266,319,330,412,502,449,367,300,270,397,472,416,331,362,328,444,496,342,377,419,391,356,523,429,347,372,248,387,434,374,464,379,381,332,523,444,421,357,223,277,363,570,376,327,288,254,467,626,447,278,274,439,477,274,352,279,390,459,176,303,289,404,230,174,382,625,261,240,243,573,234,239,367,517,349,329,164,378,397,150,292,378,424,454,236,190,238,228,295,286,201,230,290,317,338,381,270,210,201,190,383,282,278,253,158,228,415,408,240,210,154,283,376,386,223,232,199,256,380,431,242,251,167,240,338,373,317,222,159,203,376,455,289,255,164,214,420,364,237,177,187,237,346,446,321,209,160,288,346,309,251,208,209,150,358,430,366,219,186,187,358,376,283,237,196,215,281,419,385,277,205,133,268,401]

#GetData = getData('48_steps.csv')
GetData = get_Data()
GetStartAndStop, standerDV, meanSD = StartingandStopingPointFinder(GetData)
GetSlop = getSlop(gaussian_filter1d(GetStartAndStop, sigma = 4))
GetCount = CountTheOnes(GetSlop)
print('steps count is:', GetCount)

#plt.plot(GetData)
#plt.plot(x_1)
#plt.plot(GetSlop)
#plt.show()
