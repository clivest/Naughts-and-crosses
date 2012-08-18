#!/bin/bash

#directory of db
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#date to append to backup
DATE=$(date +"%Y_%m_%d_%H_%M")

echo '.dump' | sqlite3 "$DIR/naughts_and_crosses.db" >"$DIR/dump_$DATE.sql"
