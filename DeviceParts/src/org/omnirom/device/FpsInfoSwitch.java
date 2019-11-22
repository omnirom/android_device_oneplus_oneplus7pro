/*
 * Copyright (C) 2019 The OmniROM Project
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

package org.omnirom.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.Intent;
import android.provider.Settings;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceManager;

import org.omnirom.device.DeviceSettings;

public class FpsInfoSwitch implements OnPreferenceChangeListener {
    public static final String SETTINGS_KEY = DeviceSettings.KEY_SETTINGS_PREFIX + DeviceSettings.KEY_FPS_INFO;
    private static final String TAG = "FpsBootReceiver";
    private Context mContext;

    public FpsInfoSwitch (Context context) {
        mContext = context;
    }

    public static boolean isCurrentlyEnabled(Context context) {
        return Settings.System.getInt(context.getContentResolver(), SETTINGS_KEY, 1) == 1;
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Boolean enabled = (Boolean) newValue;
        Intent fpsinfo = new Intent(mContext, org.omnirom.device.FPSInfoService.class);
        if (enabled) {
            mContext.startService(fpsinfo);
        } else {
            mContext.stopService(fpsinfo);
        }
        return true;
    }
}
