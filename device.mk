# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file is the build configuration for a full Android
# build for grouper hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#
PRODUCT_PACKAGES += \
    libinit_oneplus7pro \
    omnipreopt_script

PRODUCT_PACKAGES += \
    libhidltransport \
    libhwbinder \
    android.hidl.manager@1.0 \
    android.hidl.manager@1.1 \
    android.hidl.manager@1.2 \
    libcutils

PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.build.version.all_codenames=$(PLATFORM_VERSION_ALL_CODENAMES) \
    ro.build.version.codename=$(PLATFORM_VERSION_CODENAME) \
    ro.build.version.release=$(PLATFORM_VERSION) \
    ro.build.version.sdk=$(PLATFORM_SDK_VERSION)

AB_OTA_PARTITIONS += \
    boot \
    dtbo \
    system \
    system_ext \
    product \
    vbmeta

AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/omnipreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true

PRODUCT_PACKAGES += \
    otapreopt_script \
    update_engine \
    update_engine_sideload \
    update_verifier

PRODUCT_HOST_PACKAGES += \
    brillo_update_payload

# Boot control
PRODUCT_PACKAGES_DEBUG += \
    bootctl

# Boot control
PRODUCT_PACKAGES += \
    android.hardware.boot@1.0-impl.recovery \
    bootctrl.msmnile.recovery \
    fastbootd

PRODUCT_PACKAGES_DEBUG += \
    update_engine_client

PRODUCT_PACKAGES += \
    omni_charger_res_images \
    animation.txt \
    font_charger.png

PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

# Live Wallpapers
PRODUCT_PACKAGES += \
    LiveWallpapers \
    LiveWallpapersPicker \
    VisualizationWallpapers \
    librs_jni

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.verified_boot.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.software.verified_boot.xml

# Prebuilt
PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,device/oneplus/oneplus7pro/prebuilt/system,system) \
    $(call find-copy-subdir-files,*,device/oneplus/oneplus7pro/prebuilt/root,root)

ifeq ($(TARGET_DEVICE),oneplus7pro)
PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,device/oneplus/oneplus7pro/prebuilt/product,system/product)
endif

PRODUCT_AAPT_CONFIG := xxxhdpi
PRODUCT_AAPT_PREF_CONFIG := xxxhdpi
PRODUCT_CHARACTERISTICS := nosdcard

# Lights & Health
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-service.oneplus7pro

PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_google_video.xml

# Camera
PRODUCT_PACKAGES += \
    SnapdragonCamera2

# ANT+
PRODUCT_PACKAGES += \
    AntHalService

# QMI
PRODUCT_PACKAGES += \
    libjson

PRODUCT_PACKAGES += \
    ims-ext-common \
    ims_ext_common.xml \
    qti-telephony-hidl-wrapper \
    qti_telephony_hidl_wrapper.xml \
    qti-telephony-utils \
    qti_telephony_utils.xml \
    tcmiface

# Netutils
PRODUCT_PACKAGES += \
    netutils-wrapper-1.0 \
    libandroid_net

PRODUCT_PACKAGES += \
    DeviceParts

PRODUCT_PACKAGES += \
    vndk_package

PRODUCT_PACKAGES += \
    android.hidl.base@1.0
PRODUCT_PACKAGES += vendor.qti.hardware.btconfigstore@1.0
PRODUCT_PACKAGES += com.qualcomm.qti.bluetooth_audio@1.0
PRODUCT_PACKAGES += libbtconfigstore

PRODUCT_PACKAGES += \
    vendor.display.config@1.10 \
    libdisplayconfig \
    libqdMetaData.system \
    libqdMetaData \
    vendor.nxp.nxpese@1.0 \
    vendor.nxp.nxpnfc@1.0 \
    vendor.qti.hardware.camera.device@1.0 \
    vendor.qti.hardware.camera.postproc@1.0 \
    vendor.qti.hardware.systemhelper@1.1 \
    vendor.qti.hardware.bluetooth_dun@1.0 \
    android.hardware.bluetooth@1.0

PRODUCT_HOST_PACKAGES += \
    vendor.oneplus.camera.CameraHIDL@1.0 \
    vendor.oneplus.fingerprint.extension@1.0

#Nfc
PRODUCT_PACKAGES += \
    android.hardware.nfc@1.0 \
    android.hardware.nfc@1.1 \
    android.hardware.nfc@1.2

# Display
PRODUCT_PACKAGES += \
    libion \
    libtinyxml2

PRODUCT_PACKAGES += \
    libtinyalsa


PRODUCT_PACKAGES += \
    vendor.qti.hardware.wifi@1.0 \

# Remove unwanted packages
PRODUCT_PACKAGES += \
    RemovePackages

PRODUCT_BOOT_JARS += \
    tcmiface \
    WfdCommon \
    qcnvitems


PRODUCT_PACKAGES += oneplus-mock
PRODUCT_PACKAGES += tune2fs_ramdisk
PRODUCT_PACKAGES += tune2fs_static
PRODUCT_BOOT_JARS += oneplus-mock
PRODUCT_BOOT_JARS += UxPerformance
PRODUCT_BOOT_JARS += QPerformance
#PRODUCT_BOOT_JARS += android.hidl.manager-V1.0-java


TARGET_FS_CONFIG_GEN := device/oneplus/oneplus7pro/config.fs

PRODUCT_MANIFEST_FILES += \
    device/oneplus/oneplus7pro/vintf/vendor.omni.biometrics.fingerprint.inscreen-1.0-service.xml \
    device/oneplus/oneplus7pro/vintf/android.hardware.sensors-2.0.xml

     
$(call inherit-product, build/make/target/product/gsi_keys.mk)
