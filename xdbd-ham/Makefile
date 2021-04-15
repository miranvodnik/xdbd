
-include ../makefile.defs

CFLAGS = -fPIC -Wall -c -g $(UNIX_FLAGS)
IDIR = \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-mpx \
	-I$(PRODUCT_DIR)/xdbd-errinfo \

CXXFLAGS = $(DEF_FLAGS) $(CFLAGS) $(IDIR)
LDIR = \
	-L$(PRODUCT_DIR)/lib \

LOPTS = -shared
LIBS = 

OBJS = \
	XdbdHamThread.o \

library: libxdbdham.so

-include .deps/*.d

libxdbdham.so: $(OBJS)
	$(CXX) $(LOPTS) $(LDIR) $(LIBS) $(OBJS) -o $@
	-@cp $@ ../lib

$(OBJS):

clean:
	-@rm -rf libxdbdham.so $(OBJS)

