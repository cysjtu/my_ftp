
all:ftpclient
	echo "finish"
	
ftpclient:main.o  myFtp.o cmdparser.h defines.h myFtp.h
	g++  -o ftpclient main.o myFtp.o
	
main.o:main.cpp cmdparser.h defines.h myFtp.h
	g++ -c main.cpp
	
myFtp.o:myFtp.cpp cmdparser.h defines.h myFtp.h
	g++ -c myFtp.cpp


clean:
	rm *.o
	

