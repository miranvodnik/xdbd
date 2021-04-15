
-include ../makefile.defs

CPPOPTS = -g -fPIC -c -Wall -DSDL_MAPPING=M_TP $(UNIX_FLAGS)
IDIR = -I. \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/dbadt \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/gen \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/include \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/required/include \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/required/include/g2_rtlib \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/required/include/scprint \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/required/include/solid \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/include/trc \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/g2_rtlib \
	-I/home/miran/CS_APPL_STD-5.51.0-0_0/rte/system/notify_mng \
	-I$(PRODUCT_DIR)/xdbd-include \

CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR) -DSS_UNIX
LOPTS = -shared
LIBS := 
LDIR = -L$(PRODUCT_DIR)/lib

OBJS += \
	nmFunc_h248_intf_profile_sdl.xml.o \
	nmFunc_h248_nonstd_prf_sdl.xml.o \
	clip_string.o \

#	nmFunc_statistics_group_sdl.xml.o \
#	nmFunc_sys_interface_sdl.xml.o \
#	nmFunc_vxml_config_sdl.xml.o \
#	nmFunc_vxml_server_sdl.xml.o \
#	nmFunc_vxml_trace_sdl.xml.o \

all: libsdltrigger.so

# Tool invocations
libsdltrigger.so: $(OBJS) $(ADD_OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(OBJS) $(ADD_OBJS)
	cp $@ ../lib

$(OBJS):

clean:
	-@rm -f libsdltrigger.so $(OBJS) $(ADD_OBJS)
	
