################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../xdbd-odbc/XdbdOdbcConnection.o \
../xdbd-odbc/XdbdOdbcDescriptor.o \
../xdbd-odbc/XdbdOdbcEnvironment.o \
../xdbd-odbc/XdbdOdbcHandle.o \
../xdbd-odbc/XdbdOdbcStatement.o \
../xdbd-odbc/xdbd-odbc.o 

CPP_SRCS += \
../xdbd-odbc/XdbdOdbcConnection.cpp \
../xdbd-odbc/XdbdOdbcDescriptor.cpp \
../xdbd-odbc/XdbdOdbcEnvironment.cpp \
../xdbd-odbc/XdbdOdbcHandle.cpp \
../xdbd-odbc/XdbdOdbcStatement.cpp \
../xdbd-odbc/xdbd-odbc.cpp 

OBJS += \
./xdbd-odbc/XdbdOdbcConnection.o \
./xdbd-odbc/XdbdOdbcDescriptor.o \
./xdbd-odbc/XdbdOdbcEnvironment.o \
./xdbd-odbc/XdbdOdbcHandle.o \
./xdbd-odbc/XdbdOdbcStatement.o \
./xdbd-odbc/xdbd-odbc.o 

CPP_DEPS += \
./xdbd-odbc/XdbdOdbcConnection.d \
./xdbd-odbc/XdbdOdbcDescriptor.d \
./xdbd-odbc/XdbdOdbcEnvironment.d \
./xdbd-odbc/XdbdOdbcHandle.d \
./xdbd-odbc/XdbdOdbcStatement.d \
./xdbd-odbc/xdbd-odbc.d 


# Each subdirectory must supply rules for building sources it contributes
xdbd-odbc/%.o: ../xdbd-odbc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/miran/Eclipse/workspace/cdcp.cs6114bx.009/ftpmpx" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdsqlparser" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdapi" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdmsg" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbd-include" -I/usr/include/libxml2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


