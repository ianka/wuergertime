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
	foreach tag {grid floors ladders burgers plates scores levels bonuses lives peppers signs opponentstartpoints playerstartpoints labels picked cursor} {
		.screen raise $tag
	}
}


## Switch to another level.
proc switchLevel {} {
	## Skip if level is the same.
	if {$::previousLevel eq $::currentLevel} {
		return true
	}

	## Save current state.
	dict set ::groups $::previousLevel [dict map {group state} [dict get $::groups $::previousLevel] {
		set state [set ::group$group]
	}]

	## Apply state from new level.
	dict for {group state} [dict get $::groups $::currentLevel] {
		set ::group$group $state
	}

	## Save previous level.
	set ::previousLevel $::currentLevel

	## Hide/show groups.
	applyGroups
}


## Switch displayed options group.
proc switchOptionsGroup {} {
	createOptionsGroup $::currentOptionsGroup
	if {[grid slaves .right.options] ne {}} {
		grid remove {*}[grid slaves .right.options]
	}
	grid .right.options.group$::currentOptionsGroup
}


## Switch displayed attack waves group.
proc switchAttackWavesGroup {} {
	createAttackWavesGroup $::currentAttackWavesGroup
	if {[grid slaves .right.attackwaves] ne {}} {
		grid remove {*}[grid slaves .right.attackwaves]
	}
	grid .right.attackwaves.group$::currentAttackWavesGroup
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
	dict set ::groups $::currentLevel $g [set ::group$g]
}


## Apply cursor position.
set xdiff 0
set ydiff 0
set cursorindicator {}
proc applyCursorPosition {x y} {
	if {[gc $x]<0 || [gc $x]>=$::screen_width || [gc $y]<0 || [gc $y]>=$::screen_height} {
		## Hide cursor.
		.screen itemconfigure cursor -state hidden

		## Delete picked items.
		.screen delete picked

		## Create replacement to picked items.
		.screen create image 0 0 -state hidden -tags [list images pickedimage picked] -anchor nw
		.screen create text 0 0 -state hidden -tags [list labels pickedlabel picked] -anchor se

		## Raise the cursor.
		.screen raise cursor picked

		## Update coordinate indicator.
		set ::cursorindicator {}
	} else {
		## Show and move cursor.
		.screen itemconfigure cursor -state normal
		.screen moveto cursor [expr {[sc [gc $x]]-2}] [expr {[sc [gc $y]]-2}]

		## Move picked item.
		.screen moveto pickedimage [sc [expr {[gc $x]+$::xdiff}]] [sc [expr {[gc $y]+$::ydiff}]]
		.screen moveto pickedlabel [sc [expr {[gc $x]+$::xdiff-1}]] [sc [expr {[gc $y]+$::ydiff-1}]]

		## Update coordinate indicator.
		set ::cursorindicator "[gc $x],[gc $y]"
	}
}


## Add/remove xy and origin tags.
proc addXyTags {tag x y} {
	foreach item [.screen find withtag $tag] {
		foreach t [.screen gettags $item] {
			.screen addtag [list origin $x $y] withtag $item
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
				level - bonus {
					for {set i 0} {$i<3} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] $y] withtag $item
					}
					for {set i 1} {$i<=3} {incr i} {
						.screen addtag [list xy [expr {$x+$i}] [expr {$y+1}]] withtag $item
					}
				}
				lives - peppers {
					for {set i 0} {$i<$::lives_draw_max} {incr i} {
						.screen addtag [list xy $x [expr {$y+$i}]] withtag $item
					}
				}
				sign {
					for {set i 0} {$i<5} {incr i} {
						for {set j 0} {$j<12} {incr j} {
							.screen addtag [list xy [expr {$x+$j}] [expr {$y+$i}]] withtag $item
						}
					}
				}
				playerstartpoint - opponentstartpoint {
					for {set i 0} {$i<2} {incr i} {
						for {set j 0} {$j<2} {incr j} {
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
				xy - origin {.screen dtag picked $t}
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
	addPickedGroupLabel $::group [list [list floor $type $len]]
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
	addPickedGroupLabel $::group [list [list ladder $type $len]]
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
	addPickedGroupLabel $::group [list [list burger $type]]
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
	addPickedGroupLabel $::group [list [list plate]]
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
	addPickedGroupLabel $::group [list [list score]]
	.screen itemconfigure pickedimage \
		-state normal -image $::score \
		-tags [list images pickedimage picked scores [list score]]
}


## Add level field.
proc addLevel {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list [list level]]
	.screen itemconfigure pickedimage \
		-state normal -image $::level \
		-tags [list images pickedimage picked levels [list level]]
}


## Add bonus field.
proc addBonus {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list [list bonus]]
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
	addPickedGroupLabel $::group [list [list lives]]
	.screen itemconfigure pickedimage \
		-state normal -image $::lives \
		-tags [list images pickedimage picked lives [list lives]]
}


