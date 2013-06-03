
#CCFLAGS+= -I../include

default:	all
all:    
#	/usr/local/bin/thrift --gen cpp hw.thrift & cp gen-cpp/HdividerWatcherRPC_server.skeleton.cpp HdividerWatcherRPC_server.cpp & 
	/usr/local/bin/scons
clean:
	/usr/local/bin/scons -c
install:
	/usr/local/bin/scons install

#LDFLAGS+= /usr/local/lib/libbson.a
#LDFLAGS+= /usr/local/lib/libmongoc.a

#all: 
#	g++	main.cpp db_interactions.cpp -o varqs_updater	$^ $(LDFLAGS)

#clean:
#	rm -rf *.o varqs_updater

#LDLIBS=-libmongoc.a



