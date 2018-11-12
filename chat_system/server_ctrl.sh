#!/bin/bash

WORK_PATH=$(pwd)
BIN=${WORK_PATH}/bin
CONF=${WORK_PATH}/conf
LOG=${WORK_PATH}/log

BIN_NAME=udp_serverd
CONF_NAME=server.conf
LOG_NAME=server.log

proc=$(basename $0)
function usage(){
	printf "%s\e[43;46;1m%s\e[0m%s\n" "Usage: ${proc} [" "start(-s) | stop(-t) | restart(-r) | check(-c)" "]"
}

function print_log(){
	local _msg="$1"
	local _level="$2"
	local _data=$(date +%Y_%m_%d_%H/%M/%S)
	echo ${_data} : [${_level} ] : [ ${_msg} ] >>${LOG}/${LOG_NAME} 2>/dev/null
}

function server_status(){
	local _is_exist=$(pidof ${BIN_NAME})
	echo ${_is_exist}
}

function server_start()
{
	ret=$(server_status)
	if [ -z "${ret}" ];then
	    pushd ${BIN}
	    ./${BIN_NAME}
	    popd
	else
		print_log "${BIN_NAME} is already running, pid is ${ret}" "Notice"
	fi
	ret=$(server_status)
	if [ -z "${ret}" ];then
		echo "start failed"
		print_log "${BIN_NAME} start failed" "Fatal"
	else
		echo "start success"
		print_log "${BIN_NAME} start success, pid is ${ret}" "Notice"
	fi
}

function server_stop()
{
	local _ret=$(server_status)
	if [ -z "${_ret}" ];then
		echo "${BIN_NAME} does not exist"
		print_log "stop ${BIN_NAME} does not exist" "Notice"
		return
	else
		kill -9 ${_ret}
	fi

	local _ret=$(server_status)
	if [ -z "${_ret}" ];then
		echo "stop success"
		print_log "stop ${BIN_NAME} success" "Notice"
	else
		echo "stop failed"
		print_log "stop ${BIN_NAME} failed" "Fatal"
	fi
}

function server_restart()
{
	server_stop
	server_start
}

function server_check()
{
	local _ret=$(server_status)
	if [ -z "${_ret}" ];then
		echo "${BIN_NAME} does not exist"
	else
		echo "${BIN_NAME} is running , pid is : ${_ret}"
	fi
}

[[ $# -ne 1 ]] && {
	print_log "your enter message is bad" "Warning"
	usage
	exit 1
}

case $1 in
	'start'|'-s' )
		server_start
	;;
	'stop'|'-t' )
		server_stop
	;;
	'restart'|'-r' )
		server_restart
	;;
	'check'|'-c' )
		server_check
	;;
	* )
		usage
	;;
esac

