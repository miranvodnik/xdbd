
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
IDIR = -I. \
	-I/usr/include/libxml2 \
	-I$(PRODUCT_DIR)/xdbd \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-sql-parser \

CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = 
LPATH = 

OBJS = \
	./XdbdNMGenAllFields.o \
	./XdbdNMGenAppl.o \
	./XdbdNMGenChangeMask.o \
	./XdbdNMGenCodeText.o \
	./XdbdNMGenConditions.o \
	./XdbdNMGenEvents.o \
	./XdbdNMGenEventsDelete.o \
	./XdbdNMGenEventsInsert.o \
	./XdbdNMGenEventsUpdate.o \
	./XdbdNMGenField.o \
	./XdbdNMGenIncludes.o \
	./XdbdNMGenMQNotification.o \
	./XdbdNMGenMsgsnd.o \
	./XdbdNMGenNotification.o \
	./XdbdNMGenSDLNotification.o \
	./XdbdNMGenStruct.o \
	./XdbdNMGenStructVariables.o \
	./XdbdNMGenUnit.o \
	./XdbdNMGenBase.o \
	./XdbdTableInfo.o \
	./XdbdColumnInfo.o \
	./XdbdDataBase.o \
	./XdbdNMGenSystemTables.o \

.cpp: .h

all: libxdbdnmgen.so

# Tool invocations
libxdbdnmgen.so: $(OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(OBJS) $(LIBS)
	cp $@ ../lib

# Other Targets
clean:
	-@rm -f $(OBJS) libxdbdnmgen.so
