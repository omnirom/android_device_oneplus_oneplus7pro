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

VENDOR_EXCEPTION_PATHS := oneplus \
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

# Inherit from our custom product configuration
$(call inherit-product, vendor/omni/config/common.mk)

# Inherit from hardware-specific part of the product configuration
$(call inherit-product, device/oneplus/oneplus7pro/device.mk)

ALLOW_MISSING_DEPENDENCIES := true

PRODUCT_PACKAGES += \
    android.hardware.biometrics.fingerprint@2.1-service

# Discard inherited values and use our own instead.
PRODUCT_NAME := omni_oneplus7pro
PRODUCT_DEVICE := oneplus7pro
PRODUCT_BRAND := OnePlus
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_MODEL := GM1913 

TARGET_DEVICE := OnePlus7Pro
PRODUCT_SYSTEM_NAME := OnePlus7Pro

OMNI_BUILD_FINGERPRINT := OnePlus/OnePlus7Pro_EEA/OnePlus7Pro:9/PKQ1.190110.001/1907122210:user/release-keys \
OMNI_PRIVATE_BUILD_DESC := OnePlus7Pro-user 9 PKQ1.190110.001 1907122210 release-keys

PLATFORM_SECURITY_PATCH_OVERRIDE := 2019-08-01

TARGET_VENDOR := oneplus

#PRODUCT_SYSTEM_PROPERTY_BLACKLIST += \
    ro.product.model

$(call inherit-product, vendor/oneplus/oneplus7pro/device-vendor.mk)

