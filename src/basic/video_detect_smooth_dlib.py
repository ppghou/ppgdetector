import sys, time, math
import cv2, dlib
import numpy as np
import matplotlib.pyplot as plt

# http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
# predictorPath = r"../../dep/shape_predictor_68_face_landmarks.dat"
# predictorRef = [[1,3,31],[13,15,35]]
predictorPath = r"../../dep/shape_predictor_5_face_landmarks.dat"
predictorRef = [[0,1,4],[2,3,4]]

videoPath = r"../../data/video/PIC_0401.MP4"
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
    num = shape.num_parts
    coords = np.zeros((num, 2), dtype=dtype)
    for i in range(0, num):
        coords[i] = (shape.part(i).x, shape.part(i).y)
    return coords

def np_to_bb(coords, ratio=5, dtype="int"):
    """ Choose ROI based on points and ratio
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

def meanOfChannels(image, bb):
    return np.mean(np.mean(image[bb[1]:bb[3],bb[0]:bb[2]],0),0)

def dist(p1, p2):
    return np.sqrt((p1.x-p2.x)**2+(p1.y-p2.y)**2)

class Detector:
    """ Detect and calculate ppg signal
    roiRatio: a positive number, the roi gets bigger as it increases
    smoothRatio: a real number between 0 and 1,
         the landmarks get stabler as it increases
    """
    roiRatio = 5
    smoothRatio = 0.9
    detectSize = 400
    clipSize = 540

    def __init__(self, detectorPath = None, predictorPath = None, predictorRef = None):
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
        self.refs = predictorRef
        self.landmarks = []

    def __call__(self, image):
        """ Detect the face region and returns the ROI value
        
        Face detection is the slowest part.
        
        Args:
            image: an instance of numpy.ndarray, the image
        Return:
            val: an array of ROI value in each color channel
        """
        val = [0., 0., 0.]
        # Resize the image to limit the calculation
        imageSize = image.shape
        resized, detectionSize = resize(image, self.detectSize)
        
        # Perform face detection on a grayscale image
        gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
        # No need for upsample, because its effect is the same as resize
        faces = self.detector(gray, upsample_num_times = 0)
        num = len(faces) # there should be one face
        if num == 0:
            print("No face in the frame!")
            return val
        if num >= 2:
            print("More than one face!")
            return val
        
        faceRect = dlib.rectangle(
                    int(faces[0].left()*imageSize[1]/detectionSize[0]),
                    int(faces[0].top()*imageSize[1]/detectionSize[0]),
                    int(faces[0].right()*imageSize[1]/detectionSize[0]),
                    int(faces[0].bottom()*imageSize[1]/detectionSize[0]))

        # Perform landmark prediction on the face region
        shape = self.predictor(image, faceRect)
        landmarks = shape_to_np(shape)
        landmarks = self.update(np.array(landmarks))
        rects = [np_to_bb(landmarks[ref], self.roiRatio) for ref in self.refs]
        vals = [meanOfChannels(image, bb) for bb in rects]
        val = np.mean(vals, 0)
        
        # Show detection results
        if '-s' in sys.argv:
            # Draw sample rectangles
            for bb in rects:
                cv2.rectangle(image, (bb[0], bb[1]), (bb[2], bb[3]), (0, 0, 255), 2)
            # Draw feature points
            for (i, (x, y)) in enumerate(landmarks):
                cv2.putText(image, "{}".format(i), (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 1)
            cv2.imshow("Face Detct #{}".format(i + 1), resize(image, self.detectSize)[0])
        return val
    
    def update(self, landmarks):
        if len(self.landmarks):
            landmarks = self.smoothRatio*self.landmarks+(1-self.smoothRatio)*landmarks
            landmarks = landmarks.astype(int)
        self.landmarks = landmarks
        return landmarks

if __name__ == "__main__":
    # Initialization
    detect = Detector(predictorPath = predictorPath, predictorRef = predictorRef)
    times = []
    data = []
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
        v = detect(frame)

        # show result
        times.append(t)
        data.append(v)
        print("%.2f\t%.3f\t%.3f\t%.3f\t%.1f\t%.1f"%(t, v[0], v[1], v[2], fps, 1/(time.time() - calcTime)) )#, file=file)

        # check stop or quit
        ret, frame = video.read()
        if cv2.waitKey(1) & 0xFF == ord('q') or not ret:
            break
    
    # release memory and destroy windows
    video.release()
    cv2.destroyAllWindows()
    file.close()

    data = np.array(data)
    plt.figure("Original",figsize=(12,4))
    plt.subplot(1,3,1);plt.plot(times, data[:,0]); plt.title("R")
    plt.subplot(1,3,2);plt.plot(times, data[:,1]); plt.title("G")
    plt.subplot(1,3,3);plt.plot(times, data[:,2]); plt.title("B")

    # smoothing
    const = [1., 1., 1.]
    offset = [0, 0, 0]
    data_smooth = np.zeros((len(times),3))
    for i in range(len(times)):
        data_smooth[i,:] = data[i,:]
        if i == 0:
            continue
        for j in range(3):
            dist = data[i,j] - data[i-1,j]
            if abs(dist) < const[j]:
                dist = 0
            offset[j] -= dist
            data_smooth[i,j] += offset[j]
    
    plt.figure("Smoothed",figsize=(12,4))
    plt.subplot(1,3,1);plt.plot(times, data_smooth[:,0]); plt.title("R")
    plt.subplot(1,3,2);plt.plot(times, data_smooth[:,1]); plt.title("G")
    plt.subplot(1,3,3);plt.plot(times, data_smooth[:,2]); plt.title("B")

    plt.show()

    