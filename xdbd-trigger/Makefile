
-include ../makefile.defs

COPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
CPPOPTS = -g -Wall -c -fmessage-length=0 -fPIC $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-msg \
	-I/usr/include/libxml2
CFLAGS = $(DEF_FLAGS) $(COPTS) $(IDIR)
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS = 
LPATH = -L$(PRODUCT_DIR)/lib

OBJS = \
	./xdbd-trigger.o 

all: libtrigger.so

# Tool invocations
libtrigger.so: $(OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(OBJS) $(LIBS)
	cp $@ ../lib

# Other Targets
clean:
	-@rm -f $(OBJS) libtrigger.so
