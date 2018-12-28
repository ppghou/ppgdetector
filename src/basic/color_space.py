import re
import matplotlib.pyplot as plt
import numpy as np
import sklearn
from sklearn import svm
from sklearn import linear_model, tree
from sklearn.ensemble import RandomForestClassifier
from sklearn import neural_network
import copy

path = r"D:\THU\curriculum\Software Engineering\Course_Project\data_1109_0427_RGB_fixed.txt"
f = open(path, "r")

#Read data from file
channels = []
for line in f:
    data = re.split("	", line)
    data = [float(i) for i in data]
    channels.append(data)
    
#Smoothing
length = len(channels[0])
smoothed = copy.deepcopy(channels)
for i in [2,3,4]:
    for j in range(length):
        smoothed[i][j] = np.mean(channels[i][max(j - 5, 0):min(length - 1, j + 5)])
      
#Specify each channel
time = channels[0]
index = np.arange(len(time))
ppg = channels[1]
colors = np.transpose(smoothed[3:])

#Split the training set and test set
training_data = colors[:int(len(colors)*0.9)]
training_label = ppg[:int(len(colors)*0.9)]
test_data = colors[int(len(colors)*0.9):]
test_label = ppg[int(len(colors)*0.9):]

#Rescale the ppg data into the range of 0-1
training_label = np.divide(np.subtract(training_label,50),200)
test_label = np.divide(np.subtract(test_label,50),200)

#reg = linear_model.LinearRegression()
reg =  linear_model.BayesianRidge()
reg.fit(training_data, training_label)
predicted_label = np.add(np.matmul(test_data, reg.coef_), reg.intercept_)


plt.plot(index[:200], test_label[:200])
plt.show()

plt.plot(index[:200], np.transpose(test_data)[0][:200])
plt.show()
print(np.shape(training_label))

plt.plot(index[:200], np.transpose(test_data)[1][:200])
plt.show()
print(np.shape(training_label))


plt.plot(index[:200], predicted_label[:200])
plt.show()
print(np.shape(training_label))

print(reg.coef_, reg.intercept_)


