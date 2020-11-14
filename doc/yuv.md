
## About YUV

### Planar formats
Planar (or sometimes "triplanar") formats use separate matrices for each of the 3 color components. In other words, there is one table of luminance pixel values, and two separate tables for the chrominance components. This segregated representation in memory of pixels is more convenient for video coding.

#### YUV 4:2:0 (I420/J420/YV12)
##### I420
It has the luma "luminance" plane Y first, then the U chroma plane and last the V chroma plane.

The two chroma planes (blue and red projections) are sub-sampled in both the horizontal and vertical dimensions by a factor of 2. That is to say, for a 2¡Á2 square of pixels, there are 4 Y samples but only 1 U sample and 1 V sample.

This format requires 4¡Á8+8+8=48 bits per 4 pixels, so its depth is 12 bits per pixel.

I420 is by far the most common format in VLC. Most video decoders output raw pictures in I420 format.

A graphical illustration: Each letter represents one bit.

For a single I420 pixel: YYYYYYYY UU VV
For a 50-pixel I420 frame: YYYYYYYY¡Á50 UU¡Á50 VV¡Á50 (or Y¡Á8¡Á50 U¡Á2¡Á50 V¡Á2¡Á50 for short)
For an n-pixel I420 frame: Y¡Á8¡Án U¡Á2¡Án V¡Á2¡Án

##### J420

J420 is exactly like I420, but with a full range ("digital", 0-255) luma (Y) component instead of limited range ("analog", 16-240). The chroma planes are exactly the same as in I420.

##### YV12
YV12 is exactly like I420, but the order of the U and V planes is reversed. In the name, "YV" refers to the plane order: Y, then V (then U). "12" refers to the pixel depth: 12-bits per pixel as for I420.

#### YUV 4:2:2 (I422/J422)
##### I422
Like I420, I422 has one luma plane Y and 2 chroma planes U, V.

However, in I422, chroma planes (blue and red projections) are sub-sampled only in the horizontal dimension, still by a factor of 2. Thus, there is the same amount of lines in chroma planes as in the luma plane.

For a 2¡Á2 group of pixels, there are 4 Y samples and 2 U and 2 V samples each. The depth is 4¡Á8+2¡Á8+2¡Á8=64 bits per 4 pixels, so 16 bits per pixel.

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

For a 2¡Á2 group of pixels, you have 4 Y samples and 1 U and 1 V sample.

It can be helpful to think of NV12 as I420 with the U and V planes interleaved.

Here is a graphical representation of NV12. Each letter represents one bit:

For 1 NV12 pixel: YYYYYYYY UVUV
For a 2-pixel NV12 frame: YYYYYYYYYYYYYYYY UVUVUVUV
For a 50-pixel NV12 frame: Y¡Á8¡Á50 (UV)¡Á2¡Á50
For a n-pixel NV12 frame: Y¡Á8¡Án (UV)¡Á2¡Án

##### NV21
NV21 is like NV12, but with U and V order reversed: it starts with V.

##### Other NV formats
Following the same pattern as NV12/NV21, there are NV16/NV61 (4:2:2 sub-sampling) and NV24/NV42 (4:4:4 sampling) formats. They are mostly used in some funky cheap camera circuitry and not supported in VLC (as of VLC version 2.0).
