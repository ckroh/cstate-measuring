#!/bin/sh
#requires root priviledges

cd /sys/devices/system/cpu || exit

case "$1" in
    "-on")
    	echo "disabling other C-states"
        /bin/echo -n 1 | sudo tee cpu*/cpuidle/state*/disable
	case "$2" in
		"c1")
			/bin/echo -n 0 | sudo tee cpu*/cpuidle/state1/disable	
			echo "keeping C1 enabled"	
		;;
		"c1e")
			/bin/echo -n 0 | sudo tee cpu*/cpuidle/state2/disable	
			echo "keeping C1e enabled"	
		;;
		"c3")
			/bin/echo -n 0 | sudo tee cpu*/cpuidle/state3/disable	
			echo "keeping C3 enabled"	
		;;
		"c6")
			/bin/echo -n 0 | sudo tee cpu*/cpuidle/state4/disable	
			echo "keeping C6 enabled"	
		;;
		*)
			/bin/echo -n 0 | sudo tee cpu*/cpuidle/state*/disable	
			echo "keeping all idle states enabled"
		;;
	esac
	echo "  done"
        ;;
    "-off")
    	echo "enabling all C-states"
        /bin/echo -n 0 | sudo tee cpu*/cpuidle/state*/disable
	echo "  done"
        ;;
    *)
        echo "Usage:"
        echo "  -on  enables c-state limits"
        echo "  -off disables c-state limits"
    ;;
esac
