#!

DIR=`pwd`
IMAGE=`basename $DIR`

ffmpeg -pattern_type glob -i IMG\*.JPG -vf vidstabdetect=result="transform.trf" -f null -
ffmpeg -pattern_type glob -i IMG\*.JPG -vf vidstabtransform=smoothing=30:input="transform.trf",scale=480:-1 -r 25 -f image2pipe -vcodec ppm - \
	| convert -delay 4 -coalesce -duplicate 1,-2-1 -layers OptimizePlus -loop 0 - ${IMAGE}.gif

