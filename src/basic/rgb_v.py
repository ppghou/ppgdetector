import cv2, dlib
import sys, time
import numpy as np
import matplotlib.pyplot as plt

# http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor("../../dep/shape_predictor_68_face_landmarks.dat")

# rectangle to bounding box
def rect_to_bb(rect):
    x = rect.left()
    y = rect.top()
    w = rect.right() - x 
    h = rect.bottom() - y 
    return (x, y, w, h)

# shape to n points
def shape_to_np(shape, n, dtype="int"):
    coords = np.zeros((n, 2), dtype=dtype)
    for i in range(0, n):
        coords[i] = (shape.part(i).x, shape.part(i).y)
    return coords

# n points to bounding box
def np_to_bb(shape, dtype="int"):
    # minx, miny, maxx, maxy = 65535,65535,0,0
    # for (x,y) in shape:
    #     if x < minx:
    #         minx = x
    #     if y < miny:
    #         miny = y
    #     if x > maxx:
    #         maxx = x
    #     if y > maxy:
    #         maxy = y
    x = [xi for (xi,yi) in shape]
    y = [yi for (xi,yi) in shape]
    minx, maxx = min(x), max(x)
    miny, maxy = min(y), max(y)
    p, q = 3, 4
    return (minx*p/q+maxx/q, miny*p/q+maxy/q, maxx*p/q+minx/q, maxy*p/q+miny/q)

# resize a image with width
def resize(image, width=1200):
    r = width * 1.0 / image.shape[1]
    dim = (width, int(image.shape[0] * r)) 
    resized = cv2.resize(image, dim, interpolation=cv2.INTER_AREA)
    return resized

# detect face and roi value
def detect(image):
    val = 0
    image = resize(image, 540)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    rects = detector(gray, 1)

    # for all faces
    for (i, rect) in enumerate(rects):
        shape = predictor(gray, rect)
        shape = shape_to_np(shape, 68)

        # get face area
        (x, y, w, h) = rect_to_bb(rect)
        cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

        cv2.putText(image, "Face #{}".format(i + 1), (x - 10, y - 10),
            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

        # left ROI
        bb = np_to_bb([shape[2], shape[4], shape[31]])
        cv2.rectangle(image, (bb[0], bb[1]), (bb[2], bb[3]), (0, 0, 255), 2)
        val1 = np.mean(np.mean(gray[bb[1]:bb[3],bb[0]:bb[2]]))

        # right ROI
        bb = np_to_bb([shape[12], shape[14], shape[35]])
        cv2.rectangle(image, (bb[0], bb[1]), (bb[2], bb[3]), (0, 0, 255), 2)
        val2 = np.mean(np.mean(gray[bb[1]:bb[3],bb[0]:bb[2]]))
        
        val = (val1+val2)/2

    cv2.imshow("Face Detect", image)
    return val

if __name__ == "__main__":
    # preparation
    data = []
    video = cv2.VideoCapture("../../data/video/in.mov")
    fps = video.get(cv2.CAP_PROP_FPS)

    # handle frame one by one
    t = 0.0
    ret,frame = video.read()
    while(video.isOpened()):
        t += 1.0/fps

        # detect
        value = detect(frame)

        # show result
        data.append([t, value])
        print("%f, %f"%(t, value))

        # check stop or quit
        ret,frame = video.read()
        if cv2.waitKey(1) & 0xFF == ord('q') or not ret:
            break

    import matplotlib.pyplot as plt
    data = np.array(data)
    plt.plot(data[:,0], data[:,1])
    plt.show()

    # release memory and destroy windows
    video.release()
    cv2.destroyAllWindows()
