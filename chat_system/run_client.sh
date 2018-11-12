#!/bin/bash

ROOT_PATH=$(pwd)
CONF=${ROOT_PATH}/conf/client.conf
BIN_PATH=${ROOT_PATH}/bin
BIN=udp_client

ip=''
port=''

[[ -f ${CONF} ]] && {
	ip=$(grep "DST_IP" ${CONF}| awk -F: '{print $2}'| tr -d ' ')
	port=$(grep "DST_PORT" ${CONF}| awk -F: '{print $2}'| tr -d ' ')
	pushd ${BIN_PATH}
	./${BIN} -i "${ip}" -p "${port}"
	popd
}
