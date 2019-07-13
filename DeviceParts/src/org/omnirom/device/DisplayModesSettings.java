/*
* Copyright (C) 2019 The OmniROM Project
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
import android.hardware.display.DisplayManager;
import android.os.Bundle;
import android.os.UserHandle;
import android.provider.Settings;
import android.support.v14.preference.PreferenceFragment;
import android.support.v7.preference.ListPreference;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceCategory;
import android.support.v7.preference.PreferenceManager;
import android.support.v7.preference.PreferenceScreen;
import android.support.v7.preference.TwoStatePreference;
import android.view.Display;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.util.Log;

import com.android.settingslib.display.DisplayDensityUtils;

public class DisplayModesSettings extends PreferenceFragment implements RadioGroup.OnCheckedChangeListener {
    private RadioGroup mRadioGroup;
    private DisplayManager mDisplayManager;
    private Display.Mode[] mDisplayModes;
    private Display mDisplay;
    private int mModesCount;

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mRadioGroup = (RadioGroup) view.findViewById(R.id.radio_group);

        final RadioButton[] rb = new RadioButton[mModesCount];
        for(int i=0; i < mModesCount; i++){
           rb[i]  = new RadioButton(getContext());
           rb[i].setText(" " + mDisplayModes[i].getPhysicalHeight()
                 + "x" + mDisplayModes[i].getPhysicalWidth() + "@"
                 + mDisplayModes[i].getRefreshRate());
           rb[i].setId(i + 1);
           rb[i].setGravity(Gravity.CENTER_VERTICAL);
           rb[i].setTextAppearance(android.R.style.TextAppearance_Medium);
           mRadioGroup.addView(rb[i]);
        }
        int displayMode = Settings.System.getIntForUser(getContext().getContentResolver(),
                         Settings.System.OMNI_DISPLAY_MODE, 3, UserHandle.USER_CURRENT);
        mRadioGroup.check(displayMode);
        mRadioGroup.setOnCheckedChangeListener(this);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mDisplayManager = (DisplayManager)getContext().getSystemService(Context.DISPLAY_SERVICE);
        mDisplay = mDisplayManager.getDisplay(0);
        mDisplayModes = mDisplay.getSupportedModes();
        mModesCount = mDisplayModes.length;
        return inflater.inflate(R.layout.display_modes, container, false);
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
            int displayMode = Settings.System.getIntForUser(getContext().getContentResolver(),
                         Settings.System.OMNI_DISPLAY_MODE, 3, UserHandle.USER_CURRENT);
            Settings.System.putInt(getContext().getContentResolver(), Settings.System.OMNI_DISPLAY_MODE, checkedId);
            if (checkedId == displayMode)
                return;
            int newDensity = -1;
            if (checkedId == 1 || checkedId == 2) {
                newDensity = 480;
            } else {
                newDensity = 560;
            }

            DisplayDensityUtils.setForcedDisplayDensity(Display.DEFAULT_DISPLAY, newDensity);
    }
}
