
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-sql-parser \
	-I$(PRODUCT_DIR)/xdbd-msg \
	-I$(PRODUCT_DIR)/xdbd-job \
	-I$(PRODUCT_DIR)/xdbd-mpx \
	-I$(PRODUCT_DIR)/xdbd-errinfo \
	-I$(PRODUCT_DIR)/xdbd-ham \
	-I/usr/include/libxml2
CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = -lrt -lxdbdsqlparser -lxdbdodbc -lxdbdapi -lxdbdmsg -lxdbdmpx -lxdbderrinfo -luuid -lpthread `xml2-config --libs`
LPATH = -L$(PRODUCT_DIR)/lib

OBJS += \
	./XdbdController.o \
	./XdbdLocalClient.o \
	./XdbdMqClient.o \
	./XdbdNetworkClient.o \
	./XdbdWorker.o \
	./XdbdWorkingClient.o \
	./XdbdClientHandler.o \
	./XdbdClientDriver.o \
	./XdbdAnyJob.o \
	./XdbdShmSegment.o \
	./XdbdSqlConnectJob.o \
	./XdbdSqlDisconnectJob.o \
	./XdbdSqlCreateStatementJob.o \
	./XdbdSqlDeleteStatementJob.o \
	./XdbdSqlJob.o \
	./XdbdSqlPrepareJob.o \
	./XdbdSqlExecuteJob.o \
	./XdbdSqlExecuteDirectJob.o \

all: libxdbdcore.so

libxdbdcore.so: $(OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(OBJS) $(LIBS)
	cp $@ ../lib

$(OBJS):

clean:
	-@rm -f $(OBJS) libxdbdcore.so
	
