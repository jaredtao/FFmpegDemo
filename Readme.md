## FFMEPG DEMO

####  学习FFMPEG的时候写的一些例子


* **YUV**

  yuv转码h264

* **YUVDecoding**

  h264解码为yuv

* **Capture**

  读取本地视频文件并播放 （这个可能不好用，直接看最后一个Player）

* **RGBToH264**

  RGB转换为h264

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
