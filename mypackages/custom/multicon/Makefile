include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=multicon
PKG_VERSION:=1.2
PKG_RELEASE:=1


# Source settings (i.e. where to find the source codes)
# This is a custom variable, used below
SOURCE_DIR:=/home/biagioscarciello/Scrivania/RUT9XX_R_GPL_00.06.07.3/multicon

include $(INCLUDE_DIR)/package.mk


# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/multicon
	SECTION:=custom
	CATEGORY:=Custom Applications
	DEPENDS:= +libmosquitto-ssl
	TITLE:=Multicon Application
endef

# Package description; a more verbose description on what our package does
define Package/multicon/description
	Multicon device -application.
endef

define Build/Configure
  $(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR))
endef



# Package preparation instructions; create the build directory and copy the source code. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp $(SOURCE_DIR)/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef


MODULES = $(PKG_BUILD_DIR)/multicon.o \
		$(PKG_BUILD_DIR)/device_settings.o\
		$(PKG_BUILD_DIR)/device_mqtt.o\
		$(PKG_BUILD_DIR)/device_json.o\
		$(PKG_BUILD_DIR)/device_utils.o \
		$(PKG_BUILD_DIR)/system_logger.o \
		$(PKG_BUILD_DIR)/dyad.o \
		$(PKG_BUILD_DIR)/http_server.o \
		$(PKG_BUILD_DIR)/json.o -lm -lmosquitto

# Package build instructions; invoke the target-specific compiler to first compile the source file, and then to link the file into the final executable 
define Build/Compile
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/json.o -c $(PKG_BUILD_DIR)/json.c
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/device_utils.o -c $(PKG_BUILD_DIR)/device_utils.c
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/system_logger.o -c $(PKG_BUILD_DIR)/system_logger.c
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/device_json.o -c $(PKG_BUILD_DIR)/device_json.c
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/device_settings.o -c $(PKG_BUILD_DIR)/device_settings.c
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/device_mqtt.o -c $(PKG_BUILD_DIR)/device_mqtt.c
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/dyad.o -c $(PKG_BUILD_DIR)/dyad.c 
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/http_server.o -c $(PKG_BUILD_DIR)/http_server.c 
		$(TARGET_CC) $(TARGET_CFLAGS)  -o $(PKG_BUILD_DIR)/multicon.o -c $(PKG_BUILD_DIR)/multicon.c
		$(TARGET_CC) $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/$1 $(MODULES)
endef


# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/multicon/install
		$(INSTALL_DIR) $(1)/usr/bin
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/multicon $(1)/usr/bin
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,multicon))