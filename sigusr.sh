# Sends SIGUSRX to Writer process. X is the first argument to this script
# Ex. ./sigusr.sh 2 # Sends USR2
kill -s USR$1 $(ps aux | grep '[w]riter' | awk '{print $2}')