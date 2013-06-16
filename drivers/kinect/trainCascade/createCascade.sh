#!/bin/bash

find ./negativeImages -name '*.jpg' >negatives.dat

find ./positiveImages_left -name '*.png' >positivesl.dat
find ./positiveImages_right -name '*.png' >positivesr.dat

perl createtrainsamples.pl positivesl.dat negatives.dat samples 5000 "./opencv_createsamples -bgcolor 0 -bgthresh 0 -maxxangle 1.1 -maxyangle 1.1 maxzangle 0.5 -maxidev 40 -w 50 -h 50"

perl createtrainsamples.pl positivesr.dat negatives.dat samples 5000 "./opencv_createsamples -bgcolor 0 -bgthresh 0 -maxxangle 1.1 -maxyangle 1.1 maxzangle 0.5 -maxidev 40 -w 50 -h 50"

find samples/ -name '*.vec' > samples.dat
./mergevec samples.dat samples.vec

./opencv_createsamples -vec samples.vec -show -w 50 -h 50 # Extra: If you want to see inside

./opencv_traincascade -data haarcascade -vec samples.vec -bg negatives.dat -numPos 1000 -numNeg 165 -numStages 12 -featureType HAAR -minHitRate 0.99 -maxFalseAlarmRate 0.5 -mem 2048 -w 50 -h 50 -mode ALL -maxWeakCount 70
