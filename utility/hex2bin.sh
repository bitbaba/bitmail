#!/bin/bash

echo -n -e $(echo $(cat) | sed -e 's/\([a-fA-F0-9]\{2\}\)/\\x\1/g')
