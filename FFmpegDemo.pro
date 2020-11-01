TEMPLATE = subdirs
SUBDIRS = \
    demos/YUVToH264 \
    demos/YUVFromH264 \
#    demos/RGBToH264
#    demos/Convert \
#    demos/Muxing  \
#    demos/Remuxing \
#    demos/Capture \
    demos/Player
OTHER_FILES += .* \
    _clang-format \
    LICENSE \
    Readme.md


