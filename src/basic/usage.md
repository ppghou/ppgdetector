+ Requirements:
  + `opencv`
  + `python-dlib`
  + `matplotlib`

+ Usage:

  + `rgb_simple.py` : use simple way (fixed ROI) to detect and calculate

    ```shell
    python rgb_simple.py
    ```

    ![rgb_simple](/Users/suxy/学习/7-大四上/软件工程/ppgdetector/data/figure/basic/rgb_simple.png)

  + rgb_c.py` : open camera and sample (about 10 frame rate)

    ```shell
    python rgb_c.py
    python rgb_c.py -as    # auto scale plot
    ```

    ![rgb_c](/Users/suxy/学习/7-大四上/软件工程/ppgdetector/data/figure/basic/rgb_c.png)

  + `rgb_v.py` : open a video and calculate, plot in the end

    ```shell
    python rgb_v.py
    ```
    
    ![rgb_v](/Users/suxy/学习/7-大四上/软件工程/ppgdetector/data/figure/basic/rgb_v.png)