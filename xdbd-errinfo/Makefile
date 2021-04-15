
-include ../makefile.defs

CFLAGS = -fPIC -Wall -c -g $(UNIX_FLAGS)
IDIR = \
	-I$(PRODUCT_DIR)/xdbd-include \

CXXFLAGS = $(DEF_FLAGS) $(CFLAGS) $(IDIR)
LDIR = \
	-L$(PRODUCT_DIR)/lib \

LOPTS = -shared
LIBS = 

OBJS = \
	XdbdErrInfo.o \
	XdbdErrApi.o \

library: libxdbderrinfo.so

-include .deps/*.d

libxdbderrinfo.so: $(OBJS)
	$(CXX) $(LOPTS) $(LDIR) $(LIBS) $(OBJS) -o $@
	-@cp $@ ../lib

$(OBJS):

clean:
	-@rm -rf libxdbderrinfo.so $(OBJS)

