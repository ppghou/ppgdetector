import os,sys,time,math
import numpy as np
import matplotlib.pyplot as plt
from copy import deepcopy

def optimizeStd(data, noise):
    """ Optimize data by calculating h that getting min std of (data-h*noise)
    Args:
        data: ndarray
            data to be optimized
        noise: ndarray
            noise from background
    Returns:
        float: optimized h
    """
    d_i = data -np.mean(data)
    b_i = noise-np.mean(noise)
    return np.sum(d_i*b_i)/np.sum(b_i*b_i)

def loadStandardSignal(signalFile,musigmaFile):
    """ load standard signal from files
    Args:
        signalFile: str
            file path of signal data
        musigmaFile: str
            file path of mu and sigma data
    Returns:
        curv : ndarray
            S-by-2 matrix, points coordinates
        dist : ndarray
            k-by-5 matrix, idx, mu[x] sigma[x] mu[y] and sigma[y] of k feature points
    """
    with open(signalFile) as f:
        signal = [float(val) for val in f.readlines()]
        size,maxs = len(signal),max(signal)
        curv = np.zeros([size,2])
        curv[:,0] = [x/(size-1) for x in range(size)]
        curv[:,1] = [y/maxs for y in signal]
    with open(musigmaFile) as f:
        dist = []
        for val in f.readlines():
            V = np.array([float(v) for v in val.split()])
            V[1:3] /= size
            V[3:5] /= maxs
            dist.append(V)
        dist = np.array(dist)
    return curv, dist

def curvInterp(curv,p1,p2,size):
    """ 
    Args:
        curv: 2D ndarray
            N-by-2 matrix, N points
        p1,p2: list or ndarray
            length = 2, p1 left point, p2 right point
        size: int
            the size of new curv (number of points)
    """
    if curv[0,0]>curv[-1,0]:
        print("\033[1;35mError occured. Feature points x coordinates will be exchanged\033[0m")
        curv[0,0],curv[-1,0] = curv[-1,0],curv[0,0]
    new_x = np.linspace(curv[0,0], curv[-1,0], size)
    new_y = np.interp(new_x, curv[:,0], curv[:,1])
    new_x = (new_x-new_x[0])/(new_x[-1]-new_x[0])*(p2[0]-p1[0])+p1[0]
    new_y = (new_y-new_y[0])/(new_y[-1]-new_y[0])*(p2[1]-p1[1])+p1[1]
    return np.array(list(zip(new_x,new_y)))

def readOutput(fileName):
    """
    Args:
        fileName: str
            file path of output detect data
    Returns:
        data: ndarray
            N-by-2 matrix, time and signal
    """
    with open(fileName) as f:
        return np.array([[float(l) for l in line.split()] for line in f.readlines()])[:,[0,2,4]]

class Sampler:
    def __init__(self,curv,dist):
        """
        Args:
            curv : ndarray
                S-by-2 matrix, points coordinates
            dist : ndarray
                k-by-5 matrix, idx, mu[x] sigma[x] mu[y] and sigma[y] of k feature points
        """
        self.curv = curv
        self.dist = dist

    def __call__(self,N=1):
        '''
        Args:
            N : integer
                sample size, how many curves you want to generate
        Reutrns:
            curves: ndarray
                N-by-S-by-2 matrix, N curves each has S points
        '''
        v_size, f_size = len(self.curv), len(self.dist)+2
        curves = np.zeros([N, v_size, 2])
        oldFeat = np.zeros([f_size, 2])
        newFeats = np.zeros([N, f_size, 2])

        oldFeat[ 0,:] = self.curv[ 0,:]
        oldFeat[-1,:] = self.curv[-1,:]
        newFeats[:, 0,:] = [self.curv[ 0] for i in range(N)]
        newFeats[:,-1,:] = [self.curv[-1] for i in range(N)]

        # original index segments [0, ip1, ip2, ip3, end]
        ods = [0]+[int(i) for i in self.dist[:,0]]+[v_size]
        # original index range: [0:ip1, ip1:ip2, ip2:ip3, ip3:end]
        odr = [np.arange(ods[i],ods[i+1]) for i in range(len(ods)-1)]

        # get new features
        for i,[idx,mu_x,sigma_x,mu_y,sigma_y] in enumerate(self.dist):
            oldFeat[i+1] = self.curv[int(idx)]
            newFeats[:,i+1,0] = np.random.normal(loc=mu_x, scale=sigma_x/5, size=N)
            newFeats[:,i+1,1] = np.random.normal(loc=mu_y, scale=sigma_y/5, size=N)

        for i in range(N):
            newFeat = newFeats[i,:,:]
            nds = [int(f[0]*v_size) for f in newFeat]
            ndr = [np.arange(nds[i],nds[i+1]) for i in range(len(nds)-1)]
            for j,r in enumerate(odr):
                cur = curvInterp(self.curv[r,:],newFeat[j],newFeat[j+1],nds[j+1]-nds[j])
                curves[i,ndr[j],:] = cur
        return curves

def fitCurv(t,d,x,y):
    """ Fit a curv [x,y] to connect head and tail with [t,d]
    Args:
        t: ndarray
            time axis of data to be imitated
        d: ndarray
            value axis of data to be imitated
        x: ndarray
            standard curve x
        y: ndarray
            standard curve y
    Returns:
        x: ndarray
            x after imitation
        y: ndarray
            y after imitation
    """
    x = (x-x[0])/(x[-1]-x[0])*(t[-1]-t[0])+t[0]
    dt= (t-t[0])/(t[-1]-t[0])*(d[-1]-d[0])+d[0] # data triangle
    yt= (x-x[0])/(x[-1]-x[0])*(d[-1]-d[0])+d[0] # y triangle
    y = y * max(d-dt) + yt
    return x,y

# main
if __name__=="__main__":
    # load standard signal and samples
    signalFile = "standardSig.txt"
    musigmaFile = "mu&sigma.txt"
    curv, dist = loadStandardSignal(signalFile,musigmaFile)
    sampler = Sampler(curv,dist)
    curves = sampler(32)
    
    # load one piece of noised signal
    #cut = np.arange(12,29)
    cds = [12,29,44,59]
    cdr = [np.arange(cds[i]-cds[0]-(i!=0),cds[i+1]-cds[0]) for i in range(len(cds)-1)]
    time,data,noise = np.array(list(zip(*readOutput("output_detect.txt")[cds[0]:cds[-1],:])))
    data = 255 - data + 0.5 * noise
    plt.plot(time,data)

    for r in cdr:
        t,d = time[r],data[r]
        for c in curves:
            x,y = fitCurv(t,d,c[:,0],c[:,1])
            plt.plot(x,y)
    plt.show()
