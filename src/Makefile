
TARGETS=Gateway
TESTS=test
ALL: $(TARGETS) $(TESTS)

#
#   注意要先执行 slice2cpp EAS.ice
#

CXX=g++
PUBLIC_OBJS=INIReader.o ini.o SignalDeal.o common.o \
    Net.o NetWorker.o \
    HttpClient.o \
    Message.o $(patsubst %.cpp, %.o, $(wildcard Message_*.cpp))
CPPFLAGS=-I. -I/usr/local/include
LIBS_PATH=-lpthread -L/usr/lib64 -L/usr/local/lib -lglog -lcurl

$(TARGETS): % : $(PUBLIC_OBJS) %.o
	$(CXX) $(LIBS_PATH) $^ -o $@
	@echo ------------ Make $@ OK! ------------
	@echo

$(TESTS): % : common.o %.o
	$(CXX) $^ -o $@
	@echo ------------ Make $@ OK! ------------
	@echo

.PHONY:clean
clean:
	rm -f *.o $(TARGETS) $(TESTS)


