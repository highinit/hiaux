
#CCFLAGS+= -I../include

default:	all
all:    
	/usr/local/bin/cxxtest-4.2/bin/cxxtestgen --error-printer -o stubgen_test.cpp call_test.h fault_test.h sendch_test.h stubgen_test.h & /usr/local/bin/scons
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



