#1/bin/sh
cd /sys/devices/system/cpu || exit
case "$1" in
    "-on")
     	/bin/echo -n 1 | sudo tee cpu*/online
        ;;
    "-off")
     	/bin/echo -n 0 | sudo tee "cpu"{4..11}"/online"
        ;;
    *)
        echo "Usage:"
        echo "  -on  enables all cpus on s 0"
        echo "  -off disables all but the first cpus on s 0"
    ;;
esac
