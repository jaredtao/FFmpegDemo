# FFMEPG DEMO

学习FFMPEG的时候写的一些示例

## License
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/wentaojia2014/TaoJson/blob/master/LICENSE)

## 目录结构

|路径|用途|说明|
|--|--|--|
|3rdparty| 第三方库|主要是FFmpeg，当前版本为4.2.2，包含头文件和lib库|
|bin| 运行路径|分别包含win32、win64、macos对应的ffmpeg 动态库|
|src|示例程序||
|res|资源文件|包含yuv、mp4格式的视频文件，以及调用ffplay进行播放的一些脚本|

## 示例

* **YUVGenerater**
    
    yuv数据生成器

    用来将I420(即yuv420p)格式的数据，转换生成其它yuv格式，包括：I422、I444、UYVY、YUY2、NV12、NV21。

    用法见命令行参数。默认处理res/flower_cif.yuv文件，生成res/flower_cif.i420等文件。

    可以通过res/ffplay-flower-i420.bat等脚本，播放生成的文件.

    通过此例，熟悉I420 和其它数据格式的转换。 详细的yuv文档可以参考 [yuv](doc/yuv.md)

* **YUVToH264**

    yuv转码h264
  
    不带参数时，默认将res/flower_cif.yuv 转码为res/flower_cif.h264

    目前包括 YUVToH264-FFmpeg  和 YUVToH264-Libx264 两个，分别使用FFmpeg 和Libx264.

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

## 安装依赖

vcpkg install ffmpeg libyuv


## 相关文档

[yuv](doc/yuv.md)

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
