################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

ORDERED_OBJS += \
"./CC2650STK.obj" \
"./buzzer.obj" \
"./ccfg.obj" \
"./kuva.obj" \
"./main.obj" \
"./sensors/bmp280.obj" \
"./sensors/hdc1000.obj" \
"./sensors/mpu9250.obj" \
"./sensors/opt3001.obj" \
"./sensors/tmp007.obj" \
"./wireless/CWC_CC2650_154Drv.obj" \
"./wireless/CWC_IntegrTest.obj" \
"./wireless/ERRORS.obj" \
"./wireless/comm_lib.obj" \
"../CC2650STK.cmd" \
$(GEN_CMDS__FLAG) \
-l"C:/ti/tirtos_cc13xx_cc26xx_2_20_00_06/products/cc26xxware_2_24_02_17202/driverlib/bin/ccs/driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include sensors/subdir_vars.mk
-include wireless/subdir_vars.mk
-include subdir_rules.mk
-include sensors/subdir_rules.mk
-include wireless/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
JTKJ_labra.out \

EXE_OUTPUTS__QUOTED += \
"JTKJ_labra.out" \

BIN_OUTPUTS += \
JTKJ_labra.hex \

BIN_OUTPUTS__QUOTED += \
"JTKJ_labra.hex" \


# All Target
all: JTKJ_labra.out

# Tool invocations
JTKJ_labra.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me -O2 -g --define=ccs --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi -z -m"JTKJ_labra.map" --stack_size=256 --heap_size=0 -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --reread_libs --display_error_number --warn_sections --diag_wrap=off --xml_link_info="JTKJ_labra_linkInfo.xml" --rom_model -o "JTKJ_labra.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

JTKJ_labra.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armhex" --memwidth=8 --romwidth=8 --intel -o "JTKJ_labra.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "CC2650STK.d" "buzzer.d" "ccfg.d" "kuva.d" "main.d" "sensors\bmp280.d" "sensors\hdc1000.d" "sensors\mpu9250.d" "sensors\opt3001.d" "sensors\tmp007.d" "wireless\CWC_CC2650_154Drv.d" "wireless\CWC_IntegrTest.d" "wireless\ERRORS.d" "wireless\comm_lib.d" 
	-$(RM) "CC2650STK.obj" "buzzer.obj" "ccfg.obj" "kuva.obj" "main.obj" "sensors\bmp280.obj" "sensors\hdc1000.obj" "sensors\mpu9250.obj" "sensors\opt3001.obj" "sensors\tmp007.obj" "wireless\CWC_CC2650_154Drv.obj" "wireless\CWC_IntegrTest.obj" "wireless\ERRORS.obj" "wireless\comm_lib.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

