################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../xdbd-gen/xdbd-gen.o 

CPP_SRCS += \
../xdbd-gen/xdbd-gen.cpp 

OBJS += \
./xdbd-gen/xdbd-gen.o 

CPP_DEPS += \
./xdbd-gen/xdbd-gen.d 


# Each subdirectory must supply rules for building sources it contributes
xdbd-gen/%.o: ../xdbd-gen/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/miran/Eclipse/workspace/cdcp.cs6114bx.009/ftpmpx" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdsqlparser" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdapi" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdmsg" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbd-include" -I/usr/include/libxml2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


