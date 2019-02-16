
TARGETS=cmd10001 cmd11000 cmd10003 cmd12010

all: $(TARGETS)

CXX=g++
PUBLIC_OBJS=MyMD5.o

$(TARGETS): % : $(PUBLIC_OBJS) %.o
	$(CXX) $^ -o $@
	@echo ------------ Make $@ OK! ------------
	@echo

 
.PHONY:clean
clean:
	rm -f *.o $(TARGETS)
