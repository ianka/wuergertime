#!/usr/bin/tclsh

fconfigure stdin -translation binary
fconfigure stdout -translation binary

set width [lindex $argv 0]

while {![eof stdin]} {
	set line [read stdin [expr 8*$width]]
	for {set tile 0} {$tile<[expr $width/8]} {incr tile} {
		for {set y 0} {$y<8} {incr y} {
			for {set x 0} {$x<8} {incr x} {
				puts -nonewline [string index $line [expr $width*$y+(8*$tile+$x)]]
			}
		}
	}
}

