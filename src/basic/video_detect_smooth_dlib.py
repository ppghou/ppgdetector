import cv2
import dlib
import time
import math
import numpy as np
import matplotlib.pyplot as plt

# http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
predictorPath = r"shape_predictor_68_face_landmarks.dat"
videoPath = r"E:\Undergraduate\10_大四秋\软件工程 董渊\软件工程课大作业\数据\20181101\PIC_0401.MP4"
file = open(r'output_detect.txt', 'w')
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
    """ Choss ROI based on points and ratio
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


def clip(img, size, rect):
    """ Clip the frame and return the face region
    Args:
        img: an instance of numpy.ndarray, the image
        size: size of the image when performing detection
        rect: an instance of dlib.rectangle, the face region
    Returns:
        numpy.ndarray, the face region
    """
    left = int(rect.left() / size[0] * img.shape[1])
    right = int(rect.right() / size[0] * img.shape[1])
    top = int(rect.top() / size[1] * img.shape[0])
    bottom = int(rect.bottom() / size[1] * img.shape[0])
    return img[top:bottom, left:right]

class Detector:
    """ Detect and calculate ppg signal
    roiRatio: a positive number, the roi gets bigger as it increases
    smoothRatio: a real number between 0 and 1,
         the landmarks get stabler as it increases
    """
    roiRatio = 5
    smoothRatio = 0.9
    
    def __init__(self, detectorPath = None, predictorPath = None):
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
        resized, detectionSize = resize(image, 540)
        
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
        
        # Perform landmark prediction on the face region
        rect = rects[0]
        clipped, size = resize(clip(image, detectionSize, rect), 540)
        shape = self.predictor(clipped,
                   dlib.rectangle(0, 0, size[0], size[1]))
        landmarks = shape_to_np(shape)
        
        # If not the first image, perform window smoothing
        if (self.rect != None):
            if (self.dist(self.rect, rect) < 0.3):
                landmarks = self.smoothRatio * self.landmarks \
                                + (1 - self.smoothRatio) * landmarks
                landmarks = landmarks.astype(int)
        
        # Draw feature points
        for (i, (x, y)) in enumerate(landmarks):
            cv2.putText(clipped, "{}".format(i), (x, y),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 1)
        
        # Left ROI
        bb = np_to_bb([landmarks[1], landmarks[3], landmarks[31]], self.roiRatio)
        cv2.rectangle(clipped, (bb[0], bb[1]), (bb[2], bb[3]), (0, 0, 255), 2)
        val1 = [np.mean(np.mean(clipped[bb[1]:bb[3],bb[0]:bb[2], i])) for i in range(3)]
        
        # Right ROI
        bb = np_to_bb([landmarks[13], landmarks[15], landmarks[35]], self.roiRatio)
        cv2.rectangle(clipped, (bb[0], bb[1]), (bb[2], bb[3]), (0, 0, 255), 2)
        val2 = [np.mean(np.mean(clipped[bb[1]:bb[3],bb[0]:bb[2], i])) for i in range(3)]
        
        val = np.divide(np.add(val1, val2), 2)
        val = val1
        
        cv2.imshow("Face Detct #{}".format(i + 1), clipped)
        
        cv2.imshow("Frame", resized)
        
        self.rect = rect
        self.landmarks = landmarks
        return val
    
    def dist(self, rect1, rect2):
        """ Calculate the distance between two rectangles
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
        

if __name__ == "__main__":
    # Initialization
    detect = Detector(predictorPath = predictorPath)
    times = []
    data = [[], [], []]
    video = cv2.VideoCapture(videoPath)
    fps = video.get(cv2.CAP_PROP_FPS)
    video.set(cv2.CAP_PROP_POS_FRAMES, 30*fps) # jump to certain frame
    
    # Handle frame one by one
    t = 0.0
    ret, frame = video.read()
    while(video.isOpened() and t < 10):
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
