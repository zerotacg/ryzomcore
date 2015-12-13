#!/bin/sh
### BEGIN INIT INFO
# Provides:          backup-service
# Required-Start:    $local_fs $remote_fs $network $syslog $named
# Required-Stop:     $local_fs $remote_fs $network $syslog $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: Start/stop
# Description:       Start/stop
#  This script will start
### END INIT INFO

DAEMON=/usr/local/sbin/ryzom_backup_service
RunningDirectory=/home/vagrant/backup_service
ConfigDirectory=/home/vagrant/backup_service
LogDirectory=/home/vagrant/backup_service
PORT=49990
PARAMS="--writepid -P${PORT} -A${RunningDirectory}"
PIDFILE=${RunningDirectory}/pid.state

case "$1" in
    start)
        start-stop-daemon --start --background --oknodo --pidfile ${PIDFILE} --chdir ${RunningDirectory} --startas ${DAEMON} -- ${PARAMS}
        ;;
    stop)
        start-stop-daemon --stop --oknodo --pidfile ${PIDFILE}
        ;;
    status)
        start-stop-daemon --status --oknodo --pidfile ${PIDFILE}
        exit $?
        ;;
    *)
        echo "Usage: $0 {start|stop|status}" >&2
        exit 3
        ;;
esac

exit 0

# vim: syntax=sh ts=4 sw=4 sts=4 sr noet
