#!/usr/bin/env bash

function usage {
        echo "$0 CPU-LIST PORTMASK [-t TENANT_NUMBER] [-T TIMER_PERIOD]"
        # this works well on our 2x6-core nodes
        echo "$0 0,1,2,3,4 f --> cores 0, 1, 2 and 6 with ports 0 and 1"
        echo -e "\tCores will be used as follows in numerical order:"
        echo -e "\t\tWORKER thread, ..., MASTER thread"
        exit 1
}

cpu=$1
ports=$2

SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

shift 2

if [ -z $ports ]
then
    usage
fi

while getopts "t:T:" opt; do
    case $opt in
        t) tenant_number="-t $OPTARG";;
        T) time_period="-T $OPTARG";;
        \?) echo "Unknown option -$OPTARG" && usage
            ;;
    esac
done

sudo rm -rf /mnt/huge/rtemap_*
sudo $SCRIPTPATH/build/app/mtnf -l $cpu -n 4 --proc-type=primary -- -p ${ports} ${tenant_number} ${time_period}