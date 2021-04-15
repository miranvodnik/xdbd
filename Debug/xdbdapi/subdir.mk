################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../xdbdapi/XdbdApi.o \
../xdbdapi/XdbdConnection.o \
../xdbdapi/XdbdStatement.o 

CPP_SRCS += \
../xdbdapi/XdbdApi.cpp \
../xdbdapi/XdbdConnection.cpp \
../xdbdapi/XdbdStatement.cpp 

OBJS += \
./xdbdapi/XdbdApi.o \
./xdbdapi/XdbdConnection.o \
./xdbdapi/XdbdStatement.o 

CPP_DEPS += \
./xdbdapi/XdbdApi.d \
./xdbdapi/XdbdConnection.d \
./xdbdapi/XdbdStatement.d 


# Each subdirectory must supply rules for building sources it contributes
xdbdapi/%.o: ../xdbdapi/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/miran/Eclipse/workspace/cdcp.cs6114bx.009/ftpmpx" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdsqlparser" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdapi" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdmsg" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbd-include" -I/usr/include/libxml2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


