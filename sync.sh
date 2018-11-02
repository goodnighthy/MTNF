#!/usr/bin/env bash

#scp -r mtnf amax@202.112.237.33:/home/amax/hy/MultiTenantNF
#echo "syncing to 65"
#scp -r mtnf ubuntu@202.112.237.65:/home/ubuntu/MTNF/
echo "syncing to 51"
scp -r mtnf ubuntu@101.6.30.51:/home/ubuntu/

echo "syncing finished"
