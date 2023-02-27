#!/bin/bash

input=$1
xxd -p $input > bootsector.hex
