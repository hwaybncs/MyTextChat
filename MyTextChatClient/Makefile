objects = main.o myConnection.o mySocket.o myTextChat.o conio.o myLib.o

MyTextChat : $(objects)
	g++ -o MyTextChat $(objects)

main.o : myTextChat.h

myConnection.o : 

mySocket.o : 

conio.o :

myLib.o :

myTextChat.o : conio.h myLib.h

.PHONY : clean run
clean : 
	rm MyTextChat $(objects)

run :
	./MyTextChat
