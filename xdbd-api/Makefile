	
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-errinfo \
	-I$(PRODUCT_DIR)/xdbd-msg
CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = -lxdbdmsg -lxdbderrinfo -lrt -luuid
LPATH = -L$(PRODUCT_DIR)/lib

OBJS = \
	./XdbdApi.o \
	./XdbdConnection.o \
	./XdbdStatement.o 

all: libxdbdapi.so

# Tool invocations
libxdbdapi.so: $(OBJS) $(USER_OBJS)
	g++ $(LOPTS) $(LPATH) -o $@ $(OBJS) $(USER_OBJS) $(LIBS)
	cp $@ ../lib

# Other Targets
clean:
	-@rm -f $(OBJS) libxdbdapi.so
