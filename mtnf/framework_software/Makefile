
ifndef RTE_SDK
$(error "Please define RTE_SDK environment variable")
endif


# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

ifneq ($(CONFIG_RTE_EXEC_ENV),"linuxapp")
$(error This application can only operate in a linuxapp environment, \
please change the definition of the RTE_TARGET environment variable)
endif

# binary name
APP = mtnf

# all source are stored in SRCS-y
SRCS-y := main.c mtnf_args.c mtnf_tenant.c mtnf_init.c mtnf_stats.c

INC := mtnf_common.h mtnf_args.h mtnf_tenant.h mtnf_init.h mtnf_stats.h

CFLAGS += $(WERROR_FLAGS) -O3 $(USER_FLAGS)
CFLAGS += -I$(SRCDIR)/../libs/includes/
CFLAGS += -I$(SRCDIR)/../nfs/includes/
CFLAGS += -D_GNU_SOURCE
LDFLAGS += -L$(SRCDIR)/../target/libs/ -lmtnfdpdk
LDFLAGS += -L$(SRCDIR)/../target/nfs/ -lmtnfs


# for newer gcc, e.g. 4.4, no-strict-aliasing may not be necessary
# and so the next line can be removed in those cases.
EXTRA_CFLAGS += -fno-strict-aliasing

include $(RTE_SDK)/mk/rte.extapp.mk