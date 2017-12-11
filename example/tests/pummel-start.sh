#!/bin/bash
SUBPROCESSES=$1
SUBTHREADS=$2
MESSAGE_COUNT=$3

TIMEOUT_LIMIT=10m

# 100 10 1000 => Failed to determine seats of user "1029": Too many open files

echo "pummel [subprocesses=10] [subthreads=10] [messagecount=100]"
# Notes: In terms of wall clock time, it's been taking 5 seconds to run 2 concurrent 
# tests on my desktop. And,
#
# tests : seconds
#     1 : 4
#     2 : 5
#    10 : 20
#    12 : 26
# --errors here--
#    20 : 20
#    25 : 20
#    50 : 20
#
if [ "$SUBPROCESSES" == "" ]; then
	SUBPROCESSES=10;
fi

if [ "$SUBTHREADS" == "" ]; then
	SUBTHREADS=10;
fi

if [ "$MESSAGE_COUNT" == "" ]; then
	MESSAGE_COUNT=100;
fi

mkdir logs 2>/dev/null

DATESTAMP=`date +%F_%H%M%S`
LOGFILE=logs/pummel-results-$DATESTAMP
LOGFILE_ERROR=logs/pummel-results-error-$DATESTAMP
LOGFILE_PROCESSES=logs/pummel-results-processes-$DATESTAMP

echo "Creating $SUBPROCESSES processes, each running $SUBTHREADS threads, generating $MESSAGE_COUNT messages."

echo "" > $LOGFILE
echo "" > $LOGFILE_ERROR

PID_LIST=""
for i in `seq 1 $SUBPROCESSES`;
do
#	timeout -k $TIMEOUT_LIMIT $TIMEOUT_LIMIT ./example/pummel-client $i $MESSAGE_COUNT $SUBTHREADS >> $LOGFILE 2>>$LOGFILE_ERROR & 
	( ./example/pummel-client $i $MESSAGE_COUNT $SUBTHREADS >> $LOGFILE 2>>$LOGFILE_ERROR ) & 
    PID_LIST="$PID_LIST $!"
done

echo "Results will be in " $LOGFILE
echo "Errors will be in " $LOGFILE_ERROR

echo "Waiting on processes... " $PID_LIST

wait $PID_LIST 2>LOGFILE_PROCESSES

# We only get a success if every message in a specific thread was successful
COUNT=`grep SUCCESS $LOGFILE | wc -l`

EXPECTING=$(($SUBPROCESSES * $SUBTHREADS))
if [ $COUNT == $EXPECTING ]; then
	echo "Everything worked! Let's move on..."
else
	echo "FAILURE. There were $COUNT successes out of $EXPECTING total"
	echo "Here's the log"

	grep -v SUCCESS $LOGFILE | less
fi


