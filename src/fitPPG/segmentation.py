from utils import *
from scipy import signal

def cdiff(data):
    """ central differential
    Args:
        data: list or ndarray, data need to be differenced
    Returns:
        ndarray: central differenced data
    """
    if len(data)==1: return np.array([0])
    return np.array([0]+list(np.diff(data)/2)) + np.array(list(np.diff(data)/2)+[0])

def findPeaks(sig):
    """ find valleys or peaks
    Args:
        sig: list or ndarray, data has peaks to be found
    Returns:
        list: list of locations having peaks in sig
    """
    d = cdiff(sig)
    if len(d) < 2: return []
    return [i for i in range(len(sig)-1) if d[i]<=0 and d[i+1]>=0]

def smooth(x, N, k):
    """
    Args:
        x: list or ndarray, data to be smoothed
        N: window size of smooth
        k: smooth times
    Returns:
        ndarray: data smoothed
    """
    for i in range(k):
        x = np.convolve(x, np.ones((N,))/N)[(N-1):]
    return x

def expandRange(p, mi, ma, rp):
    """ return range [p-rp, p+rp] and guarantee it's in [mi, ma]
    Args:
        p:  integer, center point of range
        rp: integer, half width of range
        mi: integer, minimum of return range
        ma: integer, maximum of return range
    Returns:
        ndarray: intergers range of [p-rp, p+rp]
    """
    return np.arange(max(mi, p-rp), min(ma, p+rp))

class Filter:
    def __init__(self,fs,order=51, bands=[0, 0.1, 0.7, 4, 5, 12.5],
                                   desired = [0, 0, 1, 1, 0, 0]):
        self.fir = signal.firls(order, bands, desired, fs=fs)

    def __call__(self,data=None,noise=None):
        gFlt = signal.filtfilt(self.fir, 1, data)
        return gFlt

# main
if __name__=="__main__":
    # load data
    # data_range = np.arange(250,450)
    data_range = np.arange(1,1500)
    time,data,noise = np.array(list(zip(*readOutput("output_detect.txt")[data_range,:])))
    g = np.mean(data) - data
    n = np.mean(noise) - noise
    fs = 1 / (time[1] - time[0])
    ran = int(fs*0.3)

    # filter
    sigFilter = Filter(fs)
    gFlt = sigFilter(data=g,noise=n)
    gSmo = smooth(gFlt, 3, 3)
    pSmo = findPeaks(gSmo)
    pFlt = deepcopy(pSmo)
    for i,psmo in enumerate(pSmo):
        sRan = expandRange(psmo,0,len(gSmo),ran)
        pFlt[i] = np.argsort(gFlt[sRan])[0]+sRan[0]

    # results
    fig, ax = plt.subplots(1)
    ax.plot(time, g)
    ax.plot(time, gFlt)
    ax.plot(time, gSmo)
    ax.plot(time[pSmo], gSmo[pSmo],'ro')
    ax.plot(time[pFlt], gFlt[pFlt],'ko')
    ax.legend([r'$g_{Ori}$', r'$g_{Flt}$', r'$g_{Smo}$'])
    plt.show()