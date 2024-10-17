project := demo

PHONY := _all 
_all: all

srctree := .
objtree := .
export srctree objtree

custom_c_flag   := -Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-function -fstack-usage -Wall -Wextra -fPIC
custom_cpp_flag := -Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-function -fstack-usage -Wall -Wextra -fPIC
custom_a_flag   :=
custom_ld_flag  :=
custom_ar_flag  :=

# Set build head
head-y :=
head-y += src/main.o
head   := $(head-y)

# Set build libs
libs-y    :=
libs-y    += src/
libs      := $(patsubst %/, %/built-in.o, $(libs-y))
libs-dirs = $(patsubst %/, %, $(filter %/, $(libs-y)))

# Set inc headers
inch :=
inch += inc/
inch := $(foreach inc,$(inch),-I$(inc))

# Set inc libs
incl := 
incl += lib/
incl := $(foreach inc,$(incl),-L$(inc))

# Set use libs
uselibs := 
uselibs += 
uselibs := $(foreach lib,$(uselibs),-l$(lib))

custom_c_flag   += $(inch) $(incl) $(uselibs)
custom_cpp_flag += $(inch) $(incl) $(uselibs)

scripts/Kbuild.include: ;
include scripts/Kbuild.include

# Make variables (CC, etc...)
CROSS_COMPILE := 
AS			  = $(CROSS_COMPILE)as
LD			  = $(CROSS_COMPILE)ld
CC			  = $(CROSS_COMPILE)gcc
CPP			  = $(CROSS_COMPILE)g++
AR			  = $(CROSS_COMPILE)ar
NM			  = $(CROSS_COMPILE)nm
LDR			  = $(CROSS_COMPILE)ldr
STRIP		  = $(CROSS_COMPILE)strip
OBJCOPY		  = $(CROSS_COMPILE)objcopy
OBJDUMP		  = $(CROSS_COMPILE)objdump
export CROSS_COMPILE AS LD CC CPP AR NM LDR STRIP OBJCOPY OBJDUMP

# Set compiler
KBUILD_CFLAGS 	:= $(custom_c_flag)
KBUILD_CPPFLAGS := $(custom_cpp_flag)
KBUILD_AFLAGS 	:= $(custom_a_flag)
KBUILD_LDFLAGS 	:= $(custom_ld_flag)
KBUILD_ARFLAGS  := cDPrsT$(custom_ar_flag)

export KBUILD_CFLAGS KBUILD_CPPFLAGS KBUILD_AFLAGS KBUILD_LDFLAGS KBUILD_ARFLAGS

c_flags   := $(KBUILD_CFLAGS)
cpp_flags := $(KBUILD_CPPFLAGS)
a_flags   := $(KBUILD_AFLAGS)
ld_flags  := $(KBUILD_LDFLAGS)
ar_flags  := $(KBUILD_ARFLAGS)

# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifeq ("$(origin V)", "command line")
	KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
	KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
	quiet =
	Q =
else
	quiet = quiet_
	Q = @
endif
export quiet Q 

prepare: FORCE

PHONY += all
all: prepare $(project)

$(project): $(libs-dirs)
	$(call cmd,build_obj)

$(libs-dirs): FORCE
	$(Q)$(MAKE) $(build)=$@

PHONY += clean-libs
clean-libs: 
	$(Q)$(foreach dir, $(libs-dirs), $(MAKE) $(clean)=$(dir);)

PHONY += clean-build
clean-build: 
	$(call cmd,clean_build)

PHONY += clean
clean: clean-libs clean-build
	$(Q)ls .*.cmd | xargs rm -f

PHONY += distclean
distclean: clean
	$(call cmd,distclean)

quiet_cmd_build_obj = BUILD     $@
cmd_build_obj 		= $(CPP) -o $(project) $(head) $(libs) $(cpp_flags) 

quiet_cmd_clean_build = CLEAN     $(project)
cmd_clean_build 	  = rm $(project) .$(project).cmd -f;

quiet_cmd_distclean = CLEAN     *.o *.su *.cmd
cmd_distclean 	    = \
	find -name *.o | xargs rm -f; \
	find -name *.su | xargs rm -f; \
	find -name *.cmd | xargs rm -f

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
