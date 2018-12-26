import cv2
import dlib
import time
import math
import numpy as np
import matplotlib.pyplot as plt
import sys

# http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
predictorPath = r"shape_predictor_68_face_landmarks.dat"
predictorIdx = [[1, 3, 31], [13, 15, 35]]
#videoPath = r"E:\Undergraduate\10_大四秋\软件工程 董渊\软件工程课大作业\数据\20181109_近距离_头部不固定\视频数据\PIC_0427.MP4"
videoPath = r"E:\Undergraduate\10_大四秋\软件工程 董渊\软件工程课大作业\数据\20181224\MVI_0005.MP4"
file = open(r'output_detect.txt', 'w')
startTime = 12 # Start the analysis from startTime
cv2.destroyAllWindows()
plt.close('all')

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
    left = int(rect.left() / oriSize[0] * imShape[1])
    right = int(rect.right() / oriSize[0] * imShape[1])
    top = int(rect.top() / oriSize[1] * imShape[0])
    bottom = int(rect.bottom() / oriSize[1] * imShape[0])
    return dlib.rectangle(left, top, right, bottom)
    
class Detector:
    """ Detect and calculate ppg signal
    roiRatio: a positive number, the roi gets bigger as it increases
    smoothRatio: a real number between 0 and 1,
         the landmarks get stabler as it increases
    """
    detectSize = 500
    clipSize = 540
    roiRatio = 5
    rectSmoothRatio = 0.98
    rectDistThres = 4
    markSmoothRatio = 0.95
    markDistThres = 0.2
    
    def __init__(self, detectorPath = None, predictorPath = None, predictorIdx = None):
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
            smoothRatio = self.rectSmoothRatio *  \
                        math.sqrt(1 - dist / self.rectDistThres)
            print("%.3f"%(dist), end="", file = file)
            print("%.3f %.3f"%(dist, smoothRatio))
            print(self.rect, rect)
            if (dist < self.rectDistThres):
                rect = self.smoothRects(self.rect, rect, smoothRatio)
        print(rect)
        print("\t", end="", file = file)
        
        # Perform landmark prediction on the face region
        face = coordTrans(image.shape, detectionSize, rect)
        shape = self.predictor(image, face)
        landmarks = shape_to_np(shape)
        
        # If not the first image, perform landmark smoothing
        if (self.rect != None):
            dist = self.distForMarks(self.rect, rect)
            print("%.3f"%(dist), end="", file = file)
            if (dist < self.markDistThres):
                landmarks = self.smoothMarks(self.landmarks,
                                             landmarks, self.markSmoothRatio)
        print("\t", end="", file = file)
        
        # ROI value
        rois = [np_to_bb(landmarks[idx], self.roiRatio) for idx in self.idx]
        vals = [np.mean(np.mean(image[roi[1]:roi[3], roi[0]:roi[2]], 0), 0) for roi in rois]
        val = np.mean(vals, 0)
        
        # Show detection results
        if '-s' in sys.argv:
            # Draw sample rectangles
            for roi in rois:
                cv2.rectangle(image, (roi[0], roi[1]), (roi[2], roi[3]), (0, 0, 255), 2)
            # Draw feature points
            for (i, (x, y)) in enumerate(landmarks):
                cv2.putText(image, "{}".format(i), (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 0, 0), 1)
            cv2.imshow("Face", resize(image[face.top():face.bottom(), 
                                        face.left():face.right()], self.detectSize)[0])
                
        self.rect = rect
        self.landmarks = landmarks
        return val
    
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
        left = round(smoothRatio * rect1.left() + \
                    (1 - smoothRatio) * rect2.left())
        right = round(smoothRatio * rect1.right() + \
                    (1 - smoothRatio) * rect2.right())
        top = round(smoothRatio * rect1.top() + \
                    (1 - smoothRatio) * rect2.top())
        bottom = round(smoothRatio * rect1.bottom() + \
                    (1 - smoothRatio) * rect2.bottom())
        return dlib.rectangle(left, top, right, bottom)
    
    def distForMarks(self, rect1, rect2):
        """ Calculate the distance between two rectangles for landmark smoothing
        Arg:
            rect1, rect2: dlib.rectangle
        Return:
            distance between rectangles
        """
        distx = abs(rect1.left() - rect2.left()) \
                    + abs(rect1.right() - rect2.right())
        disty = abs(rect1.top() - rect2.top()) \
                    + abs(rect1.bottom() - rect2.bottom())
        return abs(distx / (rect1.right() - rect1.left())) \
                    + abs(disty / (rect1.bottom() - rect1.top()))
    
    def smoothMarks(self, landmarks1, landmarks2, smoothRatio):
        landmarks = smoothRatio * landmarks1 \
                        + (1 - smoothRatio) * landmarks2
        landmarks = np.array([[round(pair[0]), round(pair[1])] 
                        for pair in landmarks])
        landmarks = landmarks.astype(int)
        return landmarks
        

if __name__ == "__main__":
    # Initialization
    detect = Detector(predictorPath = predictorPath, predictorIdx = predictorIdx)
    times = []
    data = [[], [], []]
    video = cv2.VideoCapture(videoPath)
    fps = video.get(cv2.CAP_PROP_FPS)
    video.set(cv2.CAP_PROP_POS_FRAMES, startTime * fps) 
    
    # Handle frame one by one
    t = 0.0
    ret, frame = video.read()
    while(video.isOpened()):
        t += 1.0/fps
        calcTime = time.time()
        
        # detect
        value = detect(frame)

        # show result
        times.append(t)
        for i in range(3):
            data[i].append(value[i])
        print("%.2f\t%.3f\t%.3f\t%.3f\t%.1f\t%.1f"%(t, value[0], value[1],
                        value[2], fps, 1/(time.time() - calcTime)), file = file)

        # check stop or quit
        ret, frame = video.read()
        if cv2.waitKey(1) & 0xFF == ord('q') or not ret:
            break

    # release memory and destroy windows
    video.release()
    cv2.destroyAllWindows()
    file.close()

    data = np.array(data)
    for i in range(3):
        plt.figure()
        plt.plot(times, data[i])
        plt.show()
    
    # smoothing
    const = [3, 3, 3]
    offset = [[0,], [0,], [0,]]
    data_smooth = np.zeros((3, len(times)))
    for i in range(len(times)):
        for j in range(3):
            data_smooth[j][i] = data[j][i] 
            if i == 0:
                continue
            dist = data[j][i] - data[j][i-1]
            if abs(dist) < const[j]:
                dist = 0
            offset[j].append(offset[j][-1] - dist)
            data_smooth[j][i] += offset[j][i]
    for i in range(3):
        plt.figure()
        plt.plot(times, data_smooth[i])
        plt.show()
