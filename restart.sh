#!/bin/sh

stop() {
	kill $1
}

start() {
	nohup ./Gateway -c Gateway.cfg &
	pid=$!
	echo "start ok, pid: " $pid
}

CMD="./Gateway -c Gateway.cfg"

exe=Gateway           # 主程序
cfg='Gateway.cfg'

ps ax | grep "$exe " | grep "$cfg" | grep -v grep >/dev/null 2>&1
if [ $? -ne 0 ]
then
    echo "starting"
    start
else
    pid=`ps ax | grep "$exe " | grep "$cfg" | grep -v grep | awk '{print $1}'`
    echo "stopping, pid: " $pid
    stop $pid
fi

