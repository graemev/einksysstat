#! /bin/bash

FAN=""
CPUTEMP=""

echo -e "Possible devices to use for fan speed:\n\n"
find /sys/devices/platform -name fan1_input -print0 | while IFS= read -d '' pathname
do
    echo "-------------------------------------------------------"
    echo "Found ${pathname}"

    echo "To add that to an einksysstat.conf(5), use e.g. "
    echo -e "\n          FAN(0,row, col, 24, \"${pathname}\", 9000);\n"
    echo "That would display the fan speed in Font24, starting on row 'row' column 'col'"
    echo "Normally in black, but in red if it exceeds 9000 RPM"

    echo "The output of concatenating that file is:"
    cat "${pathname}"
    echo "Decide based on the above, if this is indeed the device you want to monitor"
    echo "NB, you may need to kick off some activity, to start the fan, in order to decide"
    
done

echo -e "\n\n======================================================="

echo -e "\nPossible devices to use for CPU Temperature:\n\n"
find /sys/devices/virtual/thermal -name temp -print0 | while IFS= read -d '' pathname
do
    echo "-------------------------------------------------------"
    echo "Found ${pathname}"

    echo "To add that to an einksysstat.conf(5), use e.g. "
    echo  -e "\n          TEMP(0,row, col, 24, \"${pathname}\", 90);\n"
    echo "That would display the CPU Temperature in Font24, starting on row 'row' column 'col'"
    echo "Normally in black, but in red if it exceeds 90C"
    echo "The output of concatenating that file is:"
    cat "${pathname}"
    echo "Decide based on the above, if this is indeed the device you want to monitor"

    read temp1000 < "${pathname}"

    echo "The Above readiing, for example, indicates a CPU Temperature of $(bc <<< "scale=2; ${temp1000}/1000") Degrees Centigrade"
    


done






# find /sys/devices/virtual/thermal -name temp -print0
