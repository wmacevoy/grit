#!/bin/bash

find ./negativeImages -name '*.jpg' >negatives.dat

find ./positiveImages -name '*.png' >positives.dat

perl createtrainsamples.pl positives.dat negatives.dat samples 5000 "./opencv_createsamples -bgcolor 0 -bgthresh 0 -maxxangle 1.1 -maxyangle 1.1 maxzangle 0.5 -maxidev 40 -w 50 -h 50"

find samples/ -name '*.vec' > samples.dat
./mergevec samples.dat samples.vec

./opencv_createsamples -vec samples.vec -show -w 50 -h 50 # Extra: If you want to see inside

./opencv_traincascade -data haarcascade -vec samples.vec -bg negatives.dat -numPos 1000 -numNeg 165 -numStages 12 -featureType LBP -minHitRate 0.99 -maxFalseAlarmRate 0.5 -mem 2048 -w 50 -h 50 -mode ALL -maxWeakCount 70

#OR

#./opencv_haartraining -data haarcascade -vec samples.vec -bg negatives.dat -nstages 12 -nsplits 2 -minhitrate 0.99 -maxfalsealarm 0.5 -npos 2000 -nneg 165 -w 50 -h 50 -nonsym -mem 2048 -mode ALL
