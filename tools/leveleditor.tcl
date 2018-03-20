#!/usr/bin/tclsh

##
## Required packages
##
package require Tk
package require Img
package require cmdline


##
## Utility procedures
##

## Load include file.
proc loadInclude {name} {
	set fd [open $name r]
	set data [read $fd]
	close $fd
	return $data
}


## Remove element from list.
proc lremove {list item} {lsearch -all -inline -not -exact $list $item}


## Calculate screen/grid coordinates.
proc sc {xy} {expr {$xy*8*[dict get $::parameters -zoom]}}
proc gc {xy} {expr {int($xy/(8*[dict get $::parameters -zoom]))}}


## Copy tile from tiles images into target image.
proc copyTile {from to row column x y {mirror 0}} {
	$to copy $from \
		-from [expr {$column*8}] [expr {$row*8}] [expr {$column*8+8}] [expr {$row*8+8}] \
		-to [sc $x] [sc $y] \
		-subsample [expr {$mirror?-1:1}] 1 \
		-zoom [dict get $::parameters -zoom]
}


##
## GUI procedures.
##

## Apply grid state.
proc applyGrid {} {
	.screen itemconfigure grid -state [expr {$::grid?"normal":"hidden"}]
}


## Rearrange by layer.
proc rearrangeByLayer {} {
	foreach tag {grid floors ladders burgers plates picked cursor} {
		.screen raise $tag
	}
}


## Apply cursor position.
set xdiff 0
set ydiff 0
proc applyCursorPosition {x y} {
	if {[gc $x]<0 || [gc $x]>=$::screen_width || [gc $y]<0 || [gc $y]>=$::screen_height} {
		## Hide cursor.
		.screen itemconfigure cursor -state hidden

		## Delete picked item.
		.screen delete picked

		## Create replacement to picked item.
		.screen create image 0 0 -state hidden -tags picked -anchor nw

		## Raise the cursor.
		.screen raise cursor picked
	} else {
		## Show and move cursor.
		.screen itemconfigure cursor -state normal
		.screen moveto cursor [expr {[sc [gc $x]]-2}] [expr {[sc [gc $y]]-2}]

		## Move picked item.
		.screen moveto picked [sc [expr {[gc $x]+$::xdiff}]] [sc [expr {[gc $y]+$::ydiff}]]
	}
}


## Add/remove xy tags.
proc addXyTags {tag x y} {
	foreach item [.screen find withtag $tag] {
		foreach t [.screen gettags $item] {
			switch -- [lindex $t 0] {
				floor {
					for {set i 0} {$i<[lindex $t 2]} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] $y] withtag $item
					}
				}
				ladder {
					for {set i 0} {$i<[lindex $t 2]} {incr i} {
						.screen addtag [list xy $x [expr {$y+$i}]] withtag $item
						.screen addtag [list xy [expr {$x+1}] [expr {$y+$i}]] withtag $item
					}
				}
				burger - plate {
					for {set i 0} {$i<5} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] $y] withtag $item
					}
				}
			}
		}
	}
}
proc removeXyTags {tag} {
	foreach item [.screen find withtag $tag] {
		foreach t [.screen gettags $item] {
			switch -- [lindex $t 0] {
				xy {.screen dtag picked $t}
			}
		}
	}
}


## Add a floor item.
proc addFloor {type len} {
	set ::xdiff 0
	set ::ydiff 0
	.screen itemconfigure picked \
		-state normal -image [floor $type $len] \
		-tags [list picked floors [list floor $type $len]]
}


## Add a ladder item.
proc addLadder {type len} {
	set ::xdiff 0
	set ::ydiff 0
	.screen itemconfigure picked \
		-state normal -image [ladder $type $len] \
		-tags [list picked ladders [list ladder $type $len]]
}


## Add a burger component.
proc addBurger {type} {
	set ::xdiff 0
	set ::ydiff 0
	.screen itemconfigure picked \
		-state normal -image [dict get $::burgers $type] \
		-tags [list picked burgers [list burger $type]]
}


## Add a plate.
proc addPlate {} {
	set ::xdiff 0
	set ::ydiff 0
	.screen itemconfigure picked \
		-state normal -image $::plate \
		-tags [list picked plates [list plate]]
}


## Drop a picked item or pick a dropped one.
proc dropOrPick {x y} {
	if {[.screen itemcget picked -state] eq "normal"} {
		## Drop.
		## Add xy tags to picked item.
		addXyTags picked [expr {[gc $x]+$::xdiff}] [expr {[gc $y]+$::ydiff}]

		## Remove the picked tag from all items that had it.
		.screen dtag picked picked

		## Create replacement to picked item.
		.screen create image 0 0 -state hidden -tags picked -anchor nw

		## Rearrange by layer.
		rearrangeByLayer
	} else {
		## Pick the latest item at the cursor position.
 		set item [lindex [.screen find withtag [list xy [gc $x] [gc $y]]] end]

		## Set diff to cursor.
		if {$item ne {}} {
			lassign [.screen coords $item] xi yi
			set ::xdiff [expr {[gc $xi]-[gc $x]}]
			set ::ydiff [expr {[gc $yi]-[gc $y]}]

			## Add the picked tag to the selected item.
			.screen addtag picked withtag $item

			## Remove all xy tags from item.
			removeXyTags picked
		}
	}
}


