
#CCFLAGS+= -I../include

default:	all
all:    
	cxxtest/bin/cxxtestgen --error-printer -o network_tests_main.cpp network_tests.h && scons
clean:
	scons -c
install:
	scons install

#LDFLAGS+= /usr/local/lib/libbson.a
#LDFLAGS+= /usr/local/lib/libmongoc.a

#all: 
#	g++	main.cpp db_interactions.cpp -o varqs_updater	$^ $(LDFLAGS)

#clean:
#	rm -rf *.o varqs_updater

#LDLIBS=-libmongoc.a



