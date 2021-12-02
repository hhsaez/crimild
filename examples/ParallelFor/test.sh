#!/bin/bash

SUCCESS=0
TOTAL=50

for i in `seq 1 $TOTAL`;
do
	echo "Test: $i"
	if ./ParallelFor > /dev/null; then
		((SUCCESS++))
	fi
done

echo "$SUCCESS / $TOTAL"

