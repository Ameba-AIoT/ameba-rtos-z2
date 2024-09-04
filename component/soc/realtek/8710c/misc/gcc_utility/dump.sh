#!/bin/sh

#===============================================================================
ELFNAME=$1
echo $ELFNAME

STARTADDRESS=$2
echo $STARTADDRESS

ENDADDRESS=$3
echo $ENDADDRESS

#===============================================================================

echo "dump binary memory $ELFNAME $STARTADDRESS $ENDADDRESS" > dump.gdb
exit