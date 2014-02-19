
kill -9 `ps | grep $1 |grep -v grep | awk '{print $1}'`
