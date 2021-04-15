
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -fPIC -c -Wall -Wno-deprecated  $(UNIX_FLAGS) -D_LARGE_FILE_API `xml2-config --cflags`
IDIR = -I.
CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = 
LPATH = 

OBJS = ./XdbdMessages_xdr.o 

all: libxdbdmsg.so

XdbdMessages_xdr.o: XdbdMessages_xdr.c
	$(CC) $(CFLAGS) $(NO_WARNINGS) XdbdMessages_xdr.c

XdbdMessages_xdr.c XdbdMessages.h: XdbdMessages.x
	rpcgen XdbdMessages.x

libxdbdmsg.so: $(OBJS)
	$(CXX) $(LOPTS) -o $@ $(OBJS) $(USER_OBJS) $(LIBS)
	cp $@ ../lib

$(OBJS):

clean:
	-@rm -f libxdbdmsg.so $(OBJS) \
	XdbdMessages.h XdbdMessages_clnt.c XdbdMessages_svc.c XdbdMessages_xdr.c
	
