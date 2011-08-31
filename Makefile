smatool: smatool.c
	gcc -L/usr/lib64/mysql -lbluetooth -lcurl -lmysqlclient -g -o smatool smatool.c
