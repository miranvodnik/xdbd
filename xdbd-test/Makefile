
-include ../makefile.defs

CPPOPTS = -g -fPIC -c -Wall $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-sql-parser \
	-I$(PRODUCT_DIR)/xdbd-msg \
	-I$(PRODUCT_DIR)/xdbd-api \
	-I$(PRODUCT_DIR)/xdbd-mpx \
	-I$(PRODUCT_DIR)/xdbd-core \
	-I$(PRODUCT_DIR)/xdbd-errinfo \
	-I/usr/include/libxml2
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS := -lrt -lxdbdsqlparser -lxdbdcore -lxdbdodbc -lxdbdapi -lxdbdmsg -lxdbdmpx -lxdbderrinfo -lxdbdstat -luuid -lpthread `xml2-config --libs`
LDIR = -L$(PRODUCT_DIR)/lib

OBJS += \
./xdbd-test.o \

OBJS-CONN += \
./xdbd-conn-test.o \

all: xdbd-test xdbd-conn-test

xdbd-test: $(OBJS)
	$(CXX) $(OBJS) $(LDIR) $(LIBS) -o $@
	cp $@ ../bin

xdbd-conn-test: $(OBJS-CONN)
	$(CXX) $(OBJS-CONN) $(LDIR) $(LIBS) -o $@
	cp $@ ../bin

$(OBJS):

$(OBJS-CONN):

clean:
	-@rm -f xdbd-test $(OBJS)
	-@rm -f xdbd-conn-test $(OBJS-CONN)
	
