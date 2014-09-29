################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TestVRep.cpp 

C_SRCS += \
../extApi.c \
../extApiPlatform.c 

OBJS += \
./TestVRep.o \
./extApi.o \
./extApiPlatform.o 

C_DEPS += \
./extApi.d \
./extApiPlatform.d 

CPP_DEPS += \
./TestVRep.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -I/home/kcastlet/Desktop/V-REP_PRO_EDU_V3_1_2_64_Linux/programming/remoteApi -I/home/kcastlet/Desktop/V-REP_PRO_EDU_V3_1_2_64_Linux/programming/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -I/home/kcastlet/Desktop/V-REP_PRO_EDU_V3_1_2_64_Linux/programming/remoteApi -I/home/kcastlet/Desktop/V-REP_PRO_EDU_V3_1_2_64_Linux/programming/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


