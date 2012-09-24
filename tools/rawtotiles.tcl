#!/usr/bin/tclsh

fconfigure stdin -translation binary

set width [lindex $argv 0]

puts "const char Tiles\[\] PROGMEM={"

set linenumber 0
while {![eof stdin]} {
	set line [read stdin [expr 8*$width]]
	for {set tile 0} {$tile<[expr $width/8]} {incr tile} {
		puts -nonewline "/* Tile #[expr $linenumber*$width/8+$tile] ($linenumber|$tile) */\n\t"
		for {set y 0} {$y<8} {incr y} {
				binary scan [string range $line [expr $width*$y+8*$tile] [expr $width*$y+8*$tile+7]] c8 VALUES
				foreach VALUE $VALUES {
					puts -nonewline "0x[format %02x [expr $VALUE & 0xff]], "
				}
				puts -nonewline "\n\t"
		}
		puts ""
	}
	incr linenumber
}
puts "};"

