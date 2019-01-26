#!/bin/bash
#
# ECEN 5013 - HW1
# @author: Brian Ibeling
# @date: 1/20/2019
# 
# Script to display system and user info
# 

echo
echo "User Info:"
id

echo
echo "OS Type/Brand | Distribution | Version:"
lsb_release -a

echo
echo "Kernel Version:"
uname -r

echo
echo "Kernel gcc Version Build | Kernel Build time:"
cat /proc/version

echo
echo "System Architecture Information:"
dpkg --print-architecture
arch

echo
echo "Information on File System Memory:"
free -h

echo
