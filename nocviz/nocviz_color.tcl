# utility methods for interacting with colors in nocviz

# https://github.com/gka/chroma.js/blob/master/src/io/hsl/rgb2hsl.js

proc fmod {val mod} {
	set res $val
	while {$res > $mod} {
		set res [expr $res - $mod]
	}
	return $res
}

proc rgb2hsl {r g b} {
	set rp [expr (1.0 * $r) / 255.0]
	set gp [expr (1.0 * $g) / 255.0]
	set bp [expr (1.0 * $b) / 255.0]
	set max [::tcl::mathfunc::max $rp $gp $bp]
	set min [::tcl::mathfunc::min $rp $gp $bp]
	set delta [expr $max - $min]

	set h 0

	if {$delta == 0} {
		set h 0
	} elseif {$max == $rp} {
		set h [fmod [expr ($gp - $bp) / $delta] 6]
		set h [expr 60 * $h]
	} elseif {$max == $gp} {
		set h [expr 60 *  (( ($bp - $rp) / $delta ) + 2) ]
	} elseif {$max == $bp} {
		set h [expr 60 *  (( ($rp - $gp) / $delta ) + 4) ]
	}

	set l [expr ($max + $min) / 2.0]

	set s 0
	if {$delta == 0} {
		set s 0
	} else {
		set s [expr $delta / (1.0 - abs(2.0 * $l - 1.0)) ]
	}

	return [list [expr $h] [expr 100 * $s] [expr 100 * $l]]
}

proc hsl2rgb {h s l} {

	set s [expr (1.0 * $s) / 100.0]
	set l [expr (1.0 * $l) / 100.0]

	set c [expr (1.0 - abs(2.0 * $l - 1.0)) * $s]
	set i [fmod [expr ((1.0 * $h) / 60)] 2]
	set x [expr $c * (1.0 - abs($i - 1.0))]
	set m [expr $l - $c / 2.0]

	set rp 0
	set gp 0
	set bp 0

	if {$h < 60} {
		set rp $c
		set gp $x
		set bp 0
	} elseif {$h < 120} {
		set rp $x
		set gp $c
		set bp 0
	} elseif {$h < 180} {
		set rp 0
		set gp $c
		set bp $x
	} elseif {$h < 240} {
		set rp 0
		set gp $x
		set bp $c
	} elseif {$h < 300} {
		set rp $x
		set gp 0
		set bp $c
	} elseif {$h < 360} {
		set rp $c
		set gp 0
		set bp $x
	}

	return [list [expr int(($rp + $m) * 255)] [expr int(($gp + $m) * 255)] [expr int(($bp + $m) * 255)]]

}

# HSL interpolate drawn from https://github.com/jackparmer/colorlover

proc interp_linear {frac start end} {
	return [expr $start + ($end - $start) * $frac]
}

proc interp_circular {frac start end} {
	set s_mod [fmod $start 360]
	set e_mod [fmod $end 360]
	if { [expr max($s_mod, $e_mod) - min($s_mod, $e_mod)] > 180 } {
		if {$s_mod < $e_mod} {
			set s_mod [expr $s_mod + 360]
		} else {
			set e_mod [expr $e_mod + 360]
		}
		return [fmod [interp_linear $frac $s_mod $e_mod] 360]
	} else {
		return [interp_linear $frac $s_mod $e_mod]
	}
}

# interpolate between two HSL color tuples
#
# frac should be in 0..1
proc hsl_interp {frac h1 s1 l1 h2 s2 l2} {
	return [list [interp_circular $frac $h1 $h2] [interp_circular $frac $s1 $s2] [interp_circular $frac $l1 $l2]]

}

# interpolate between RGB colors using HSL 
proc rgb_interp {frac r1 g1 b1 r2 g2 b2} {
	set hsl1 [rgb2hsl $r1 $g1 $b1]
	set hsl2 [rgb2hsl $r2 $g2 $b2]
	return [hsl2rgb {*}[hsl_interp $frac {*}$hsl1 {*}$hsl2]]
}
