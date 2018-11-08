import cv2
import sys, time
import numpy as np

if __name__ == "__main__":
    values = []
    timess = []

    # open videoCapture and videoWriter
    video = cv2.VideoCapture("../../data/video/in.mov")
    fps = video.get(cv2.CAP_PROP_FPS)
    # box = [47, 56, 619, 343] # right small
    # box = [63, 87, 607, 325] # right big
    box = [38, 57, 439, 346] # left small
    # box = [49, 106, 431, 312] # left big
    # box = [57, 45, 509, 190] # middle

    # handle frame one by one
    ret,frame = video.read()
    t = 0.0
    while(video.isOpened()):
        t += 1.0/fps
        image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        value = np.mean(np.mean(image[box[2]:box[0]+box[2],box[3]:box[1]+box[3]]))
        values.append(value)
        timess.append(t)

        # check stop or quit
        ret,frame = video.read()
        if cv2.waitKey(1) & 0xFF == ord('q') or not ret:
            break
        print("%f, %f"%(t, value))

    import matplotlib.pyplot as plt
    plt.plot(timess,values)
    plt.show()

    # release memory and destroy windows
    video.release()
    cv2.destroyAllWindows()