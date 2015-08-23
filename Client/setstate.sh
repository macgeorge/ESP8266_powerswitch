#!/bin/bash
arguments=$#
max_attempts=10	#maximum attempts to try to connect
timeout=2		#initial timeout in seconds
attempt=0
if [ "$arguments" -ne "2" ] 
then echo "usage: setstate <ip> <state>"
exit 1
fi

dev=$1
state=$2
url="http://"$dev"/gpio/"$state

while (($attempt < $max_attempts))
do
	curl -s --max-time 1 $url > /dev/null
	exitCode=$?
	if [[ $exitCode == 0 ]]
	then
		break
	fi
	
	logger -p error "[WARNING] Wireless $ip not responding, retrying in $timeout"
	echo "Failure! Retrying in $timeout.." 1>&2
    sleep $timeout
    attempt=$(( attempt + 1 ))
    timeout=$(( timeout * 2 ))
done

if [[ $exitCode != 0 ]]
  then
   	logger -p error "[ERROR] Wireless $ip not responding"
    echo "Maximum attempts reached trying to connect ($@)" 1>&2
fi

exit $exitCode
