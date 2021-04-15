
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd-api \
	-I$(PRODUCT_DIR)/xdbd-msg \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-errinfo \

CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = -lxdbdapi
LPATH = -L../lib

OBJS = \
	./XdbdOdbcHandle.o \
	./XdbdOdbcEnvironment.o \
	./XdbdOdbcConnection.o \
	./XdbdOdbcStatement.o \
	./XdbdOdbcDescriptor.o \
	./xdbd-odbc.o 

all: libxdbdodbc.so

# Tool invocations
libxdbdodbc.so: $(OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(OBJS) $(LIBS)
	cp $@ ../lib

# Other Targets
clean:
	-@rm -f $(OBJS) libxdbdodbc.so
