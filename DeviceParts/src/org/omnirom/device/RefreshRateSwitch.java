/*
* Copyright (C) 2016 The OmniROM Project
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
package org.omnirom.device;

import android.content.Context;
import android.content.SharedPreferences;
import android.provider.Settings;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceManager;

import org.omnirom.device.DeviceSettings;

public class RefreshRateSwitch implements OnPreferenceChangeListener {

    public static final String SETTINGS_KEY = DeviceSettings.KEY_SETTINGS_PREFIX + DeviceSettings.KEY_REFRESH_RATE;
    private Context mContext;

    public RefreshRateSwitch(Context context) {
        mContext = context;
    }

    public static boolean isCurrentlyEnabled(Context context) {
        return Settings.System.getInt(context.getContentResolver(),
                Settings.System.PEAK_REFRESH_RATE, 90) == 90;
    }

    public static void setPeakRefresh (Context context, boolean enabled) {
        Settings.System.putInt(context.getContentResolver(),
                Settings.System.PEAK_REFRESH_RATE, enabled ? 90 : 60);
        Settings.System.putInt(context.getContentResolver(),
                Settings.System.MIN_REFRESH_RATE, enabled ? 90 : 60);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Boolean enabled = (Boolean) newValue;
        Settings.System.putInt(mContext.getContentResolver(),
                Settings.System.PEAK_REFRESH_RATE, enabled ? 90 : 60);
        Settings.System.putInt(mContext.getContentResolver(),
                Settings.System.MIN_REFRESH_RATE, enabled ? 90 : 60);
        return true;
    }
}
