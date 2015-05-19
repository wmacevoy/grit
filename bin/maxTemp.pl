#!/bin/perl -w

$maxServo=0;
$maxTemp=0;
%temps=();
$t=0;
while (<>)
{
    if (m/t=([-.0-9]+)/) {
	if ($1 >= $t + 15) {
	    $t=$1;
	    print "$1,$maxServo,$maxTemp";
	    foreach $servo (keys(%temps)) {
		$temp=$temps{$servo};
		if ($temp == $maxTemp) {
		    print ",$servo=>\033[1;31m${temp}\033[0m";
		} elsif ($temp >= $maxTemp-5) {
		    print ",$servo=>${temp}";
		}
	    }
	    print "\n";
	    $maxServo=0;
	    $maxTemp=0;
	    %temps=();
	}
	while (m/angle\[([0-9]+)\]=([-.0-9]+) temp ([-.0-9]+)/g) {
	    $servo=$1;
	    $temp=$3;
	    $temps{$servo}=$temp;
	    if ($temp > $maxTemp) {
		$maxTemp = $temp;
		$maxServo= $servo;
	    }
	}
    }
}