## Add peppers field.
proc addPeppers {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list [list peppers]]
	.screen itemconfigure pickedimage \
		-state normal -image $::peppers \
		-tags [list images pickedimage picked peppers [list peppers]]
}



## Add game sign.
proc addSign {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list [list sign]]
	.screen itemconfigure pickedimage \
		-state normal -image $::sign \
		-tags [list images pickedimage picked signs [list sign]]
}


## Add a player start point.
proc addPlayerStartpoint {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list [list playerstartpoint]]
	.screen itemconfigure pickedimage \
		-state normal -image $::playerstartpoint \
		-tags [list images pickedimage picked playerstartpoints [list playerstartpoint]]
}


## Add an opponent start point.
proc addOpponentStartpoint {} {
	set ::xdiff 0
	set ::ydiff 0
	set ::group$::group 1
	applyGroup $::group
	addPickedGroupLabel $::group [list [list opponentstartpoint]]
	.screen itemconfigure pickedimage \
		-state normal -image $::opponentstartpoint \
		-tags [list images pickedimage picked opponentstartpoints [list opponentstartpoint]]
}


## Set level options.
proc setOptions {options} {
	set ::stompCount$::group {}
	set ::opponentRandomness$::group {}
	set ::opponentNumber$::group {}
	set ::attackWaveSpeed$::group {}
	set ::bonusSpeed$::group {}
	foreach option $options {
		switch -regexp -matchvar match -- $option {
			{^stomp_(.*)$}               {set ::stompCount$::group         [lindex $match 1]}
			{^opponent_randomness_(.*)$} {set ::opponentRandomness$::group [lindex $match 1]}
			{^opponent_(.*)$}            {set ::opponentNumber$::group     [lindex $match 1]}
			{^attack_wave_(.*)$}         {set ::attackWaveSpeed$::group    [lindex $match 1]}
			{^bonus_(.*)$}               {set ::bonusSpeed$::group         [lindex $match 1]}
		}
	}
}


## Set attack waves.
proc setAttackWaves {attackwaves} {
	createAttackWavesGroup $::group
	set w 0
	foreach attackwave $attackwaves {
		set ::attackWave$::group.$w $attackwave
		incr w
	}
}


## Get items at position.
proc getItemsAt {x y} {
	## Get visible items to match.
	set match {}
	for {set g 1} {$g<100} {incr g} {
		if {[set ::group$g]} {
			lappend match [list group $g]
		}
	}

	## Skip if no match.
	if {$match eq {}} return

	## Make an expression from the match.
	set match "[list xy [gc $x] [gc $y]]&&([join $match ||])"

	## Get the latest two items at the cursor position.
	lrange [lsort -integer [.screen find withtag $match]] end-1 end
}


