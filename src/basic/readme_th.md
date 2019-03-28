#### 1. Face recognition and landmarks extraction



#### 2. Landmarks filter and ROI selection



#### 3. Background Subtraction

According to the work by Li et al [^1], the variations of face signal can be divided in to two additive factors:
$$
\bold{g}_{face} = \bold{s} + \bold{y}
$$
where $\bold{s}​$ denotes the green value variations caused by cardiac pulse, and $\bold{y}​$ denotes the green value variations caused by illumination changes.

The background region's green value variations can be used to assume illumination's influence, which is denoted as $\bold{g}_{bg}$. And a linear function is used to estimated the correlation of $\bold{y}$ and $\bold{g}_{bg}​$.
$$
\bold{y}\approx h \bold{g}_{bg}
$$
So the illumination rectified pulse signal is:
$$
\bold{g}_{ir} = \bold{g}_{face}-h\bold{g}_{bg}
$$
To estimated the parameter $h$, they used Normalized Least Mean Square (NLMS) adaptive filter. But here we implement an explicit method to get a proper estimation of $h$. Denote the derivatives of $\bold{g}_{ir}$, $\bold{g}_{face}$, $\bold{g}_{bg}$ as $\bold{D'}$, $\bold{D}$ and $\bold{B}$. We have
$$
\bold{D'} = \bold{D}-h\bold{B}
$$
The arrays above have elements denoted as $\bold{D'}=\{d'_1, d'_2,\cdots,d'_n\}$, $\bold{D}=\{d_1, d_2,\cdots,d_n\}$ and $\bold{B}=\{b_1, b_2,\cdots,b_n\}$. Our goal is to get $var(\bold{D'})$ to be minimized.
$$
\begin{align*}
var(\bold{D'})
	&=\frac{1}{n}\sum_{i=1}^{n}{(d'_i-\bar{d'})^2}\\
	&=\frac{1}{n}\sum_{i=1}^{n}{\left[(d_i-hb_i)-(\bar{d}-h\bar b)\right]^2}\\
	&=\frac{1}{n}\sum_{i=1}^{n}(d_i-\bar d)^2+h^2\frac{1}{n}\sum_{i=1}^{n}{(b_i-\bar b)^2}-2h\frac{1}{n}\sum_{i=1}^{n}(d_i-\bar d)(b_i-\bar b) \\
	&=var(\bold{D})+h^2 var(\bold{B})-2h\frac{1}{n}\sum_{i=1}^{n}(d_i-\bar d)(b_i-\bar b)
\end{align*}
$$
Obviously, to get $var(\bold{D'})$ minimized, $h$ is constrained to be:
$$
h=\frac{\sum_{i=1}^n (d_i-\bar d)(b_i-\bar b)}{\sum_{i=1}^n(b_i-\bar b)^2}
$$
After implementing this strategy, the results (purple line) is now better than original green value (blue line).

![Figure_1](/Users/suxy/Workspace/ppgdetector/src/basic/Figure_1.png)



[^1]: Li, X. , Chen, J. , Zhao, G. , & Matti Pietikäinen. (2014). Remote Heart Rate Measurement from Face Videos under Realistic Situations. *Computer Vision & Pattern Recognition*. IEEE.