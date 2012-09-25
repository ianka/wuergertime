#!/usr/bin/tclsh
#
#  pcxtotiles.tcl - a minimalist PCX to tiles converter for uzebox video mode 3.
#
#  PCX must be 8 bits per pixel/1 plane, like needed for uzebox.
#  A palette given in the file is ignored as uzebox has a fixed palette.
#  Pixels per line and number of lines must be a multiple of 8.
#  The image may have any number of tiles per line you find useful.
# 
# (C)2012 Janka <jjj@gmx.de>, use and distribute under the terms of GNU GPLv3 or any later version.
#


## Binary input.
fconfigure stdin -translation binary

## Read and decode PCX header.
binary scan [read stdin 128] @3c@6s@10s@65cs bpp umg lmg ncp nbs
set nbs [expr $nbs & 0xffff]
set lines [expr ($lmg & 0xffff - $umg & 0xffff)/8]

## Reject non-8 bpp/1 plane files.
if {($bpp != 8) || ($ncp != 1)} {
	puts stderr "Uzebox requires an 8 bpp 1 plane indexed PCX file as source for tiles."
	exit
}

## Output header.
puts "const char Tiles\[\] PROGMEM={"

## Read until all tile lines are read.
set linenumber 0
while {$linenumber<=$lines} {
	## Read 8 scanlines into one line of tiles.
	set line {}
	set i [expr 8*$nbs]
	while {$i>0} {
		set byte [read stdin 1]
		binary scan $byte c value
		set value [expr $value & 0xff]

		## RLE byte?
		if {$value >= 192} {
			## Yes. Get next byte for actual pixel value.
			set byte [read stdin 1]

			## Repeat pixel.
			for {} {$value>192} {incr value -1} {
				## Store pixel value.
				append line $byte

				## Next pixel.
				incr i -1
			}
		} {
			## No. Store pixel value.
			append line $byte

			## Next pixel.
			incr i -1
		}
	}

	## Go through all tiles in that line.
	for {set tile 0} {$tile<[expr $nbs/8]} {incr tile} {
		## Arrange tile.
		puts -nonewline "/* Tile #[expr $linenumber*$nbs/8+$tile] ($linenumber|$tile) */\n\t"
		for {set y 0} {$y<8} {incr y} {
				binary scan [string range $line [expr $nbs*$y+8*$tile] [expr $nbs*$y+8*$tile+7]] c8 VALUES
				foreach VALUE $VALUES {
					puts -nonewline "0x[format %02x [expr $VALUE & 0xff]], "
				}
				puts -nonewline "\n\t"
		}
		puts ""
	}

	## Next tile line
	incr linenumber
}

## Output footer.
puts "};"