## Drop a picked item or pick a dropped one.
proc dropOrPick {x y} {
	if {[.screen itemcget pickedimage -state] eq "normal"} {
		## Drop.
		## Add xy tags to picked items.
		addXyTags picked [expr {[gc $x]+$::xdiff}] [expr {[gc $y]+$::ydiff}]

		## Add group tag if there isn't any yet.
		if {[lsearch -glob [.screen gettags picked] {group *}] < 0} {
			.screen addtag [list group $::group] withtag picked
		}

		## Remove the picked tags from all items that had it.
		.screen dtag picked picked
		.screen dtag pickedimage pickedimage
		.screen dtag pickedlabel pickedlabel

		## Create replacement to picked item.
		.screen create image 0 0 -state hidden -tags [list images pickedimage picked] -anchor nw
		.screen create text  0 0 -state hidden -tags [list labels pickedlabel picked] -anchor c

		## Rearrange by layer.
		rearrangeByLayer
	} else {
		## Pick the latest two items at the cursor position.
		foreach item [getItemsAt $x $y] {
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


## Change group of an item.
proc changeGroup {x y} {
	## Get the latest two items at the cursor position.
	set items [getItemsAt $x $y]

	## Remove old group tags.
	foreach item $items {
		foreach t [.screen gettags $item] {
			switch -- [lindex $t 0] {
				group {.screen dtag $item $t}
			}
		}
	}

	## Insert into new group.
	foreach item $items {
		## Add new group tag.
		.screen addtag [list group $::group] withtag $item

		## Change label.
		if {"labels" in [.screen gettags $item]} {
			.screen itemconfigure $item -text [format %02d $::group]
		}

		## Activate group.
		set ::group$::group 1
		dict set ::groups $::currentLevel $::group 1
		applyGroup $::group
	}
}


## Load levels from file.
foreach {simplecomponent params} {
	playerstartpoint   {x y}
	opponentstartpoint {x y}
	sign               {x y}
	score              {x y}
	level              {x y}
	bonus              {x y}
	lives              {x y}
	peppers            {x y}
	plate              {x y}
	burger             {type x y}
	floor              {type x y length}
	ladder             {type x y length}} {
	set i 1
	set paramstring {}
	foreach param $params {
		append paramstring [string cat { } $param { [string tolower [lindex $match } $i {]]}]
		incr i
	}
	lappend simplecomponentpatterns \
		[string cat \
			{^[[:blank:]]*LEVEL_COMPONENT_} \
			[string toupper $simplecomponent] \
			{\(} \
			[string repeat {[[:blank:]]*(.+),} [expr {[llength $params]-1}]] \
			{[[:blank:]]*(.+)\),[[:blank:]]*$}] \
		[string cat {dict lappend levelcomponents $component [dict create item } $simplecomponent $paramstring {]}]
}
foreach leveloption {
	stomp_once stomp_twice stomp_threetimes
	opponent_single opponent_duo opponent_trio opponent_quad
	opponent_randomness_minimal opponent_randomness_normal opponent_randomness_medium opponent_randomness_high
	attack_wave_fast attack_wave_medium attack_wave_slow attack_wave_slowest
	bonus_fast bonus_medium bonus_slow bonus_slowest} {
		lappend leveloptionpatterns \
			[string cat {^[[:blank:]]*LEVEL_ITEM_OPTION_} [string toupper $leveloption] {\|?[[:blank:]]*$}] \
			[list lappend leveloptions $leveloption]
}
foreach opponent {egghead sausageman mrmustard anticook} {
	lappend attackwavepatterns \
		[string cat	{^[[:blank:]]*LEVEL_ITEM_ATTACK_WAVE_} [string toupper $opponent] {,?[[:blank:]]*$}] \
		[list lappend attackwaves $opponent]
}

proc loadLevels {filename} {
	## Clear.
	set ::currentLevel 1
	set ::currentOptionsGroup 1
	set ::currentAttackWavesGroup 1
	set ::group 1
	setupGroups
	switchOptionsGroup
	switchAttackWavesGroup
	.screen delete images labels

	## Load the file.
	set levels [loadInclude $filename]

	## Parse the file contents.
	set part 0
	set levelsdrawingspart {}
	set levelscomponentspart {}
	set levelsparts [dict create]
	foreach line [split $levels \n] {
		switch -regexp -match match -- $line {
			{^const uint8_t LevelDrawings\[\] PROGMEM=\{$} {
				incr part
				set levelsdrawingspart $part
			}
			{^const level_item_t LevelComponents\[\] PROGMEM=\{$} {
				incr part
				set levelscomponentspart $part
			}
			{^\}\;$} {
				incr part
			}
			{#define LEVEL_DESIGN "(.*)"$} {
				set ::leveldesign [lindex $match 1]
			}
			{#define LEVEL_AUTHOR "(.*)"$} {
				set ::levelauthor [lindex $match 1]
			}
		}
		dict lappend levelsparts $part $line
	}

	## Parse level components.
	set component 1
	set attackwaves {}
	set attackwavepatterns {}
	set simplecomponentpatterns {}
	set leveloptionpatterns {}
	set leveloptions {}
	set levelcomponents [dict create]
	foreach line [lrange [dict get $levelsparts $levelscomponentspart] 1 end] {
		switch -regexp -matchvar match -- $line \
			{*}$::simplecomponentpatterns \
			{*}$::leveloptionpatterns \
			{*}$::attackwavepatterns \
			{^[[:blank:]]*LEVEL_COMPONENT_END,[[:blank:]]*$} {
				incr component
			} \
			{^[[:blank:]]*\),[[:blank:]]*$} {
				if {$attackwaves ne {}} {
					dict lappend levelcomponents $component [dict create item attackwaves attackwaves $attackwaves]
					set attackwaves {}
				}
				if {$leveloptions ne {}} {
					dict lappend levelcomponents $component [dict create item options options $leveloptions]
					set leveloptions {}
				}
			}
	}

	## Update screen objects from parsed data.
	.screen create image 0 0 -state hidden -tags [list images pickedimage picked] -anchor nw
	.screen create text  0 0 -state hidden -tags [list labels pickedlabel picked] -anchor se
	dict for {group components} $levelcomponents {
		foreach component $components {
			set ::group $group
			dict with component {
				switch -- $item {
					floor   {addFloor  $type $length}
					ladder  {addLadder $type $length}
					burger  {addBurger $type}
					plate   {addPlate}
					sign    {addSign}
					score   {addScore}
					level   {addLevel}
					bonus   {addBonus}
					lives   {addLives}
					peppers {addPeppers}
					playerstartpoint   {addPlayerStartpoint}
					opponentstartpoint {addOpponentStartpoint}
					options            {setOptions $options}
					attackwaves        {setAttackWaves $attackwaves}
				}
				if {$item in {floor ladder burger plate sign score level bonus}} {
					.screen moveto pickedimage [sc $x] [sc $y]
					.screen moveto pickedlabel [sc [expr {$x-1}]] [sc [expr {$y-1}]]
					dropOrPick [sc $x] [sc $y]
				}
				if {$item in {lives peppers}} {
					.screen moveto pickedimage [sc $x] [sc [expr {$y+1-$::lives_draw_max}]]
					.screen moveto pickedlabel [sc [expr {$x-1}]] [sc [expr {$y-$::lives_draw_max}]]
					dropOrPick [sc $x] [sc [expr {$y+1-$::lives_draw_max}]]
				}
				if {$item in {playerstartpoint opponentstartpoint}} {
					.screen moveto pickedimage [sc [expr {$x-1}]] [sc [expr {$y-2}]]
					.screen moveto pickedlabel [sc [expr {$x-2}]] [sc [expr {$y-3}]]
					dropOrPick [sc [expr {$x-1}]] [sc [expr {$y-2}]]
				}
			}
		}
	}

	## Parse level drawings.
	setupLevels
	set level 1
	foreach line [lrange [dict get $levelsparts $levelsdrawingspart] 1 end] {
		if {[regexp -- {^[[:blank:]]*([[:digit:]]+,[[:blank:]]*)+$} $line match]} {
			foreach value [lrange [split $match ,] 0 end-1] {
				set value [expr $value]
				if {$value == 0} {
					incr level
				} else {
					set ::group$value 1
					dict set ::groups $level $value 1
				}
			}
		}
	}

	## Hide/show groups.
	applyGroups
}


## Save levels to file.
proc saveLevels {filename} {
	## Setup LevelDrawings part.
	set levelsdrawings {}
	dict for {level groups} $::groups {
		set found 0
		dict for {group state} $groups {
			if {$state} {
				set found 1
				break
			}
		}
		if {!$found} continue

		append levelsdrawings "\t/* Level $level */\n\t"
		dict for {group state} $groups {
			if {$state} {
				append levelsdrawings $group , { }
			}
		}
		append levelsdrawings "0,\n\n"
	}

	## Get last level component in use.
	set m 0
	for {set g 1} {$g<100} {incr g} {
		if {[.screen find withtag [list group $g]] ne {}} {
			set m $g
		}
	}

	## Setup LevelComponents part.
	for {set g 1} {$g<=$m} {incr g} {
		set items [.screen find withtag [list group $g]]

		append levelscomponents "\t/* Component block $g */\n"

		if {$items ne {}} {
			## Add level options, if any.
			set options {}
			if {[set ::stompCount$g] ne {}} {
				lappend options [string cat "\t\tLEVEL_ITEM_OPTION_STOMP_" [string toupper [set ::stompCount$g]]]
			}
			if {[set ::opponentNumber$g] ne {}} {
				lappend options [string cat "\t\tLEVEL_ITEM_OPTION_OPPONENT_" [string toupper [set ::opponentNumber$g]]]
			}
			if {[set ::opponentRandomness$g] ne {}} {
				lappend options [string cat "\t\tLEVEL_ITEM_OPTION_OPPONENT_RANDOMNESS_" [string toupper [set ::opponentRandomness$g]]]
			}
			if {[set ::attackWaveSpeed$g] ne {}} {
				lappend options [string cat "\t\tLEVEL_ITEM_OPTION_ATTACK_WAVE_" [string toupper [set ::attackWaveSpeed$g]]]
			}
			if {[set ::bonusSpeed$g] ne {}} {
				lappend options [string cat "\t\tLEVEL_ITEM_OPTION_BONUS_" [string toupper [set ::bonusSpeed$g]]]
			}
			if {$options ne {}} {
				append levelscomponents "\tLEVEL_COMPONENT_OPTIONS(\n" [join $options "|\n"] "\n\t),\n"
			}

			## Add attack waves, if any.
			set attackwaves {}
			for {set w 0} {$w<8} {incr w} {
				if {[set ::attackWave$g.$w] ne {}} {
					lappend attackwaves [string cat "\t\tLEVEL_ITEM_ATTACK_WAVE_" [string toupper [set ::attackWave$g.$w]]]
				}
			}
			if {$attackwaves ne {}} {
				append levelscomponents "\tLEVEL_COMPONENT_ATTACKWAVES(\n" [join $attackwaves ",\n"] "\n\t),\n"
			}

			## Add screen items.
			set xyitems [dict create]
			foreach item $items {
				set tags [.screen gettags $item]
				if {"images" in $tags} continue

				set itemtype {}
				set type {}
				set length {}
				foreach tag $tags {
					switch -regexp -matchvar match -- $tag {
						score - level - bonus - lives - peppers - plate - sign - playerstartpoint - opponentstartpoint {
							set itemtype $tag
						}
						{^burger (.+)$} {
							set itemtype burger
							set type [lindex $match 1]
						}
						{^(floor) (.+) ([[:digit:]]+)$} - {^(ladder) (.+) ([[:digit:]]+)$} {
							set itemtype [lindex $match 1]
							set type     [lindex $match 2]
							set length   [lindex $match 3]
						}
						{^origin ([[:digit:]]+) ([[:digit:]]+)$} {
							set x [lindex $match 1]
							set y [lindex $match 2]
						}
					}
				}

				## Remember item by origin.
				set xyitem [list [dict create itemtype $itemtype type $type length $length]]
				if {[dict exists $xyitems $x $y]} {
					dict set xyitems $x $y [concat [dict get $xyitems $x $y] $xyitem]
				} else {
					dict set xyitems $x $y $xyitem
				}
			}

			## Output items, sorted by increasing x, then decreasing y origin.
			dict for {x yitems} [lsort -stride 2 -index 0 -integer $xyitems] {
				append levelscomponents "\n"
				dict for {y items} [lsort -stride 2 -index 0 -integer -decreasing $yitems] {
					foreach item $items {
						dict with item {
							switch -- $itemtype {
								score - level - bonus - plate - sign {
									append levelscomponents [format "\tLEVEL_COMPONENT_%-20s%2d,%2d),\n" [string toupper "$itemtype\("] $x $y]
								}
								burger {
									append levelscomponents [format "\tLEVEL_COMPONENT_%-20s%2d,%2d),\n" [string toupper "$itemtype\($type,"] $x $y]
								}
								floor - ladder {
									append levelscomponents [format "\tLEVEL_COMPONENT_%-20s%2d,%2d,%2d),\n" [string toupper "$itemtype\($type,"] $x $y $length]
								}
								lives - peppers {
									append levelscomponents [format "\tLEVEL_COMPONENT_%-20s%2d,%2d),\n" [string toupper "$itemtype\("] $x [expr {$y-1+$::lives_draw_max}]]
								}
								playerstartpoint - opponentstartpoint {
									append levelscomponents [format "\tLEVEL_COMPONENT_%-20s%2d,%2d),\n" [string toupper "$itemtype\("] [expr {$x+1}] [expr {$y+2}]]
								}
							}
						}
					}
				}
			}
		}
		append levelscomponents "\tLEVEL_COMPONENT_END,\n\n"
	}

	## Write the file.
	set fd [open $filename w+]
	puts -nonewline $fd [string cat {#ifndef LEVELS_INC
#define LEVELS_INC

#include <avr/io.h> /* for uint8_t */
#include <avr/pgmspace.h> /* for PROGMEM */

/* Local includes */
#include "../screens.h" /* for level_item_t */

/* Level design description. */
#define LEVEL_DESIGN "} [string toupper $::leveldesign] {"
#define LEVEL_AUTHOR "} [string toupper $::levelauthor] {"

/* Level descriptions. */
const uint8_t LevelDrawings[] PROGMEM=} "\{\n" $levelsdrawings {	/* End of level drawings */
	0
} "\};" {

const level_item_t LevelComponents[] PROGMEM=} "\{\n" $levelscomponents "\};" {

#endif /* LEVELS_INC */
}]
	close $fd
}


## Open a file.
proc openFile {} {
	set filename [tk_getOpenFile -initialdir [file dirname $::levelFile] -initialfile [file tail $::levelFile]]
	if {$filename eq {}} return

	loadLevels $filename
	set ::levelFile $filename
}

## Revert to saved.
proc revertToSaved {} {
	loadLevels $::levelFile
}

## Save as file.
proc saveFile {} {
	saveLevels $::levelFile
}

## Save as new file.
proc saveFileAs {} {
 	set filename [tk_getSaveFile -initialdir [file dirname $::levelFile] -initialfile [file tail $::levelFile]]
	if {$filename eq {}} return

	saveLevels $filename
	set ::levelFile $filename
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


## Setup default level file.
set levelFile [dict get $::parameters -levelsinc]


## Load include files.
foreach name [list drawh spritesinc tilesh tilesinc] {
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
set floortypes [dict create cap_none "No Caps" cap_left "Left Cap" cap_right "Right Cap" cap_both "Both Caps"]
set floors [dict create]
proc floor {type len} {
	if {[dict exists $::floors $type $len]} {
		return [dict get $::floors $type $len]
	}
	set cap_left [expr {$type eq "cap_left"||$type eq "cap_both"?1:0}]
	set cap_right [expr {$type eq "cap_right"||$type eq "cap_both"?1:0}]
	set photo [image create photo]
	for {set x $cap_left} {$x<$len-$cap_right} {incr x} {
		lassign [dict get $::coords tiles [list floor middle]] row column
		copyTile $::tilesphoto $photo $row $column $x 0
	}
	if {$cap_left} {
		lassign [dict get $::coords tiles [list floor left]] row column
		copyTile $::tilesphoto $photo $row $column 0 0
	}
	if {$cap_right} {
		lassign [dict get $::coords tiles [list floor right]] row column
		copyTile $::tilesphoto $photo $row $column [expr {$len-1}] 0
	}
	dict set floors $type $len $photo
	return $photo
}


## Create needed ladder, if not already done.
set laddertypes [dict create simple "Simple" continued "Continued" uponly "Up Only" contuponly "Both"]
set ladders [dict create]
proc ladder {type len} {
	if {[dict exists $::ladders $type $len]} {
		return [dict get $::ladders $type $len]
	}
	set continued [expr {$type eq "continued"||$type eq "contuponly"?1:0}]
	set uponly [expr {$type eq "uponly"||$type eq "contuponly"?1:0}]
	set photo [image create photo]
	foreach side {left right} x {0 1} {
		lassign [dict get $::coords tiles [list ladder top [expr {$uponly?"uponly":"floorend"}] $side]] row column
		copyTile $::tilesphoto $photo $row $column $x 0
		for {set y 1} {$y<$len} {incr y} {
			lassign [dict get $::coords tiles [list ladder $side]] row column
			copyTile $::tilesphoto $photo $row $column $x $y
		}
		if {!$continued} {
			lassign [dict get $::coords tiles [list ladder bottom floorend $side]] row column
			copyTile $::tilesphoto $photo $row $column $x $len
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
$photo copy [image create photo -data {
	iVBORw0KGgoAAAANSUhEUgAAACgAAAAICAYAAACLUr1bAAABBElEQVQ4y82U
	sW0DMRAEZ9WCi3iGX4MbYOgevgC6Gip39B048hehGkQDcgXjgJYBV2BtcsfD
	4bAgsAOA9PXAveBXuuJhLDZxiLZoFqV7RTta0ZpZG3qgMjRNkzkc+GrEGLsc
	mjLPmatS55GBTYyL+CHdHVwBMGWV43ZGiVsOn/hP2TcOpXhrccWU3Qxb3HgU
	DTeqNtxPuDxP57w9ir/AW4BLeD4RwDl9GD194gIX4YSX9x+TL4/iT3jJ/Mr3
	35B0NLjl/N8hKVsc0ryV3JPMef24Y6ZpHMZqFa/+cGVBU5Wu6bOv0RGriHVS
	pk70yPiDkWrmjsO0uTOicn9gM2IRx57Xae4bYav7h3DUrZAAAAAASUVORK5C
	YII=
	====}] -zoom [dict get $::parameters -zoom]
dict set burgers placeholder $photo

set photo [image create photo]
foreach side {left middleleft middle middleright right} x {0 1 2 3 4} {
	lassign [dict get $::coords tiles [list plate $side]] row column
	copyTile $::tilesphoto $photo $row $column $x 0
}
set plate $photo


## Create player and opponent start points.
set photo [image create photo]
$photo copy [image create photo -data {
	iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAB50lEQVQ4y2Ng
	QAObNv9JOdr88+oTxdc/fzJ+/f+L4ej/V2ILfn5ovnH1SfODFHT1TMicwtpn
	Vz8v/jPjNz/Tg7NNfJE57lcEixm/CV4IsIhk42d/INFeMuMOW+5VBmyAue/B
	a6HCuzfs0o8KMeAA+/h6he7KRNx45eb2GkWC/+Cuq6wLz91gIBJ8m11249e6
	5qsMDAwMLG7nKlK+qEups7NsEttPpAGMYjZWLBxtr94smJHCWPpy6VWWf+wP
	2iVDvBlIAHeLXLZycHxVYPoseF/FRoR/LgOJ4PnO13O53z1SYfrEupBN99+3
	faQa8Oul6j7eR1/ZmML/fWb4se4cqfoZeOxsGBiucDEweR4R/vVxw0knUg0Q
	0zBz+vfd/hfT05v/7jB4nU8m1QC+nxzJ32wi7zDdf/eqf5f/K/eFexmFiNW8
	oeq2ENvOs+7PLE36GRgYGBh67jBejborT3RCOm144sbV4CuoSdrmme9ru/s9
	NxIvFON0iTrzPqGtPHdunJO8BE/KjMgKfO9Pucogpq+u/5J9pxCz6Nzvz5n3
	3Tz2juHRoR9O0me5k32eKrl/YHl+M/OXijZWAxgYGBgqHy5NYWDWLXx6WUbl
	3EIBNr7r/xhUBL78+vLhzZ1nH1/1n3hgNQdZPQBjRcihwBh/4wAAAABJRU5E
	rkJggg==
	====}] -zoom [dict get $::parameters -zoom]
set playerstartpoint $photo

set photo [image create photo]
$photo copy [image create photo -data {
	iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAB6UlEQVQ4y2Ng
	QAMfPvyweHv71fTHcf3XfVnEf4gwsP6471J7/fPTD9MP7L9oga6eCZkzL6pv
	+r+6+WsYuTjus3Ynhoknhgi8ZfwjsC3AIOwfK+t93sL+NUJcgtMZsAFeFs7t
	qiKKs0NCIjkZcABRcXFOfna+2UUGwdtRJJSE5KcLsPPPZiAS3N5xefara88h
	LslIrrXYoBH0JCY2iZNYA95/+Mj5dcWBJxcv3rdgPLPgyPTv3Oz3bUNNuxhI
	AJVh9WV8334pMnw7ffv616+/dRlIBMysrLqizNzXmTj+/Vb8/ef7bVINEBMR
	u/3z709Flr9bjzPsfixMqn6GyMgYhp9TVzKw3Dh4/f6GWz9UGRgYLpNigJ2d
	j6rWeab7LC0fbx24dPeuJ6kGyHIJeb4JcDrAICWlYMHGwPyEiYmJ6GhcsXId
	58fu1U9OnbxpwfTs2YMTfxj/bxblEppEtPWbn066xim02cxc/QRczFJcc3uF
	Y+bs9o5+nC5hY2PjtJAzmt1plQxPyozICiYFN0+P/MfgezfRZxKzFOf2t++e
	3T5x/CTDvt2HVC+eP+755evHPHkhuc333j3MxGoAAwMDw4a1RyxUmDjjxZ88
	c+DbeVTx2smTDNPYvt1f++7mgfc/Pi78////CWT1AKjhwOTcbwi2AAAAAElF
	TkSuQmCC
	====}] -zoom [dict get $::parameters -zoom]
set opponentstartpoint $photo


## Create score, level and bonus.
foreach element {score level bonus} count {7 2 3} {
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


## Create peppers.
set photo [image create photo]
for {set y 0} {$y<$::lives_draw_max} {incr y} {
	lassign [dict get $::coords tiles [list pepper]] row column
	copyTile $::tilesphoto $photo $row $column 0 $y
}
set peppers $photo


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
set sign $photo


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
bind .screen <ButtonPress-3> {changeGroup %x %y}


##
## Levels and groups.
##

## Setup groups and levels.
proc setupGroups {} {
	for {set g 1} {$g<100} {incr g} {
		set ::stompCount$g {}
		set ::opponentRandomness$g {}
		set ::opponentNumber$g {}
		set ::attackWaveSpeed$g {}
		set ::bonusSpeed$g {}
		for {set w 0} {$w<8} {incr w} {
			set ::attackWave$g.$w {}
		}
	}
}

proc setupLevels {} {
	set ::groups [dict create]
	for {set g 1} {$g<100} {incr g} {
		set ::group$g 0
		for {set l 1} {$l<100} {incr l} {
			dict set ::groups $l $g 0
		}
	}
}
setupGroups
setupLevels
set ::labels 1


## Setup options/attack waves frame.
frame .right
ttk::label .right.cursorindicatorlabel -text "Cursor Position:"
ttk::entry .right.cursorindicator -width 6 -state readonly -textvariable ::cursorindicator
grid .right.cursorindicatorlabel
grid .right.cursorindicator

ttk::label .right.leveldesignlabel -text "Level Design:"
ttk::entry .right.leveldesign -textvariable ::leveldesign
ttk::label .right.levelauthorlabel -text "Level Author:"
ttk::entry .right.levelauthor -textvariable ::levelauthor
grid .right.leveldesignlabel
grid .right.leveldesign
grid .right.levelauthorlabel
grid .right.levelauthor

ttk::label .right.optiongrouplabel -text "Option Group:"
ttk::spinbox .right.optiongroup \
	-width 2 -from 1 -to 99 -textvariable ::currentOptionsGroup \
	-validate focusout -validatecommand {validateInteger %P 1 99} \
	-command switchOptionsGroup
frame .right.options
proc createOptionsGroup {g} {
	if {[winfo exists .right.options.group$g]} return

	frame .right.options.group$g
	ttk::label    .right.options.group$g.stomplabel              -text "Stomp:"
	ttk::combobox .right.options.group$g.stomp                   -state readonly -values {{} once twice threetimes} -textvariable ::stompCount$g
	ttk::label    .right.options.group$g.opponentnumberlabel     -text "Opponent number:"
	ttk::combobox .right.options.group$g.opponentnumber          -state readonly -values {{} single duo trio quad} -textvariable ::opponentNumber$g
	ttk::label    .right.options.group$g.opponentrandomnesslabel -text "Opponent randomness:"
	ttk::combobox .right.options.group$g.opponentrandomness      -state readonly -values {{} minimal normal medium high} -textvariable ::opponentRandomness$g
	ttk::label    .right.options.group$g.attackwavespeedlabel    -text "Attack wave speed:"
	ttk::combobox .right.options.group$g.attackwavespeed         -state readonly -values {{} fast medium slow slowest} -textvariable ::attackWaveSpeed$g
	ttk::label    .right.options.group$g.bonusspeedlabel         -text "Bonus speed:"
	ttk::combobox .right.options.group$g.bonusspeed              -state readonly -values {{} fast medium slow slowest} -textvariable ::bonusSpeed$g

	grid .right.options.group$g.stomplabel
	grid .right.options.group$g.stomp
	grid .right.options.group$g.opponentnumberlabel
	grid .right.options.group$g.opponentnumber
	grid .right.options.group$g.opponentrandomnesslabel
	grid .right.options.group$g.opponentrandomness
	grid .right.options.group$g.attackwavespeedlabel
	grid .right.options.group$g.attackwavespeed
	grid .right.options.group$g.bonusspeedlabel
	grid .right.options.group$g.bonusspeed

	grid .right.options.group$g
	grid remove .right.options.group$g
}
grid .right.optiongrouplabel
grid .right.optiongroup
grid .right.options

ttk::label .right.attackwavesgrouplabel -text "Attack Waves Group:"
ttk::spinbox .right.attackwavesgroup \
	-width 2 -from 1 -to 99 -textvariable ::currentAttackWavesGroup \
	-validate focusout -validatecommand {validateInteger %P 1 99} \
	-command switchAttackWavesGroup
frame .right.attackwaves
proc createAttackWavesGroup {g} {
	if {[winfo exists .right.attackwaves.group$g]} return

	frame .right.attackwaves.group$g
	for {set w 0} {$w<8} {incr w} {
		ttk::label    .right.attackwaves.group$g.wavelabel$w -text "Opponent $w:"
		ttk::combobox .right.attackwaves.group$g.wave$w -state readonly -values {{} egghead sausageman mrmustard anticook} -textvariable ::attackWave$g.$w
		grid .right.attackwaves.group$g.wavelabel$w
		grid .right.attackwaves.group$g.wave$w
	}
	grid .right.attackwaves.group$g
	grid remove .right.attackwaves.group$g
}

grid .right.attackwavesgrouplabel
grid .right.attackwavesgroup
grid .right.attackwaves


## Setup spinbox/checkbox frame.
frame .bottom
frame .bottom.groups
set ::previousLevel 1
set ::currentLevel 1
set ::currentOptionsGroup 1
set ::currentAttackWavesGroup 1
ttk::spinbox .bottom.level -width 2 -from 1 -to 99 -textvariable ::currentLevel -validate focusout -validatecommand {validateInteger %P 1 99} -command switchLevel
ttk::label .bottom.levellabel -text "Level:"
set ::group 1
ttk::label .bottom.grouplabel -text "Group:"
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
.menu.burgers add command -command [list addBurger placeholder] -label "Placeholder"
.menu.burgers add command -command [list addPlate] -label "Plate"


## Create misc menu.
menu .menu.misc
.menu.misc add command -command [list addPlayerStartpoint] -label "Player Startpoint"
.menu.misc add command -command [list addOpponentStartpoint] -label "Opponent Startpoint"
.menu.misc add separator
.menu.misc add command -command [list addScore]   -label "Score"
.menu.misc add command -command [list addLevel]   -label "Level"
.menu.misc add command -command [list addBonus]   -label "Bonus"
.menu.misc add command -command [list addLives]   -label "Lives"
.menu.misc add command -command [list addPeppers] -label "Peppers"
.menu.misc add separator
.menu.misc add command -command [list addSign] -label "Game Sign"


## Create decoration menu.
menu .menu.decoration


## Create view menu.
menu .menu.view
.menu.view add checkbutton -variable grid -command applyGrid -label "Grid" -underline 0 -accelerator "Crtl+G"
.menu.view add checkbutton -variable labels -command applyLabels -label "Labels" -underline 0 -accelerator "Crtl+L"
bind . <Control-g> {set grid [expr {!$grid}] ; applyGrid}
bind . <Control-l> {set labels [expr {!$labels}] ; applyLabels}


## Setup menubar.
.menu add cascade -menu .menu.file    -label "File"       -underline 1
.menu add cascade -menu .menu.floors  -label "Floors"     -underline 0
.menu add cascade -menu .menu.ladders -label "Ladders"    -underline 0
.menu add cascade -menu .menu.burgers -label "Burgers"    -underline 0
.menu add cascade -menu .menu.misc    -label "Misc"       -underline 0
.menu add cascade -menu .menu.view    -label "View"       -underline 0


##
## Other settings
##
wm title . "Würgertime Level Editor"
wm protocol . WM_DELETE_WINDOW exit
pack .right  -side right  -fill y
pack .bottom -side bottom -fill x
pack .screen
. configure -menu .menu


##
## Import data from default file.
##
revertToSaved

## Drop into tk event loop.
vwait endless
