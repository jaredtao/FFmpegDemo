## FFMEPG DEMO

学习FFMPEG的时候写的一些示例

## 目录结构

|路径|用途|说明|
|--|--|--|
|3rdparty| 第三方库|主要是FFmpeg，当前版本为4.2.2，包含头文件和lib库|
|bin| 运行路径|分别包含win32、win64、macos对应的ffmpeg 动态库|
|demos|示例程序|统一使用FFmpegDemoConfig.pri做配置|
|res|资源文件|包含yuv、mp4格式的视频文件，以及调用ffplay进行播放的一些脚本|


## 示例

* **YUVToH264**

    yuv转码h264
  
    不带参数时，默认将res/flower_cif.yuv 转码为res/flower_cif.h264

* **YUVFromH264**

    h264解码为yuv
  
    不带参数时，默认将res/flower_cif.h264 解码回到res/flower_cif_fromh264.yuv
    
    如果不存在flower_cif.h264文件，先运行一下YUVToH264示例
    
    可双击运行res/ffplay-flower.bat及res/ffplay-flower_fromh264.bat脚本进行对比

* **RGBToH264**
    
    RGB转换为h264

* **Capture**

  读取本地视频文件并播放 （这个可能不好用，直接看最后一个Player）

* **Convert**

  格式转换

* **Muxing**

  音频、视频混合

* **Remuxing**

  音频、视频分离

* **Player**

  一个综合的例子
  拉流rtmp, 解码到yuv420p/yuv444p, OpenGL 转码rgb并显示

  其中rtmp服务器地址可以换成本地视频文件路径.

#### License
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/wentaojia2014/TaoJson/blob/master/LICENSE)

## About YUV

### Planar formats
Planar (or sometimes "triplanar") formats use separate matrices for each of the 3 color components. In other words, there is one table of luminance pixel values, and two separate tables for the chrominance components. This segregated representation in memory of pixels is more convenient for video coding.

#### YUV 4:2:0 (I420/J420/YV12)
##### I420
It has the luma "luminance" plane Y first, then the U chroma plane and last the V chroma plane.

The two chroma planes (blue and red projections) are sub-sampled in both the horizontal and vertical dimensions by a factor of 2. That is to say, for a 2×2 square of pixels, there are 4 Y samples but only 1 U sample and 1 V sample.

This format requires 4×8+8+8=48 bits per 4 pixels, so its depth is 12 bits per pixel.

I420 is by far the most common format in VLC. Most video decoders output raw pictures in I420 format.

A graphical illustration: Each letter represents one bit.

For a single I420 pixel: YYYYYYYY UU VV
For a 50-pixel I420 frame: YYYYYYYY×50 UU×50 VV×50 (or Y×8×50 U×2×50 V×2×50 for short)
For an n-pixel I420 frame: Y×8×n U×2×n V×2×n

##### J420

J420 is exactly like I420, but with a full range ("digital", 0-255) luma (Y) component instead of limited range ("analog", 16-240). The chroma planes are exactly the same as in I420.

##### YV12
YV12 is exactly like I420, but the order of the U and V planes is reversed. In the name, "YV" refers to the plane order: Y, then V (then U). "12" refers to the pixel depth: 12-bits per pixel as for I420.

#### YUV 4:2:2 (I422/J422)
##### I422
Like I420, I422 has one luma plane Y and 2 chroma planes U, V.

However, in I422, chroma planes (blue and red projections) are sub-sampled only in the horizontal dimension, still by a factor of 2. Thus, there is the same amount of lines in chroma planes as in the luma plane.

For a 2×2 group of pixels, there are 4 Y samples and 2 U and 2 V samples each. The depth is 4×8+2×8+2×8=64 bits per 4 pixels, so 16 bits per pixel.

Some video codecs support I422 (optionally) for higher quality video encoding.

##### J422
J422 is exactly like I422, but with a full range ("digital", 0-255) luma (Y) component instead of limited range ("analog", 16-240). The chroma planes are exactly the same as in I422.

#### YUV 4:4:4
##### I444

As I420 and I422, I444 has one luma plane Y and 2 chroma planes U and V. Chroma planes are not sub-sampled: there are 3 octet values for each pixels. The depth is thus 24 bits per pixel (as with RGB24 or RGB32).

A few video codecs support I444 optionally.

### Packed formats
In packed formats, you usually have only one plane, with all the luma and chroma data interleaved. This is similar to RGB pixel formats, only using a different color space.

Packed formats are very popular inside webcams. In hardware, using separate planes is inefficient: several memory accesses are needed for each pixel. Packed formats are easier and thus cheaper to use.

On the other hand, packet formats cannot normally deal with vertical sub-sampling. Otherwise scan lines would have different sizes. So generally, packed formats are horizontally subsampled, especially by a factor of 2 (i.e., YUV 4:2:2).

#### UYVY
Known as UYVY, Y422 or UYNV.

In UYVY, the chroma samples are sub-sampled by a factor of 2.

In UYVY, the succession for 2 pixels, starts by U then one luma if the first pixel, then V and then another luma for the second pixel.

#### YUY2
Known as YUY2, YUYV, V422 or YUNV.

In YUY2, the chroma samples are sub-sampled by a factor of 2.

In YUY2, the succession for 2 pixels, starts by one luma for the first pixel, then U, then another luma for the second pixel and then V again.

Conversion between YUY2 and UYVY is achieved by swapping bytes pairwise.

#### YVYU
YVYU is just like YUY2, but with U and V order reversed.

#### VYUV
TBD.

### Semi-planar
Semi planar formats have two planes instead of three, one plane for luminance, and one plane for both chrominance components. They are also sometimes referred to as biplanar formats also.

#### NV12/NV21

##### NV12
Related to I420, NV12 has one luma "luminance" plane Y and one plane with U and V values interleaved.

In NV12, chroma planes (blue and red) are subsampled in both the horizontal and vertical dimensions by a factor of 2.

For a 2×2 group of pixels, you have 4 Y samples and 1 U and 1 V sample.

It can be helpful to think of NV12 as I420 with the U and V planes interleaved.

Here is a graphical representation of NV12. Each letter represents one bit:

For 1 NV12 pixel: YYYYYYYY UVUV
For a 2-pixel NV12 frame: YYYYYYYYYYYYYYYY UVUVUVUV
For a 50-pixel NV12 frame: Y×8×50 (UV)×2×50
For a n-pixel NV12 frame: Y×8×n (UV)×2×n

##### NV21
NV21 is like NV12, but with U and V order reversed: it starts with V.

##### Other NV formats
Following the same pattern as NV12/NV21, there are NV16/NV61 (4:2:2 sub-sampling) and NV24/NV42 (4:4:4 sampling) formats. They are mostly used in some funky cheap camera circuitry and not supported in VLC (as of VLC version 2.0).


#### 联系方式:
***
|作者|涛哥|
|---|---|
|QQ|759378563|
|微信|xsd2410421|
|邮箱|jared2020@163.com|
|blog|https://wentaojia2014.github.io/|

###### 请放心联系我，乐于提供相关帮助
***
#### **打赏**
<img src="https://github.com/wentaojia2014/wentaojia2014.github.io/blob/master/img/weixin.jpg?raw=true" width="30%" height="30%" /><img src="https://github.com/wentaojia2014/wentaojia2014.github.io/blob/master/img/zhifubao.jpg?raw=true" width="30%" height="30%" />

###### 觉得分享的内容还不错, 就请作者喝杯奶茶吧~~
***
