
-include ../makefile.defs

CPPOPTS = -g -fPIC -c -Wall $(UNIX_FLAGS)
IDIR = -I. \
	-I$(PRODUCT_DIR)/xdbd-include \
	-I$(PRODUCT_DIR)/xdbd-sql-parser \
	-I$(PRODUCT_DIR)/xdbd-msg \
	-I$(PRODUCT_DIR)/xdbd-job \
	-I$(PRODUCT_DIR)/xdbd-mpx \
	-I$(PRODUCT_DIR)/xdbd-core \
	-I$(PRODUCT_DIR)/xdbd-errinfo \
	-I$(PRODUCT_DIR)/xdbd-ham \
	-I/usr/include/libxml2
CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR)
LOPTS = -shared
LIBS := -lrt -lxdbdsqlparser -lxdbdcore -lxdbdodbc -lxdbdapi -lxdbdmsg -lxdbdstat -lxdbdmpx -lxdbderrinfo -luuid -lpthread `xml2-config --libs`
LDIR = -L$(PRODUCT_DIR)/lib

OBJS += \
	./xdbd-main.o 

all: xdbd

xdbd: $(OBJS)
	$(CXX) $(OBJS) $(LDIR) $(LIBS) -o $@
	cp $@ ../bin
	cp $@ ../bin/xdbd-client
	cp $@ ../bin/xdbd-worker

$(OBJS):

clean:
	-@rm -f xdbd $(OBJS)
	
