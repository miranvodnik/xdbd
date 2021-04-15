
-include ../makefile.defs

CPPOPTS = -g -fPIC -c -Wall -DSDL_MAPPING=M_TP $(UNIX_FLAGS)
IDIR = -I. \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/dbadt \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/gen \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/include \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/required/include \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/required/include/solid \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/include/trc \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/g2_rtlib \
	-I/home/miran/CS_APPL_STD-5.27.0-0_TEST/rte/system/notify_mng \
	-I$(PRODUCT_DIR)/xdbd-include \

CPPFLAGS = $(DEF_FLAGS) $(CPPOPTS) $(IDIR) -DSS_UNIX
LOPTS = -shared
LIBS := 
LDIR = -L$(PRODUCT_DIR)/lib

MQ_OBJS += \
	nmFunc_basic_filter_mq.xml.o \
	nmFunc_filter_grp_member_mq.xml.o \
	nmFunc_filter_mq.xml.o \
	nmFunc_ip_route_mq.xml.o \
	nmFunc_protocol_qos_mq.xml.o \
	nmFunc_syslog_client_mq.xml.o \
	nmStructureDefinition.o \
	clip_string.o \

SDL_OBJS += \
	nmFunc_coco_net_addr_sdl.xml.o \
	nmFunc_h248_interface_sdl.xml.o \
	nmFunc_h248_intf_profile_sdl.xml.o \
	nmFunc_h248_nonstd_prf_sdl.xml.o \
	nmFunc_protocol_qos_sdl.xml.o \
	nmFunc_request_exe_sdl.xml.o \
	nmFunc_vxml_config_sdl.xml.o \
	nmStructureDefinition.o \
	clip_string.o \

#	nmFunc_node_mq.xml.o \
#	nmFunc_ext_dsp_options_sdl.xml.o \
#	nmFunc_node_sdl.xml.o \
#	nmFunc_sys_interface_sdl.xml.o \
#	nmFunc_vxml_server_sdl.xml.o \
#	nmFunc_vxml_trace_sdl.xml.o \

all: libmqtrigger.so libsdltrigger.so

# Tool invocations
libmqtrigger.so: $(MQ_OBJS) $(ADD_OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(MQ_OBJS) $(ADD_OBJS)
	cp $@ ../lib

libsdltrigger.so: $(SDL_OBJS) $(ADD_OBJS) $(USER_OBJS)
	$(CXX) $(LOPTS) $(LPATH) -o $@ $(SDL_OBJS) $(ADD_OBJS)
	cp $@ ../lib

$(OBJS):

clean:
	-@rm -f libmqtrigger.so libsdltrigger.so $(MQ_OBJS) $(SDL_OBJS) $(ADD_OBJS)
	
