#!/bin/bash

./Httpd 0 8080 #&
id=$?
sleep 10
kill -9 $id
