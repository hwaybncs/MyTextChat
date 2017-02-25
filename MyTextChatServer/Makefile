objects = main.o myConnection.o mySocket.o myTextChat.o myLib.o

MyTextChat : $(objects)
	g++ -lpthread -L/usr/lib64/mysql -lmysqlclient  -o MyTextChat $(objects)

main.o : myConnection.h myTextChat.h

myConnection.o : 

mySocket.o : 

myTextChat.o : myPair.h

.PHONY : clean run
clean : 
	rm MyTextChat $(objects)

run :
	./MyTextChat
