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


## Validate a string is an integer within a range.
proc validateInteger {string from to} {
	if {![string is integer -strict $string]} {
		return false
	}
	if {$string<$from} {
		return false
	}

	expr {$string<=$to}
}


##
## GUI procedures.
##

## Apply grid state.
proc applyGrid {} {
	.screen itemconfigure grid -state [expr {$::grid?"normal":"hidden"}]
}


## Apply labels state.
proc applyLabels {} {
	if {$::labels} {
		## Turn on labels for all visible groups.
		for {set g 1} {$g<100} {incr g} {
			if {[set ::group$g]} {
				foreach item [.screen find withtag [list group $g]] {
					if {"labels" in [.screen gettags $item]} {
						.screen itemconfigure $item -state normal
					}
				}
			}
		}
	} else {
		## Turn off all labels.
		.screen itemconfigure labels -state hidden
	}
}


## Rearrange by layer.
proc rearrangeByLayer {} {
	foreach tag {grid floors ladders burgers plates scores bonuses lives gamesigns labels picked cursor} {
		.screen raise $tag
	}
}


## Switch to another level.
proc switchLevel {} {
	## Skip if level is the same.
	if {$::previousLevel eq $::level} {
		return true
	}

	## Save current state.
	dict set ::groups $::previousLevel [dict map {group state} [dict get $::groups $::previousLevel] {
		set state [set ::group$group]
	}]

	## Apply state from new level.
	dict for {group state} [dict get $::groups $::level] {
		set ::group$group $state
	}

	## Save previous level.
	set ::previousLevel $::level

	## Hide/show groups.
	applyGroups
}


## Apply groups.
proc applyGroups {} {
	for {set g 1} {$g<100} {incr g} {
		applyGroup $g
	}
}

proc applyGroup {g} {
	if {$::labels} {
		.screen itemconfigure [list group $g] -state [expr {[set ::group$g]?"normal":"hidden"}]
	} else {
		foreach item [.screen find withtag [list group $g]] {
			if {"labels" ni [.screen gettags $item]} {
				.screen itemconfigure $item -state [expr {[set ::group$g]?"normal":"hidden"}]
			}
		}
	}
}


## Apply cursor position.
set xdiff 0
set ydiff 0
proc applyCursorPosition {x y} {
	if {[gc $x]<0 || [gc $x]>=$::screen_width || [gc $y]<0 || [gc $y]>=$::screen_height} {
		## Hide cursor.
		.screen itemconfigure cursor -state hidden

		## Delete picked items.
		.screen delete picked

		## Create replacement to picked items.
		.screen create image 0 0 -state hidden -tags [list images pickedimage picked] -anchor nw
		.screen create text 0 0 -state hidden -tags [list labels pickedlabel picked] -anchor c

		## Raise the cursor.
		.screen raise cursor picked
	} else {
		## Show and move cursor.
		.screen itemconfigure cursor -state normal
		.screen moveto cursor [expr {[sc [gc $x]]-2}] [expr {[sc [gc $y]]-2}]

		## Move picked item.
		.screen moveto pickedimage [sc [expr {[gc $x]+$::xdiff}]] [sc [expr {[gc $y]+$::ydiff}]]
		.screen moveto pickedlabel [sc [expr {[gc $x]+$::xdiff-1}]] [sc [expr {[gc $y]+$::ydiff-1}]]
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
				score {
					for {set i 0} {$i<3} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] $y] withtag $item
					}
					for {set i 1} {$i<=7} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] [expr {$y+1}]] withtag $item
					}
				}
				bonus {
					for {set i 0} {$i<3} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] $y] withtag $item
					}
					for {set i 1} {$i<=3} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] [expr {$y+1}]] withtag $item
					}
				}
				lives {
					for {set i 0} {$i<$::lives_draw_max} {incr i} {
						.screen addtag [list xy $x [expr {$y+$i}]] withtag $item
					}
				}
				gamesign {
					for {set i 0} {$i<5} {incr i} {
						for {set j 0} {$j<12} {incr j} {
							.screen addtag [list xy [expr {$x+$j}] [expr {$y+$i}]] withtag $item
						}
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


## Add picked group label.
proc addPickedGroupLabel {group tags} {
	.screen itemconfigure pickedlabel \
		-state normal -fill white -font $::grouplabelfont -text [format %02d $group] -angle 30 \
		-tags [list labels pickedlabel picked {*}$tags]
}


## Add a floor item.
proc addFloor {type len} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list floor $type $len]
	.screen itemconfigure pickedimage \
		-state normal -image [floor $type $len] \
		-tags [list images pickedimage picked floors [list floor $type $len]]
}


