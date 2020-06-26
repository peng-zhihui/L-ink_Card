#!/bin/sh
# Local tests looper until fail. CeDeROM 20170815
# Runs all tests on all supported Boards/Targets on provided environment.
LOGDIR=../test/out/
TGTDIR=../bin/target

echo "ARE WE INSIDE VALID VIRTUALENV READY FOR TESTING?"
echo "LOGDIR: $LOGDIR"
echo "TGTDIR: $TGTDIR"
echo "Press Ctrl+C to Abort, Return to Start.."
read

while [ 1 ]; do
        TIME=`date +%s`
        SAY_ITERATION="STARTING DAPLINK TEST ITERATION $TIME"
        echo $SAY_ITERATION
        if say "" ; then say "$SAY_ITERATION" ; fi
        python ../test/run_test.py --targetdir $TGTDIR --testdl --loadbl --verbose All --logdir "$LOGDIR"/$TIME
        RES=$?
        if [ $RES -ne 0 ]; then
                SAY_ERROR="DAPLINK TESTING ERROR, TEST $TIME FAILED MISERABLY WITH STATUS $RES, EJECTING!"
                echo "$SAY_ERROR"
                if say "" ; then say -v "bad news" "$SAY_ERROR" ; fi
                if [ -f "$LOGDIR"/summary.txt ]; then cat "$LOGDIR"/summary.txt; fi
                break
        fi
done
