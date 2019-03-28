import os,sys,time,math
import cv2,dlib
import numpy as np
import matplotlib.pyplot as plt
from utils import *

def rect_to_bb(rect):
    """ Transform a rectangle into a bounding box
    Args:
        rect: an instance of dlib.rectangle
    Returns:
        [x, y, w, h]: coordinates of the upper-left corner
            and the width and height of the box
    """
    x = rect.left()
    y = rect.top()
    w = rect.right() - x
    h = rect.bottom() - y
    return [x, y, w, h]

def shape_to_np(shape, dtype="int"):
    """ Transform the detection results into points
    Args:
        shape: an instance of dlib.full_object_detection
    Returns:
        coords: an array of point coordinates
            columns - x; y
    """
    coords = np.zeros((68, 2), dtype=dtype)
    for i in range(0, 68):
        coords[i] = (shape.part(i).x, shape.part(i).y)
    return coords

def np_to_bb(coords, ratio=5, dtype="int"):
    """ Chooose ROI based on points and ratio
    Args:
        coords: an array of point coordinates
            columns - x; y
        ratio: the ratio of the length of the bounding box in each direction
            to the distance between ROI and the bounding box
        dtype: optional variable, type of the coordinates
    Returns:
        coordinates of the upper-left and bottom-right corner
    """
    x = [xi for (xi,yi) in coords]
    y = [yi for (xi,yi) in coords]
    minx, maxx = min(x), max(x)
    miny, maxy = min(y), max(y)
    p, q = ratio - 1, ratio
    roi = [minx * p / q + maxx / q, miny * p / q + maxy / q,
           maxx * p / q + minx / q, maxy * p / q + miny / q]
    return [int(i) for i in roi]

def resize(image, width=1200):
    """ Resize the image with width
    Args:
        image: an instance of numpy.ndarray, the image
        width: the width of the resized image
    Returns:
        resized: the resized image
        size: size of the resized image
    """
    r = width * 1.0 / image.shape[1]
    size = (width, int(image.shape[0] * r)) 
    resized = cv2.resize(image, size, interpolation=cv2.INTER_AREA)
    return resized, size

def coordTrans(imShape, oriSize, rect):
    """Transform the coordinates into the original image
    Args:
        imShape: shape of the detected image
        oriSize: size of the original image
        rect: an instance of dlib.rectangle, the face region
    Returns:
        the rect in the original image
    """
    left = int(round(rect.left() / oriSize[0] * imShape[1]))
    right = int(round(rect.right() / oriSize[0] * imShape[1]))
    top = int(round(rect.top() / oriSize[1] * imShape[0]))
    bottom = int(round(rect.bottom() / oriSize[1] * imShape[0]))
    return dlib.rectangle(left, top, right, bottom)

