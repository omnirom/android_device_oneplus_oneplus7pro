/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.vibrator@1.3-service.oneplus7pro"
#include <log/log.h>
#include <hardware/hardware.h>
#include <hardware/vibrator.h>
#include <cutils/properties.h>
#include "Vibrator.h"
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <fstream>
namespace android {
namespace hardware {
namespace vibrator {
namespace V1_3 {
namespace implementation {
static constexpr int8_t MAX_RTP_INPUT = 127;
static constexpr int8_t MIN_RTP_INPUT = 0;
static constexpr char RTP_MODE[] = "rtp";
static constexpr char WAVEFORM_MODE[] = "waveform";
static constexpr uint32_t LOOP_MODE_OPEN = 1;
static constexpr uint32_t SINE_WAVE = 1;
static constexpr uint32_t SQUARE_WAVE = 0;
static constexpr uint32_t VMAX = 9;
static constexpr uint32_t GAIN = 128;
// Default max voltage 2.15V
static constexpr uint32_t VOLTAGE_MAX = 107;
// Use effect #1 in the waveform library for CLICK effect
static constexpr char WAVEFORM_CLICK_EFFECT_SEQ0[] = "0 1";
static constexpr char WAVEFORM_CLICK_EFFECT_SEQ1[] = "1 0";
static constexpr int32_t WAVEFORM_CLICK_EFFECT_MS = 0;
// Use effect #2 in the waveform library for TICK effect
static constexpr char WAVEFORM_TICK_EFFECT_SEQ0[] = "0 1";
static constexpr char WAVEFORM_TICK_EFFECT_SEQ1[] = "1 0";
static constexpr int32_t WAVEFORM_TICK_EFFECT_MS = 0;
// Use effect #3 in the waveform library for DOUBLE_CLICK effect
static constexpr char WAVEFORM_DOUBLE_CLICK_EFFECT_SEQ[] = "0 1";
static constexpr uint32_t WAVEFORM_DOUBLE_CLICK_EFFECT_MS = 10;
// Use effect #4 in the waveform library for HEAVY_CLICK effect
static constexpr char WAVEFORM_HEAVY_CLICK_EFFECT_SEQ0[] = "0 0";
static constexpr char WAVEFORM_HEAVY_CLICK_EFFECT_SEQ1[] = "1 0";
static constexpr uint32_t WAVEFORM_HEAVY_CLICK_EFFECT_MS = 10;
using Status = ::android::hardware::vibrator::V1_0::Status;
using EffectStrength = ::android::hardware::vibrator::V1_0::EffectStrength;
Vibrator::Vibrator(std::ofstream&& activate,
        std::ofstream&& ignore_store,
        std::ofstream&& duration,
        std::ofstream&& vmax,
        std::ofstream&& gain,
        std::ofstream&& brightness,
        std::ofstream&& state, std::ofstream&& rtpinput,
        std::ofstream&& mode, std::ofstream&& sequencer,
        std::ofstream&& scale, std::ofstream&& ctrlloop,
        std::ofstream&& lptrigger,
        std::ofstream&& lrawaveshape, std::ofstream&& odclamp, std::ofstream&& ollraperiod,
        std::uint32_t short_lra_period, std::uint32_t long_lra_period) :
        mActivate(std::move(activate)),
        mIgnoreStore(std::move(ignore_store)),
        mDuration(std::move(duration)),
        mVmax(std::move(vmax)),
        mGain(std::move(gain)),
        mBrightness(std::move(brightness)),
        mState(std::move(state)),
        mRtpInput(std::move(rtpinput)),
        mMode(std::move(mode)),
        mSequencer(std::move(sequencer)),
        mScale(std::move(scale)),
        mCtrlLoop(std::move(ctrlloop)),
        mLpTriggerEffect(std::move(lptrigger)),
        mLraWaveShape(std::move(lrawaveshape)),
        mOdClamp(std::move(odclamp)),
        mOlLraPeriod(std::move(ollraperiod)),
        mShortLraPeriod(short_lra_period),
        mLongLraPeriod(long_lra_period) { 
    mClickDuration = property_get_int32("ro.vibrator.hal.click.duration", WAVEFORM_CLICK_EFFECT_MS);
    mTickDuration = property_get_int32("ro.vibrator.hal.tick.duration", WAVEFORM_TICK_EFFECT_MS);
    mHeavyClickDuration = property_get_int32(
        "ro.vibrator.hal.heavyclick.duration", WAVEFORM_HEAVY_CLICK_EFFECT_MS);
    mShortVoltageMax = property_get_int32("ro.vibrator.hal.short.voltage", VOLTAGE_MAX);
    mLongVoltageMax = property_get_int32("ro.vibrator.hal.long.voltage", VOLTAGE_MAX);
    // This enables effect #1 from the waveform library to be triggered by SLPI
    // while the AP is in suspend mode
    mLpTriggerEffect << 1 << std::endl;
    if (!mLpTriggerEffect) {
        ALOGW("Failed to set LP trigger mode (%d): %s", errno, strerror(errno));
    }
    shouldBright = false;
}
Return<Status> Vibrator::on(uint32_t timeoutMs, bool isWaveform) {
    mCtrlLoop << LOOP_MODE_OPEN << std::endl;
    mDuration << timeoutMs << std::endl;
    if (!mDuration) {
        ALOGE("Failed to set duration (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    if (isWaveform) {
        mMode << WAVEFORM_MODE << std::endl;
        mLraWaveShape << SINE_WAVE << std::endl;
        mOdClamp << mShortVoltageMax << std::endl;
        mOlLraPeriod << mShortLraPeriod << std::endl;
    } else {
        mMode << RTP_MODE << std::endl;
        mLraWaveShape << SQUARE_WAVE << std::endl;
        mOdClamp << mLongVoltageMax << std::endl;
        mOlLraPeriod << mLongLraPeriod << std::endl;
    }
    if (shouldBright) {
        mBrightness << 1 << std::endl;
    } else {
        mBrightness << 0 << std::endl;
        mActivate << 1 << std::endl;
    }
    if (!mActivate) {
        ALOGE("Failed to activate (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
   return Status::OK;
}
Return<Status> Vibrator::on(uint32_t timeoutMs) {
    shouldBright = false;
    return on(timeoutMs, false /* isWaveform */);
}
Return<Status> Vibrator::off()  {
    mActivate << 0 << std::endl;
    mBrightness << 0 << std::endl;
    if (!mActivate) {
        ALOGE("Failed to turn vibrator off (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    return Status::OK;
}
Return<bool> Vibrator::supportsAmplitudeControl()  {
    return (mRtpInput ? true : false);
}
Return<Status> Vibrator::setAmplitude(uint8_t amplitude) {
    if (amplitude == 0) {
        return Status::BAD_VALUE;
    }
    int32_t rtp_input =
           std::round((amplitude - 1) / 254.0 * (MAX_RTP_INPUT - MIN_RTP_INPUT) +
           MIN_RTP_INPUT);
    mRtpInput << rtp_input << std::endl;
    if (!mRtpInput) {
        ALOGE("Failed to set amplitude (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    return Status::OK;
}
static uint8_t convertEffectStrength(EffectStrength strength) {
    uint8_t scale;
    switch (strength) {
    case EffectStrength::LIGHT:
        scale = 54; // 50%
        break;
    case EffectStrength::MEDIUM:
    case EffectStrength::STRONG:
        scale = 107; // 100%
        break;
    }
    return scale;
}
Return<bool> Vibrator::supportsExternalControl() {
    return false;
}
Return<Status> Vibrator::setExternalControl(bool) {
    return Status::OK;
}
Return<void> Vibrator::perform(V1_0::Effect effect, EffectStrength strength, perform_cb _hidl_cb) {
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
}
Return<void> Vibrator::perform_1_1(V1_1::Effect_1_1 effect, EffectStrength strength,
        perform_cb _hidl_cb) {
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
}
Return<void> Vibrator::perform_1_2(V1_2::Effect effect, EffectStrength strength, perform_cb _hidl_cb) {
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
}
Return<void> Vibrator::perform_1_3(Effect effect, EffectStrength strength, perform_cb _hidl_cb) {
    return performEffect(static_cast<Effect>(effect), strength, _hidl_cb);
}
Return<void> Vibrator::performEffect(Effect effect, EffectStrength strength, perform_cb _hidl_cb) {
    Status status = Status::OK;
    uint32_t timeMS;
    switch (effect) {
    case Effect::CLICK:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        mSequencer << WAVEFORM_CLICK_EFFECT_SEQ0 << std::endl;
        mSequencer << WAVEFORM_CLICK_EFFECT_SEQ1 << std::endl;
        mCtrlLoop << "0 0x0" << std::endl;
        shouldBright = true;
        timeMS = mClickDuration;
        break;
    case Effect::DOUBLE_CLICK:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        mSequencer << WAVEFORM_DOUBLE_CLICK_EFFECT_SEQ << std::endl;
        mCtrlLoop << "0 0x0" << std::endl;
        mCtrlLoop << "1 0x1" << std::endl;
        shouldBright = true;
        timeMS = WAVEFORM_DOUBLE_CLICK_EFFECT_MS;
        break;
    case Effect::TICK:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        mSequencer << WAVEFORM_TICK_EFFECT_SEQ0 << std::endl;
        mSequencer << WAVEFORM_TICK_EFFECT_SEQ1 << std::endl;
        mCtrlLoop << "1 0x0" << std::endl;
        shouldBright = true;
        timeMS = mTickDuration;
        break;
    case Effect::HEAVY_CLICK:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl; 
        mDuration << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        mSequencer << WAVEFORM_HEAVY_CLICK_EFFECT_SEQ0 << std::endl;
        mSequencer << WAVEFORM_HEAVY_CLICK_EFFECT_SEQ1 << std::endl;
        mCtrlLoop << "1 0x1" << std::endl;
        shouldBright = true;
        timeMS = mHeavyClickDuration;
        break;
    case Effect::POP:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl;
        mDuration << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        shouldBright = true;
        timeMS = 5;
        break;
    case Effect::THUD:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl;
        mDuration << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        shouldBright = true;
        timeMS = 10;
        break;
    // TODO for now use the same as CLICK
    case Effect::TEXTURE_TICK:
        mActivate << 0 << std::endl;
        mIgnoreStore << 0 << std::endl;
        mVmax << VMAX << std::endl;
        mGain << GAIN << std::endl;
        mSequencer << WAVEFORM_CLICK_EFFECT_SEQ0 << std::endl;
        mSequencer << WAVEFORM_CLICK_EFFECT_SEQ1 << std::endl;
        mCtrlLoop << "0 0x0" << std::endl;
        shouldBright = true;
        timeMS = mClickDuration;
        break;
    default:
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        shouldBright = false;
        return Void();
    }
    mScale << convertEffectStrength(strength) << std::endl;
    on(timeMS, true /* isWaveform */);
    _hidl_cb(status, timeMS);
    return Void();
}
} // namespace implementation
}  // namespace V1_3
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
