# Copyright (C) 2010 The Android Open Source Project
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

VENDOR_EXCEPTION_PATHS += oneplus \
    omni

# Sample: This is where we'd set a backup provider if we had one
# $(call inherit-product, device/sample/products/backup_overlay.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# Get the prebuilt list of APNs
$(call inherit-product, vendor/omni/config/gsm.mk)

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

# must be before including omni part
TARGET_BOOTANIMATION_SIZE := 1080p
AB_OTA_UPDATER := true

DEVICE_PACKAGE_OVERLAYS += device/oneplus/oneplus7pro/overlay/common
DEVICE_PACKAGE_OVERLAYS += device/oneplus/oneplus7pro/overlay/device
DEVICE_PACKAGE_OVERLAYS += vendor/omni/overlay/CarrierConfig

#BOARD_AVB_ALGORITHM := SHA256_RSA2048
#BOARD_AVB_KEY_PATH := build/target/product/security/verity.pem

# Inherit from our custom product configuration
$(call inherit-product, vendor/omni/config/common.mk)

# Inherit from hardware-specific part of the product configuration
$(call inherit-product, device/oneplus/oneplus7pro/device.mk)

# get the rest of aosp stuff after ours
$(call inherit-product, $(SRC_TARGET_DIR)/product/mainline_system_arm64.mk)


ALLOW_MISSING_DEPENDENCIES := true

PRODUCT_SHIPPING_API_LEVEL := 29

# Discard inherited values and use our own instead.
PRODUCT_NAME := omni_oneplus7pro
PRODUCT_DEVICE := oneplus7pro
PRODUCT_BRAND := OnePlus
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_MODEL := GM1917 

TARGET_DEVICE := OnePlus7Pro
PRODUCT_SYSTEM_NAME := OnePlus7Pro

VENDOR_RELEASE := 10/QKQ1.190716.003/1910071200:user/release-keys
#PLATFORM_SECURITY_PATCH_TIMESTAMP := 1909110008
#BUILD_ID := QKQ1
#BUILD_NUMBER := 1909110008
#BUILD_THUMBPRINT := $(VENDOR_RELEASE)
BUILD_FINGERPRINT := OnePlus/OnePlus7Pro_EEA/OnePlus7Pro:$(VENDOR_RELEASE)
OMNI_BUILD_FINGERPRINT := OnePlus/OnePlus7Pro_EEA/OnePlus7Pro:$(VENDOR_RELEASE)
OMNI_PRIVATE_BUILD_DESC := "'OnePlus7Pro-user 10 QKQ1.190716.003 1910071200 release-keys'"

PLATFORM_SECURITY_PATCH_OVERRIDE := 2019-09-05

TARGET_VENDOR := oneplus

# Fingerprint
PRODUCT_PACKAGES += \
    omni.biometrics.fingerprint.inscreen@1.1-service.oneplus7pro

$(call inherit-product, vendor/oneplus/oneplus7pro/oneplus7pro-vendor.mk)
