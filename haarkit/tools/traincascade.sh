./opencv_traincascade -data data/ -vec data/vector.vec -bg negative/negatives.txt -numPos 1000 -numNeg 7 -numStages 13 -featureType HAAR -w 40 -h 40 -bt GAB -maxFalseAlarmRate 0.3 -mode ALL