## Add a ladder item.
proc addLadder {type len} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list ladder $type $len]
	.screen itemconfigure pickedimage \
		-state normal -image [ladder $type $len] \
		-tags [list images pickedimage picked ladders [list ladder $type $len]]
}


## Add a burger component.
proc addBurger {type} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list burger $type]
	.screen itemconfigure pickedimage \
		-state normal -image [dict get $::burgers $type] \
		-tags [list images pickedimage picked burgers [list burger $type]]
}


## Add a plate.
proc addPlate {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list plate]
	.screen itemconfigure pickedimage \
		-state normal -image $::plate \
		-tags [list images pickedimage picked plates [list plate]]
}


## Add score field.
proc addScore {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list score]
	.screen itemconfigure pickedimage \
		-state normal -image $::score \
		-tags [list images pickedimage picked scores [list score]]
}


## Add bonus field.
proc addBonus {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list bonus]
	.screen itemconfigure pickedimage \
		-state normal -image $::bonus \
		-tags [list images pickedimage picked bonuses [list bonus]]
}


## Add lives field.
proc addLives {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list lives]
	.screen itemconfigure pickedimage \
		-state normal -image $::lives \
		-tags [list images pickedimage picked lives [list lives]]
}


## Add game sign.
proc addGameSign {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list gamesign]
	.screen itemconfigure pickedimage \
		-state normal -image $::gamesign \
		-tags [list images pickedimage picked gamesigns [list gamesign]]
}