class Detector:
    """ Detect and calculate ppg signal
    roiRatio: a positive number, the roi gets bigger as it increases
    smoothRatio: a real number between 0 and 1,
         the landmarks get stabler as it increases
    """
    detectSize = 500
    clipSize = 540
    roiRatio = 6
    rectSmoothRatio = 0.98
    rectDistThres = 4
    markSmoothRatio = 0.95
    markDistThres1 = 0.02
    markDistThres2 = 0.025
    
    def __init__(self, predictorPath = None, predictorIdx = None):
        """ Initialize the instance of Detector
        
        detector: dlib.fhog_object_detector
        predictor: dlib.shape_predictor
        rect: dlib.rectangle, face region in the last frame
        landmarks: numpy.ndarray, coordinates of face landmarks in the last frame
                columns - x; y
        
        Args:
            detectorPath: path of the face detector
            predictorPath: path of the shape predictor
        """
        self.detector = dlib.get_frontal_face_detector()
        self.predictor = dlib.shape_predictor(predictorPath)
        self.idx = predictorIdx
        self.rect = None
        self.landmarks = None

    def __call__(self, image):
        """ Detect the face region and returns the ROI value
        
        Face detection is the slowest part.
        
        Args:
            image: an instance of numpy.ndarray, the image
        Return:
            val: an array of ROI value in each color channel
        """
        val = [0, 0, 0]
        
        # Resize the image to limit the calculation
        resized, detectionSize = resize(image, self.detectSize)
        
        # Perform face detection on a grayscale image
        gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
        # No need for upsample, because its effect is the same as resize
        rects = self.detector(gray, upsample_num_times = 0)
        num = len(rects) # there should be one face
        if num == 0:
            print("No face in the frame!")
            return val
        if num >= 2:
            print("More than one face!")
            return val
        rect = rects[0]
        
        # If not the first image, perform face region smoothing
        if (self.rect!= None):
            dist = self.distForRects(self.rect, rect)
            if (dist < self.rectDistThres):
                smoothRatio = self.rectSmoothRatio * math.sqrt(1 - dist / self.rectDistThres)
                rect = self.smoothRects(self.rect, rect, smoothRatio)
        
        # Perform landmark prediction on the face region
        face = coordTrans(image.shape, detectionSize, rect)
        shape = self.predictor(image, face)
        landmarks = shape_to_np(shape)
        
        # If not the first image, perform landmark smoothing
        if (self.rect != None):
            dist = self.distForMarks(self.rect, rect)
            if (dist < self.markDistThres2):
                tmp = dist - self.markDistThres1
                smoothRatio = self.markSmoothRatio + 0.5 * (np.sign(tmp) + 1) * (math.exp(-1e3 * tmp) - self.markSmoothRatio)
                if dist > self.markDistThres1:
                    smoothRatio = math.exp(1e3 * (self.markDistThres1 - dist))
                landmarks = self.smoothMarks(self.landmarks, landmarks, smoothRatio)
        
        # ROI value
        rois = [np_to_bb(landmarks[idx], self.roiRatio) for idx in self.idx]
        vals = [np.mean(np.mean(image[roi[1]:roi[3], roi[0]:roi[2]],0),0) for roi in rois]
        val = np.mean(vals, 0)
        
        # Show detection results
        if '-m' in sys.argv:
            roi = rois[0]
            Y = np.array(range(roi[1],roi[3]))
            X = np.array(range(roi[0],roi[2]))
            X,Y = np.meshgrid(X,Y)
            Z = ndimage.filters.gaussian_filter(image[roi[1]:roi[3], roi[0]:roi[2],1],sigma=5)
            surf = ax.plot_surface(X, Y, Z,cmap=cm.hsv,vmin=70,vmax=110)
            ax.set_zlim(60,120)
            plt.draw()
            plt.pause(1e-17)
            surf.remove()

        if '-s' in sys.argv:
            # Draw sample rectangles
            for roi in rois:
                cv2.rectangle(image, (roi[0], roi[1]),(roi[2], roi[3]), (0, 0, 255), 3)
            # Draw feature points
            for (i, (x, y)) in enumerate(landmarks):
                cv2.putText(image, "{}".format(i), (x, y),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 1)
            face = dlib.rectangle(max(face.left(), 0), max(face.top(), 0), min(face.right(), image.shape[1]), min(face.bottom(), image.shape[0]))
            cv2.rectangle(image, (face.left(), face.top()), (face.right(),face.bottom()), (255, 0, 0), 3)
            # image = resize(image[face.top():face.bottom(), face.left():face.right()], self.detectSize)[0]
            cv2.imshow("Face", image)
                
        self.rect = rect
        self.landmarks = landmarks
        return val
    
    def initialize(self):
        self.rect = None
        self.landmarks = None
    
    def distForRects(self, rect1, rect2):
        """Calculate the distance between two rectangles for rectangle smoothing
        Arg:
            rect1, rect2: dlib.rectangle
        Return:
            distance between rectangles
        """
        distx = (rect1.left() - rect2.left()) + (rect1.left() - rect2.left())
        disty = (rect1.top() - rect2.top()) + (rect1.bottom() - rect2.bottom())
        return pow(pow(distx / (rect1.right() - rect1.left()), 2) +
                   pow(disty / (rect1.bottom() - rect1.top()), 2), 1/2)
        
    def smoothRects(self, rect1, rect2, smoothRatio):
        left = int(round(smoothRatio * rect1.left() + (1 - smoothRatio) * rect2.left()))
        right = int(round(smoothRatio * rect1.right() + (1 - smoothRatio) * rect2.right()))
        top = int(round(smoothRatio * rect1.top() + (1 - smoothRatio) * rect2.top()))
        bottom = int(round(smoothRatio * rect1.bottom() + (1 - smoothRatio) * rect2.bottom()))
        return dlib.rectangle(left, top, right, bottom)
    
    def distForMarks(self, rect1, rect2):
        """ Calculate the distance between two rectangles for landmark smoothing
        Arg:
            rect1, rect2: dlib.rectangle
        Return:
            distance between rectangles
        """
        distx = abs(rect1.left() - rect2.left()) + abs(rect1.right() - rect2.right())
        disty = abs(rect1.top() - rect2.top()) + abs(rect1.bottom() - rect2.bottom())
        return abs(distx / (rect1.right() - rect1.left())) + abs(disty / (rect1.bottom() - rect1.top()))
    
    def smoothMarks(self, landmarks1, landmarks2, smoothRatio):
        landmarks = smoothRatio * landmarks1 + (1 - smoothRatio) * landmarks2
        landmarks = np.array([[round(pair[0]), round(pair[1])] for pair in landmarks])
        return landmarks.astype(int)

