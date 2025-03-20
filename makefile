all : 
	gcc main.c -lcurl -lcjson -o define

install :
	sudo cp define /sbin/

uninstall : 
	sudo rm /sbin/define