##
## Initialization
##

## Parse command line.
if {[ catch {set parameters [cmdline::getoptions argv {
	{-background.arg  "#492056"          "level background color"}
	{-cursor.arg      "#c0ff20"          "cursor color"}
	{-drawh.arg       "draw.h"           "draw.h file"}
	{-grid.arg        "#808080"          "grid color"}
	{-levelsinc.arg   "data/levels.inc"  "levels include file"}
	{-sprites.arg     "data/sprites.pcx" "sprites image file"}
	{-spritesinc.arg  "data/sprites.inc" "sprites include file"}
	{-tiles.arg       "data/tiles.pcx"   "tiles image file"}
	{-tilesh.arg      "tiles.h"          "tiles.h file"}
	{-tilesinc.arg    "data/tiles.inc"   "tiles include file"}
	{-transparent.arg "#daffff"          "transparent sprite color"}
	{-zoom.arg        "3"                "image zooming factor"}
}	{[options] -- options are:}]} result]} {
	puts stderr $result
	exit 1
}


## Load include files.
foreach name [list drawh levelsinc spritesinc tilesh tilesinc] {
	set $name [loadInclude [dict get $parameters -$name]]
}


## Parse .h files for constants.
foreach line [split [string cat $drawh $tilesh] \n] {
	foreach var {screen_height screen_width tilemap_width unique_tiles_count} {
		if {[regexp -- [string cat {^#define[[:blank:]]+} [string toupper $var] {[[:blank:]]+([[:graph:]]+)[[:blank:]]*.*$}] $line match value]} {
			set $var $value
		}
	}
}
set unique_rows [expr {$unique_tiles_count/$tilemap_width}]


## Parse all sprite and ingame tile coordinates.
set coords [dict create sprites [dict create] tiles [dict create]]
set pass2 [dict create]
foreach line [split $spritesinc \n] {
	if {[regexp -- {^#define[[:blank:]]+TILES2_([[:graph:]]+)[[:blank:]]+([[:graph:]]+)[[:blank:]]*.*$} $line match name value]} {
		if {[regexp -- {TILES2_(.*)} $value match ref]} {
			dict set pass2 [split [string tolower $name] _] [split [string tolower $ref] _]
			continue
		}
		if {[regexp -- {^mtile\(([[:digit:]]+),([[:digit:]]+),([[:digit:]]+)\)$} $value match row column mirror]} {
			dict set coords sprites [split [string tolower $name] _] [list $row $column $mirror]
		}
	}
}
dict for {name ref} $pass2 {
	dict set coords sprites $name [dict get $coords sprites $ref]
}

set pass2 [dict create]
foreach line [split $tilesinc \n] {
	if {[regexp -- {^#define[[:blank:]]+TILES0_([[:graph:]]+)[[:blank:]]+([[:graph:]]+)[[:blank:]]*.*$} $line match name value]} {
		if {[regexp -- {^POINTS_} $name]} continue
		if {[regexp -- {TILES0_(.*)} $value match ref]} {
			dict set pass2 [split [string tolower $name] _] [split [string tolower $ref] _]
			continue
		}
		if {[regexp -- {^utile\(0,([[:digit:]]+),([[:digit:]]+)\)$} $value match row column]} {
			dict set coords tiles [split [string tolower $name] _] [list $row $column]
		}
		if {[regexp -- {^stile\(0,([[:digit:]]+),([[:digit:]]+)\)$} $value match row column]} {
			dict set coords tiles [split [string tolower $name] _] [list [expr {$unique_rows+$row}] $column]
		}
	}
}
dict for {name ref} $pass2 {
	dict set coords tiles $name [dict get $coords tiles $ref]
}

unset pass2


## Setup sprites.
dict for {key data} [dict get $coords sprites] {
	switch [llength $key] {
		1 - 2 - 3 continue
		4 {
			lassign $key character view step row
			regexp -- {^animate(.*)} $step match step
			set y [expr {$row eq "top"? 0:1}]
			set x 0
		}
		5 {
			lassign $key character view step row column
			regexp -- {^animate(.*)} $step match step
			set y [expr {$row eq "upper"?0:1}]
			set x [expr {$column eq "left"?0:1}]
		}
	}
	dict set sprites [list $character $view $step] [list $y $x] $data
}

set pcxphoto [image create photo -file [dict get $parameters -sprites]]
set xpmphoto [image create photo -data [string map [list "c [dict get $parameters -transparent]" "c None"] [$pcxphoto data -format XPM]]]
set sprites [dict map {ckey cvalue} $sprites {
	set photo [image create photo]
	dict for {yx data} $cvalue {
		lassign $yx y x
		lassign $data row column mirror
		copyTile $xpmphoto $photo $row $column $x $y $mirror
	}
	set cvalue $photo
}]
image delete $xpmphoto
image delete $pcxphoto


## Setup tiles.
set pcxphoto [image create photo -file [dict get $parameters -tiles]]
set tilesphoto [image create photo -data [string map [list "c [dict get $parameters -background]" "c None"] [$pcxphoto data -format XPM]]]
image delete $pcxphoto


## Create needed floor, if not already done.
set floortypes [dict create nocaps "No Caps" leftcap "Left Cap" rightcap "Right Cap" bothcaps "Both Caps"]
set floors [dict create]
proc floor {type len} {
	if {[dict exists $::floors $type $len]} {
		return [dict get $::floors $type $len]
	}
	set leftcap [expr {$type eq "leftcap"||$type eq "bothcaps"?1:0}]
	set rightcap [expr {$type eq "rightcap"||$type eq "bothcaps"?1:0}]
	set photo [image create photo]
	for {set x $leftcap} {$x<$len-$rightcap} {incr x} {
		lassign [dict get $::coords tiles [list floor middle]] row column
		copyTile $::tilesphoto $photo $row $column $x 0
	}
	if {$leftcap} {
		lassign [dict get $::coords tiles [list floor left]] row column
		copyTile $::tilesphoto $photo $row $column 0 0
	}
	if {$rightcap} {
		lassign [dict get $::coords tiles [list floor right]] row column
		copyTile $::tilesphoto $photo $row $column [expr {$len-1}] 0
	}
	dict set floors $type $len $photo
	return $photo
}


## Create needed ladder, if not already done.
set laddertypes [dict create simple "Simple" continued "Continued" uponly "Up Only" both "Both"]
set ladders [dict create]
proc ladder {type len} {
	if {[dict exists $::ladders $type $len]} {
		return [dict get $::ladders $type $len]
	}
	set continued [expr {$type eq "continued"||$type eq "both"?1:0}]
	set uponly [expr {$type eq "uponly"||$type eq "both"?1:0}]
	set photo [image create photo]
	foreach side {left right} x {0 1} {
		lassign [dict get $::coords tiles [list ladder top [expr {$uponly?"uponly":"floorend"}] $side]] row column
		copyTile $::tilesphoto $photo $row $column $x 0
		for {set y 1} {$y<$len-1+$continued} {incr y} {
			lassign [dict get $::coords tiles [list ladder $side]] row column
			copyTile $::tilesphoto $photo $row $column $x $y
		}
		if {!$continued} {
			lassign [dict get $::coords tiles [list ladder bottom floorend $side]] row column
			copyTile $::tilesphoto $photo $row $column $x [expr {$len-1}]
		}
	}
	dict set ladders $type $len $photo
	return $photo
}


## Create burger components and plate.
set burgertypes [dict create buntop "Bun Top" tomato "Tomato" patty "Patty" cheesesalad "Cheese+Salad" bunbottom "Bun Bottom"]
set burgers [dict create]
foreach type [dict keys $burgertypes] {
	set photo [image create photo]
	foreach side {left middleleft middle middleright right} x {0 1 2 3 4} {
		lassign [dict get $::coords tiles [list burger $type $side]] row column
		copyTile $::tilesphoto $photo $row $column $x 0
	}
	dict set burgers $type $photo
}
set photo [image create photo]
foreach side {left middleleft middle middleright right} x {0 1 2 3 4} {
	lassign [dict get $::coords tiles [list plate $side]] row column
	copyTile $::tilesphoto $photo $row $column $x 0
}
set plate $photo


## Display.
frame .sprites
dict for {sname simage} [lsort -stride 2 -dictionary $sprites ] {
	set w .sprites.[join $sname _]
	ttk::label $w -image $simage
	pack $w -side left -padx 5 -pady 5
}
#pack .sprites


##
## Screen
##

## Create screen view.
canvas .screen \
	-background [dict get $parameters -background] \
	-height [expr {$screen_height*8*[dict get $parameters -zoom]}] \
	-width [expr {$screen_width*8*[dict get $parameters -zoom]}]


## Create grid.
for {set y 1} {$y<$screen_height} {incr y} {
	.screen create line 0 [sc $y] [sc $screen_width] [sc $y] \
		-fill [dict get $parameters -grid] -dash [list 1 [expr {2*[dict get $parameters -zoom]-1}]] \
		-tags grid
}
for {set x 1} {$x<$screen_width} {incr x} {
	.screen create line [sc $x] 0 [sc $x] [sc $screen_height] \
		-fill [dict get $parameters -grid] -dash [list 1 [expr {2*[dict get $parameters -zoom]-1}]] \
		-tags grid
}
set grid 1


## Create cursor.
.screen configure -cursor [list @[file join [file dirname $argv0] none.cur] black]
.screen create rectangle 0 0 [sc 1] [sc 1] \
	-outline [dict get $parameters -cursor] -width 3 \
	-tags cursor
bind .screen <Motion> {applyCursorPosition %x %y}
bind .screen <Enter> {applyCursorPosition %x %y}
bind .screen <Leave> {applyCursorPosition %x %y}
bind . <KeyPress-Escape> {.screen itemconfigure picked -state hidden}
bind .screen <ButtonPress-1> {dropOrPick %x %y}


##
## Import data from file.
##
.screen create image 0 0 -tags picked -anchor nw
addFloor bothcaps 10
.screen moveto picked [sc 5] [sc 10]
dropOrPick [sc 5] [sc 10]


## Create picked item.
#.screen create image 0 0 -state hidden -tags picked -anchor nw



##
## Menu
##

## Create menubar.
menu .menu -type menubar


## Create file menu.
menu .menu.file
.menu.file add command -command openFile -label "Open…" -underline 0 -accelerator "Crtl+O"
bind . <Control-o> openfile
.menu.file add command -command revertToSaved -label "Revert to saved" -underline 0
.menu.file add separator
.menu.file add command -command saveFile -label "Save" -underline 0 -accelerator "Crtl+S"
bind . <Control-s> savefile
.menu.file add command -command saveFileAs -label "Save as…" -underline 5
.menu.file add separator
.menu.file add command -command exit -label "Quit" -underline 0 -accelerator "Crtl+Q"
bind . <Control-q> exit


## Create levels menu.
menu .menu.levels
.menu.levels add command -command addLevel    -label "Add…"    -underline 0
.menu.levels add command -command removeLevel -label "Remove…" -underline 0
.menu.levels add command -command swapLevels  -label "Swap…"   -underline 0
.menu.levels add separator


## Create groups menu.
menu .menu.groups
.menu.groups add command -command addGroup    -label "Add…"    -underline 0
.menu.groups add command -command removeGroup -label "Remove…" -underline 0
.menu.groups add command -command swapGroups  -label "Swap…"   -underline 0
.menu.groups add separator


## Create floors menu.
menu .menu.floors
dict for {type name} $floortypes {
	menu .menu.floors.$type
		for {set len 1} {$len<=$screen_width} {incr len} {
			.menu.floors.$type add command -command [list addFloor $type $len] -label $len
	}
	.menu.floors add cascade -menu .menu.floors.$type -label $name -underline 0
}


## Create ladders menu.
menu .menu.ladders
dict for {type name} $laddertypes {
	menu .menu.ladders.$type
		for {set len 1} {$len<=$screen_height} {incr len} {
			.menu.ladders.$type add command -command [list addLadder $type $len] -label $len
	}
	.menu.ladders add cascade -menu .menu.ladders.$type -label $name -underline 0
}


## Create burgers menu.
menu .menu.burgers
dict for {type name} $burgertypes {
	.menu.burgers add command -command [list addBurger $type] -label $name
}
.menu.burgers add separator
.menu.burgers add command -command [list addPlate] -label "Plate"


## Create plays menu.
menu .menu.player


## Create opponents menu.
menu .menu.opponents


## Create decoration menu.
menu .menu.decoration


## Create view menu.
menu .menu.view
.menu.view add checkbutton -variable grid -command applyGrid -label "Grid" -underline 0 -accelerator "Crtl+G"
bind . <Control-g> {set grid [expr {!$grid}] ; applyGrid}


## Setup menubar.
.menu add cascade -menu .menu.file       -label "File"       -underline 0
.menu add cascade -menu .menu.levels     -label "Levels"     -underline 0
.menu add cascade -menu .menu.groups     -label "Groups"     -underline 0
.menu add cascade -menu .menu.floors     -label "Floors"     -underline 0
.menu add cascade -menu .menu.ladders    -label "Ladders"    -underline 0
.menu add cascade -menu .menu.burgers    -label "Burgers"    -underline 0
.menu add cascade -menu .menu.player     -label "Player"     -underline 0
.menu add cascade -menu .menu.opponents  -label "Opponents"  -underline 0
.menu add cascade -menu .menu.decoration -label "Decoration" -underline 0
.menu add cascade -menu .menu.view       -label "View"       -underline 0


##
## Other settings
##
wm title . "Würgertime Level Editor"
wm protocol . WM_DELETE_WINDOW exit
pack .screen
. configure -menu .menu


## Drop into tk event loop.
vwait endless
