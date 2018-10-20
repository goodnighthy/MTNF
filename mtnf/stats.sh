echo 'getting speed'
SPEED=`sudo nfp-hwinfo -n 0 me.speed | awk -F= '{print $2}'`

echo 'getting max'
MAX=`sudo /opt/netronome/bin/nfp-rtsym -n 0 -vl 8 _tsopt_data:0x00`
MAX=`echo $SPEED $MAX | awk -n '{val = $3 * 2^32 + $4; print (val * 16 / $1 / 1000)}'`

echo 'getting min'
MIN=`sudo /opt/netronome/bin/nfp-rtsym -n 0 -vl 8 _tsopt_data:0x08`
MIN=`echo $SPEED $MIN | awk -n '{val = $3 * 2^32 + $4; print (val * 16 / $1 / 1000)}'`

echo 'getting total'
TOTAL=`sudo /opt/netronome/bin/nfp-rtsym -n 0 -vl 8 _tsopt_data:0x18`
TOTAL=`echo $SPEED $TOTAL | awk -n '{val = $3 * 2^32 + $4; print (val * 16 / $1 / 1000)}'`

echo 'getting packets'
PACKETS=`sudo /opt/netronome/bin/nfp-rtsym -n 0 -vl 8 _tsopt_data:0x10 | awk -n '{val = $2 * 2^32 + $3; print val}'`

echo SPEED: $SPEED MHz
echo PACKETS: $PACKETS
echo MAX: $MAX ms
echo MIN: $MIN ms
if [ $PACKETS -gt 0 ]; then
  echo AVE: `echo "$TOTAL $PACKETS" | awk '{print ($1 / $2)}'` ms
fi