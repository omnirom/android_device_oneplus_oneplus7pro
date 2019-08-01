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
#define LOG_TAG "android.hardware.vibrator@1.2-service.oneplus7pro"
#include <android/hardware/vibrator/1.2/IVibrator.h>
#include <cutils/properties.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>
#include <utils/StrongPointer.h>
#include "Vibrator.h"
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::vibrator::V1_2::IVibrator;
using android::hardware::vibrator::V1_2::implementation::Vibrator;
using namespace android;
// Refer to non existing
// kernel documentation on the detail usages for ABIs below
static constexpr char ACTIVATE_PATH[] = "/sys/class/leds/vibrator/activate";
static constexpr char IGNORE_STORE_PATH[] = "/sys/class/leds/vibrator/ignore_store";
static constexpr char VMAX_PATH[] = "/sys/class/leds/vibrator/vmax";
static constexpr char GAIN_PATH[] = "/sys/class/leds/vibrator/gain";
static constexpr char BRIGHTNESS_PATH[] = "/sys/class/leds/vibrator/brightness";
static constexpr char DURATION_PATH[] = "/sys/class/leds/vibrator/duration";
static constexpr char STATE_PATH[] = "/sys/class/leds/vibrator/state";
static constexpr char RTP_INPUT_PATH[] = "/sys/class/leds/vibrator/rtp";
static constexpr char MODE_PATH[] = "/sys/class/leds/vibrator/activate_mode";
static constexpr char SEQUENCER_PATH[] = "/sys/class/leds/vibrator/seq";
static constexpr char SCALE_PATH[] = "/sys/class/leds/vibrator/gain";
static constexpr char CTRL_LOOP_PATH[] = "/sys/class/leds/vibrator/loop";
static constexpr char LP_TRIGGER_PATH[] = "/sys/class/leds/vibrator/haptic_audio";
static constexpr char LRA_WAVE_SHAPE_PATH[] = "/sys/class/leds/vibrator/lra_resistance";
static constexpr char OD_CLAMP_PATH[] = "/sys/class/leds/vibrator/od_clamp";
// Kernel ABIs for updating the calibration data
static constexpr char AUTOCAL_CONFIG[] = "autocal";
static constexpr char LRA_PERIOD_CONFIG[] = "lra";
static constexpr char AUTOCAL_FILEPATH[] = "/sys/class/leds/vibrator/autocal";
static constexpr char OL_LRA_PERIOD_FILEPATH[] = "/sys/class/leds/vibrator/ol_lra_period";
// Set a default lra period in case there is no calibration file
static constexpr uint32_t DEFAULT_LRA_PERIOD = 173;
static constexpr uint32_t DEFAULT_FREQUENCY_SHIFT = 10;
static std::uint32_t freqPeriodFormula(std::uint32_t in) {
    return 1000000000 / (24615 * in);
}
static bool loadCalibrationData(std::uint32_t &short_lra_period,
        std::uint32_t &long_lra_period) {
    std::map<std::string, std::string> config_data;
    std::ofstream autocal{AUTOCAL_FILEPATH};
    if (!autocal) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", AUTOCAL_FILEPATH, error,
                strerror(error));
        return false;
    }
    std::ofstream ol_lra_period{OL_LRA_PERIOD_FILEPATH};
    if (!ol_lra_period) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", OL_LRA_PERIOD_FILEPATH, error,
                strerror(error));
        return false;
    }
    std::string line;
    if(config_data.find(AUTOCAL_CONFIG) != config_data.end()) {
        autocal << config_data[AUTOCAL_CONFIG] << std::endl;
    }
    if(config_data.find(LRA_PERIOD_CONFIG) != config_data.end()) {
        uint32_t thisFrequency;
        uint32_t thisPeriod;
        ol_lra_period << config_data[LRA_PERIOD_CONFIG] << std::endl;
        thisPeriod = std::stoul(config_data[LRA_PERIOD_CONFIG]);
        short_lra_period = thisPeriod;
        // 1. Change long lra period to frequency
        // 2. Get frequency': subtract the frequency shift from the frequency
        // 3. Get final long lra period after put the frequency' to formula
        thisFrequency = freqPeriodFormula(thisPeriod) -
                property_get_int32("ro.vibrator.hal.long.frequency.shift",
                        DEFAULT_FREQUENCY_SHIFT);
        long_lra_period = freqPeriodFormula(thisFrequency);
    }
    return true;
}
status_t registerVibratorService() {
    // Calibration data: lra period 262(i.e. 155Hz)
    std::uint32_t short_lra_period(DEFAULT_LRA_PERIOD);
    std::uint32_t long_lra_period(DEFAULT_LRA_PERIOD);
    // ostreams below are required
    std::ofstream activate{ACTIVATE_PATH};
    if (!activate) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", ACTIVATE_PATH, error, strerror(error));
        return -error;
    }
    std::ofstream ignore_store{IGNORE_STORE_PATH};
    if (!ignore_store) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", IGNORE_STORE_PATH, error, strerror(error));
        return -error;
    }
    std::ofstream vmax{VMAX_PATH};
    if (!vmax) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", VMAX_PATH, error, strerror(error));
        return -error;
    }
    std::ofstream gain{GAIN_PATH};
    if (!gain) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", GAIN_PATH, error, strerror(error));
        return -error;
    }
    std::ofstream brightness{BRIGHTNESS_PATH};
    if (!brightness) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", BRIGHTNESS_PATH, error, strerror(error));
        return -error;
    }
    std::ofstream duration{DURATION_PATH};
    if (!duration) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", DURATION_PATH, error, strerror(error));
        return -error;
    }
    std::ofstream state{STATE_PATH};
    if (!state) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", STATE_PATH, error, strerror(error));
        return -error;
    }
    state << 1 << std::endl;
    if (!state) {
        int error = errno;
        ALOGE("Failed to set state (%d): %s", errno, strerror(errno));
        return -error;
    }
    // ostreams below are optional
    std::ofstream rtpinput{RTP_INPUT_PATH};
    if (!rtpinput) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", RTP_INPUT_PATH, error, strerror(error));
    }
    std::ofstream mode{MODE_PATH};
    if (!mode) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", MODE_PATH, error, strerror(error));
    }
    std::ofstream sequencer{SEQUENCER_PATH};
    if (!sequencer) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", SEQUENCER_PATH, error, strerror(error));
    }
    std::ofstream scale{SCALE_PATH};
    if (!scale) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", SCALE_PATH, error, strerror(error));
    }
    std::ofstream ctrlloop{CTRL_LOOP_PATH};
    if (!ctrlloop) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", CTRL_LOOP_PATH, error, strerror(error));
    }
    std::ofstream lptrigger{LP_TRIGGER_PATH};
    if (!lptrigger) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", LP_TRIGGER_PATH, error, strerror(error));
    }
    std::ofstream lrawaveshape{LRA_WAVE_SHAPE_PATH};
    if (!lrawaveshape) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", LRA_WAVE_SHAPE_PATH, error, strerror(error));
    }
    std::ofstream odclamp{OD_CLAMP_PATH};
    if (!odclamp) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", OD_CLAMP_PATH, error, strerror(error));
    }
    std::ofstream ollraperiod{OL_LRA_PERIOD_FILEPATH};
    if (!ollraperiod) {
        int error = errno;
        ALOGW("Failed to open %s (%d): %s", OL_LRA_PERIOD_FILEPATH, error, strerror(error));
    }
    if (!loadCalibrationData(short_lra_period, long_lra_period)) {
        ALOGW("Failed load calibration data");
    }
    sp<IVibrator> vibrator = new Vibrator(std::move(activate),
            std::move(ignore_store),
            std::move(duration),
            std::move(vmax),
            std::move(gain),
            std::move(brightness),
            std::move(state), std::move(rtpinput),
            std::move(mode),
            std::move(sequencer), std::move(scale),
            std::move(ctrlloop),
            std::move(lptrigger),
            std::move(lrawaveshape), std::move(odclamp), std::move(ollraperiod),
            short_lra_period, long_lra_period);
    return vibrator->registerAsService();
}
int main() {
    configureRpcThreadpool(1, true);
    status_t status = registerVibratorService();
    if (status != OK) {
        return status;
    }
    joinRpcThreadpool();
}
