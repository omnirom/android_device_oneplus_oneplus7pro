/*
* Copyright (C) 2017 The OmniROM Project
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

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.res.Resources;
import android.content.Intent;
import android.os.Bundle;
import androidx.preference.PreferenceFragment;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceCategory;
import androidx.preference.PreferenceScreen;
import androidx.preference.TwoStatePreference;
import android.provider.Settings;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.util.Log;

public class DozeSettings extends PreferenceFragment  {

    private static final String KEY_TILT_CHECK = "tilt_check";
    private static final String KEY_SINGLE_TAP = "single_tap";
    private static final String KEY_WAVE_CHECK = "wave_check";
    private static final String KEY_POCKET_CHECK = "pocket_check";
    private static final String KEY_FOOTER = "footer";
    private static final boolean sIsOnePlus7t = android.os.Build.DEVICE.equals("oneplus7t");

    private boolean mUseTiltCheck;
    private boolean mUseSingleTap;
    private boolean mUseWaveCheck;
    private boolean mUsePocketCheck;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.doze_settings, rootKey);

        getDozeSettings();

        TwoStatePreference singleTapSwitch = (TwoStatePreference) findPreference(KEY_SINGLE_TAP);
        singleTapSwitch.setChecked(mUseSingleTap);
        singleTapSwitch.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                mUseSingleTap = (Boolean) newValue;
                setDozeSettings();
                return true;
            }
        });
        TwoStatePreference tiltSwitch = (TwoStatePreference) findPreference(KEY_TILT_CHECK);
        tiltSwitch.setChecked(mUseTiltCheck);
        tiltSwitch.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                mUseTiltCheck = (Boolean) newValue;
                setDozeSettings();
                return true;
            }
        });
        TwoStatePreference waveSwitch = (TwoStatePreference) findPreference(KEY_WAVE_CHECK);
        waveSwitch.setChecked(mUseWaveCheck);
        waveSwitch.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                mUseWaveCheck = (Boolean) newValue;
                setDozeSettings();
                return true;
            }
        });
        if (!sIsOnePlus7t) {
            getPreferenceScreen().removePreference(waveSwitch);
        }
        TwoStatePreference pocketSwitch = (TwoStatePreference) findPreference(KEY_POCKET_CHECK);
        pocketSwitch.setChecked(mUsePocketCheck);
        pocketSwitch.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                mUsePocketCheck = (Boolean) newValue;
                setDozeSettings();
                return true;
            }
        });
        if (!sIsOnePlus7t) {
            getPreferenceScreen().removePreference(pocketSwitch);
        }
        Preference footer = findPreference(KEY_FOOTER);
        if (isAmbientDisplayEnabled()) {
            getPreferenceScreen().removePreference(footer);
        }
    }

    private void getDozeSettings() {
        String value = Settings.System.getString(getContext().getContentResolver(),
                    Settings.System.OMNI_DEVICE_FEATURE_SETTINGS);
        if (!TextUtils.isEmpty(value)) {
            String[] parts = value.split(":");
            mUseTiltCheck = Boolean.valueOf(parts[0]);
            mUseSingleTap = Boolean.valueOf(parts[1]);
            if (parts.length >= 3) {
                mUseWaveCheck = Boolean.valueOf(parts[2]);
            } else {
                mUseWaveCheck = false;
            }
            if (parts.length == 4) {
                mUsePocketCheck = Boolean.valueOf(parts[3]);
            } else {
                mUsePocketCheck = false;
            }
        }
    }

    private void setDozeSettings() {
        String newValue = String.valueOf(mUseTiltCheck) + ":" + String.valueOf(mUseSingleTap)
                + ":" + String.valueOf(mUseWaveCheck) + ":" + String.valueOf(mUsePocketCheck);
        Settings.System.putString(getContext().getContentResolver(), Settings.System.OMNI_DEVICE_FEATURE_SETTINGS, newValue);
    }

    private boolean isAmbientDisplayEnabled() {
        return Settings.Secure.getInt(getContext().getContentResolver(), Settings.Secure.DOZE_ENABLED, 1) == 1;
    }
}
