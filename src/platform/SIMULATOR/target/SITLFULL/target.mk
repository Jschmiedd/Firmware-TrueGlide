TARGET_MCU        := SIMULATOR
TARGET_MCU_FAMILY := SITL
SIMULATOR_BUILD    = yes

# TrueGlide : carte simulée d'une vraie carte. Son config.h (broches, réglages d'usine, nom) est repris tel quel ;
# ses fonctions viennent de carte_generee.h (trueglide/essai_navigateur/sitlfull_carte.py).
SITLFULL_CARTE    ?= SPEEDYBEEF405MINI
SITLFULL_CONFIG   := $(firstword $(wildcard $(ROOT)/src/config/configs/*/$(SITLFULL_CARTE)/config.h))
ifeq ($(SITLFULL_CONFIG),)
$(error SITLFULL : carte $(SITLFULL_CARTE) introuvable dans src/config/configs)
endif
INCLUDE_DIRS      += $(dir $(SITLFULL_CONFIG))
TARGET_FLAGS      += -DUSE_CONFIG

TARGET_SRC = \
            drivers/accgyro/accgyro_virtual.c \
            drivers/barometer/barometer_virtual.c \
            drivers/compass/compass_virtual.c \
            drivers/serial_tcp.c \
            io/gps_virtual.c \
            blackbox/blackbox_virtual.c

SIZE_OPTIMISED_SRC += \
            drivers/serial_tcp.c