if __name__ == "__main__":
    # = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    # # settings

    # http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
    predictorPath = r"../../dep/shape_predictor_68_face_landmarks.dat"
    predictorIdx = [[3, 31, 40], [13, 35, 47]]
    # videopath = r"../../data/video/ljn_ce_0047.mp4"
    videopath = r"../../data/video/ljn_t1_1126.mp4"
    saveFile = open(r'output_detect.txt', 'w')
    bx,by,bh,bw = 3200,180,256,256 # background rect start point and height and width
    br = [bx,bx+bw,by,by+bh]    
    startTime = None

    # = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    # # initialize
    
    detect = Detector(predictorPath = predictorPath, predictorIdx = predictorIdx)
    detect.initialize()
    
    times = []
    data = [[], [], []]
    bg = []
    H,h = [],0.5

    if videopath == None:
        video = cv2.VideoCapture(0)
    else:
        video = cv2.VideoCapture(videopath)
    fps = video.get(cv2.CAP_PROP_FPS)
    if startTime != None:
        video.set(cv2.CAP_PROP_POS_FRAMES, startTime * fps)

    # = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    # # preparation

    t = 0.0
    ret, frame = video.read()
    bm = np.mean(frame[br[2]:br[3],br[0]:br[1],1])+5
    if ret:
        print("Video Captured")
    else:
        print("Video Failed")
        sys.exit(0)

    if '-s' in sys.argv:
        cv2.namedWindow("Face", 0)
        cv2.namedWindow("bg", 0)
        cv2.namedWindow("bgSub", 0)
        cv2.resizeWindow("Face", 1280, 720)
        cv2.resizeWindow("bg", bw, bh)
        cv2.resizeWindow("bgSub", bw, bh)
        cv2.moveWindow("Face",0,0)
        cv2.moveWindow("bg",0,0)
        cv2.moveWindow("bgSub",bw,0)

    if '-p' in sys.argv:
        plt.ion()
        fig = plt.figure()
        plt.title("Background Subtraction")
        plt.xlabel("Time / s")
        ax = plt.subplot()

    if '-m' in sys.argv:
        from scipy import ndimage
        from mpl_toolkits.mplot3d import Axes3D
        from matplotlib import cm
        from matplotlib.ticker import LinearLocator, FormatStrFormatter
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')        

    # = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    # # Handle frame one by one

    while(video.isOpened()):
        t += 1.0/fps
        calcTime = time.time()
        if '-s' in sys.argv:
            cv2.rectangle(frame, (br[0],br[2]), (br[1],br[3]), (0, 255, 0), 3)

        # detect
        value = detect(frame)
        
        bg_rect = frame[br[2]:br[3],br[0]:br[1],1]
        bg_mean = np.mean(bg_rect)

        times.append(t)
        for i in range(3):
            data[i].append(value[i]-107)
        bg.append(bg_mean-bm)

        # show result
        if '-s' in sys.argv:
            cv2.imshow('bg',bg_rect)
            cv2.imshow('bgSub',bg_rect-bg_mean)

        if '-p' in sys.argv:
            h_arr = [0.3, 0.9]
            D,B,T = np.array(data[1]),np.array(bg),np.array(times)
            beg = len(T)-100 if len(T)>100 else 0
            Y,legend_str = [D,B],[r'$g_{face}$',r'$g_{bg}$']
            hop = optimizeStd(np.diff(D[beg:]),np.diff(B[beg:])) if len(D)>1 else 0.5
            H.append(hop)

            for h in h_arr:
                Y.append(D-h*B+h)
                legend_str.append(r'$h=$'+'%.2f'%h)
            Y.append(D-hop*B+hop)
            legend_str.append(r'$h=h_{opt}$')

            for i,y in enumerate(Y):
                ax.plot(T[beg:],y[beg:],'C%d'%i,lw=(i==len(Y)-1)+1)
                legend_str[i] += ' %.4f'%np.std(np.diff(y[beg:]))
            legend_str.append(r'$h_{opt}=$'+'%.2f'%hop)
            
            plt.plot(T,H,'C9')
            plt.xlim(T[beg],T[-1])
            plt.legend(legend_str,frameon=False,loc='upper right')
            plt.draw()
            plt.pause(1e-17)
            for y in Y:
                ax.lines.pop(0)
            ax.lines.pop(0)

        print("%.2f %.3f %.3f %.3f %.3f %.1f %.1f"%(t, value[0], value[1], value[2], bg_mean,
                        fps, 1/(time.time() - calcTime)), file = saveFile)

        # check stop or quit
        ret, frame = video.read()
        if cv2.waitKey(1) & 0xFF == ord('q') or not ret:
            break
    
    # = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    # # analyze

    data = np.array(data)
    for i in range(3):
        plt.figure()
        plt.plot(times, data[i])
        plt.show()
    
    dataCombined = 0.00774539 * data[1] + 0.003436163 * data[2]
    dataCombined = 2 * np.mean(dataCombined) - dataCombined
    plt.figure();
    plt.plot(times, dataCombined)
    plt.show()
    
    # # smoothing
    # const = [3, 3, 3]
    # offset = [[0,], [0,], [0,]]
    # data_smooth = np.zeros((3, len(times)))
    # for i in range(len(times)):
    #     for j in range(3):
    #         data_smooth[j][i] = data[j][i] 
    #         if i == 0:
    #             continue
    #         dist = data[j][i] - data[j][i-1]
    #         if abs(dist) < const[j]:
    #             dist = 0
    #         offset[j].append(offset[j][-1] - dist)
    #         data_smooth[j][i] += offset[j][i]
    # for i in range(3):
    #     plt.figure()
    #     plt.plot(times, data_smooth[i])
    #     plt.show()

    # = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
    # # release memory and destroy windows

    saveFile.close()
    video.release()
    cv2.destroyAllWindows()
    plt.close('all')

    