## Drop a picked item or pick a dropped one.
proc dropOrPick {x y} {
	if {[.screen itemcget pickedimage -state] eq "normal"} {
		## Drop.
		## Add xy tags to picked items.
		addXyTags picked [expr {[gc $x]+$::xdiff}] [expr {[gc $y]+$::ydiff}]

		## Add group tag.
		.screen addtag [list group $::group] withtag picked

		## Remove the picked tags from all items that had it.
		.screen dtag picked picked
		.screen dtag pickedimage pickedimage
		.screen dtag pickedlabel pickedlabel

		## Create replacement to picked item.
		.screen create image 0 0 -state hidden -tags [list images pickedimage picked] -anchor nw
		.screen create text 0 0 -state hidden -tags [list labels pickedlabel picked] -anchor c

		## Rearrange by layer.
		rearrangeByLayer
	} else {
		## Pick the latest two items at the cursor position.
		foreach item [lrange [.screen find withtag [list xy [gc $x] [gc $y]]] end-1 end] {
			if {"images" in [.screen gettags $item]} {
				## Set diff to cursor for image item.
				lassign [.screen coords $item] xi yi
				set ::xdiff [expr {[gc $xi]-[gc $x]}]
				set ::ydiff [expr {[gc $yi]-[gc $y]}]

				## Add the picked tags to the selected item.
				.screen addtag pickedimage withtag $item
				.screen addtag picked withtag $item

				## Remove all xy tags from item.
				removeXyTags pickedimage
			} elseif {"labels" in [.screen gettags $item]} {
				## Add the picked tags to the selected item.
				.screen addtag pickedlabel withtag $item
				.screen addtag picked withtag $item

				## Remove all xy tags from item.
				removeXyTags pickedlabel
			}
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
	foreach var {screen_height screen_width tilemap_width unique_tiles_count lives_draw_max} {
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


## Create score and bonus.
foreach element {score bonus} count {7 3} {
	set photo [image create photo]
	foreach side {left middle right} x {0 1 2} {
		lassign [dict get $::coords tiles [list $element $side]] row column
		copyTile $::tilesphoto $photo $row $column $x 0
	}
	for {set x 1} {$x<=$count} {incr x} {
		lassign [dict get $::coords tiles [list number $x]] row column
		copyTile $::tilesphoto $photo $row $column $x 1
	}
	set $element $photo
}


## Create lives.
set photo [image create photo]
for {set y 0} {$y<$::lives_draw_max} {incr y} {
	lassign [dict get $::coords tiles [list cook small]] row column
	copyTile $::tilesphoto $photo $row $column 0 $y
}
set lives $photo


## Create "Würgertime" game sign.
set photo [image create photo]
set y 0
foreach tiles {{{sign top} {sign top} {sign top} {sign top} {sign top} {sign top} {open on left} {open on right} {sign top} {sign top} {sign top} {sign top}}
	{{sign left} {w upper left} {w upper right} {space} {space} {space} {burger buntop left} {burger buntop right} {space} {space} {space} {sign right}}
	{{sign left} {w lower left} {w lower right} {uerger left} {uerger middle} {uerger right} {burger patty left} {burger patty right} {time left} {time middle} {time right} {sign right}}
	{{plate left} {plate middle} {plate middle} {plate middle} {plate middle} {sign left} {burger bunbottom left} {burger bunbottom right} {sign right} {plate middle} {plate middle} {plate right}}
	{{sign right} {space} {space} {space} {space} {plate left} {plate middle} {plate middle} {plate right} {space} {space} {sign left}}} {
	set x 0
	foreach tile $tiles {
		lassign [dict get $::coords tiles $tile] row column
		copyTile $::tilesphoto $photo $row $column $x $y
		incr x
	}
	incr y
}
set gamesign $photo


## Display.
frame .sprites
dict for {sname simage} [lsort -stride 2 -dictionary $sprites ] {
	set w .sprites.[join $sname _]
	ttk::label $w -image $simage
	pack $w -side left -padx 5 -pady 5
}
#pack .sprites


## Fonts.
set grouplabelfont [font create -family monospace -weight bold -size 14]


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
## Levels and groups.
##

## Setup groups dict.
set groups [dict create]
for {set l 0} {$l<100} {incr l} {
	for {set g 1} {$g<100} {incr g} {
		dict set groups $l $g 0
	}
}
set labels 1


## Setup spinbox/checkbox frame.
frame .bottom
frame .bottom.groups
ttk::label .bottom.grouplabel -text "Group:"
set ::previousLevel 0
set ::level 0
ttk::spinbox .bottom.level -width 2 -from 0 -to 99 -textvariable ::level -validate focusout -validatecommand {validateInteger %P 0 99} -command switchLevel
ttk::label .bottom.levellabel -text "Level:"
set ::group 1
ttk::spinbox .bottom.group -width 2 -from 1 -to 99 -textvariable ::group -validate focusout -validatecommand {validateInteger %P 1 99}

set gs {}
for {set y 0} {$y<5} {incr y} {
	for {set x 0} {$x<20} {incr x} {
		set g [expr {$y*20+$x}]
		if {$g!=0} {
			set group$g 0
			ttk::checkbutton .bottom.groups.$g -text [format %02d $g] -variable ::group$g -command [list applyGroup $g]
			lappend gs .bottom.groups.$g
		} else {
			lappend gs x
		}
	}
	grid {*}$gs
	set gs {}
}

grid .bottom.levellabel  .bottom.groups
grid .bottom.level       ^              -sticky n
grid .bottom.grouplabel  ^
grid .bottom.group       ^              -sticky n
grid columnconfigure .bottom 0 -pad 30


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
.menu.player add command -command [list addScore] -label "Score"
.menu.player add command -command [list addBonus] -label "Bonus"
.menu.player add command -command [list addLives] -label "Lives"


## Create opponents menu.
menu .menu.opponents


## Create decoration menu.
menu .menu.decoration
.menu.decoration add command -command [list addGameSign] -label "Game Sign"


## Create view menu.
menu .menu.view
.menu.view add checkbutton -variable grid -command applyGrid -label "Grid" -underline 0 -accelerator "Crtl+G"
.menu.view add checkbutton -variable labels -command applyLabels -label "Labels" -underline 0 -accelerator "Crtl+L"
bind . <Control-g> {set grid [expr {!$grid}] ; applyGrid}
bind . <Control-l> {set labels [expr {!$labels}] ; applyLabels}


## Setup menubar.
.menu add cascade -menu .menu.file       -label "File"       -underline 1
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
pack .bottom -side bottom -fill x
pack .screen
. configure -menu .menu


##
## Import data from file.
##
.screen create image 0 0 -tags [list images pickedimage picked] -anchor nw
addFloor bothcaps 10
.screen moveto picked [sc 5] [sc 10]
dropOrPick [sc 5] [sc 10]


## Create picked item.
#.screen create image 0 0 -state hidden -tags picked -anchor nw



## Drop into tk event loop.
vwait endless
