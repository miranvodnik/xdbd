
-include ../makefile.defs

CPPOPTS = -g -fPIC -c -Wall
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbdsqlparser \
	-I$(PRODUCT_DIR)/xdbdmsg \
	-I$(PRODUCT_DIR)/ftpmpx \
	-I/usr/include/libxml2
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS := -lftpmpx
LDIR = -L$(PRODUCT_DIR)/lib

OBJS += \
	./XdbdAdminJob.o \
	./XdbdAnyJob.o \
	./XdbdCommon.o \
	./XdbdMessageQueue.o \
	./XdbdShmSegment.o \
	./XdbdSqlConnectJob.o \
	./XdbdSqlDisconnectJob.o \
	./XdbdSqlCreateStatementJob.o \
	./XdbdSqlDeleteStatementJob.o \
	./XdbdSqlDeleteJob.o \
	./XdbdSqlInsertJob.o \
	./XdbdSqlJob.o \
	./XdbdSqlPrepareJob.o \
	./XdbdSqlExecuteJob.o \
	./XdbdSqlExecuteDirectJob.o \
	./XdbdSqlSelectJob.o \
	./XdbdSqlUpdateJob.o \
	./XdbdWorkingThread.o \

all: libxdbdjob.so

libxdbdjob.so: $(OBJS)
	$(CXX) $(LOPTS) $(LPATH) $(OBJS) $(LIBS) -o $@
	cp $@ ../lib

$(OBJS):

clean:
	-@rm -f libxdbdobj.so $(OBJS)
	
