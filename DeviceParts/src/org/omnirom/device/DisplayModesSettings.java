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

import android.app.ActivityManager;
import android.app.ActivityManager.RecentTaskInfo;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.ComponentName;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Resources;
import android.hardware.display.DisplayManager;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.UserHandle;
import android.provider.Settings;
import androidx.preference.PreferenceFragment;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceCategory;
import androidx.preference.PreferenceManager;
import androidx.preference.PreferenceScreen;
import androidx.preference.TwoStatePreference;
import android.view.Display;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.util.Log;

import com.android.settingslib.display.DisplayDensityUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class DisplayModesSettings extends PreferenceFragment implements RadioGroup.OnCheckedChangeListener {
    private RadioGroup mRadioGroup;
    private DisplayManager mDisplayManager;
    private Display.Mode[] mDisplayModes;
    private Display mDisplay;
    private int mModesCount;
    private ActivityManager mAm;

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mRadioGroup = (RadioGroup) view.findViewById(R.id.radio_group);

        final RadioButton[] rb = new RadioButton[mModesCount];
        for(int i=0; i < mModesCount; i++){
            if (mDisplayModes[i].getPhysicalWidth() == 1440) {
                rb[i]  = new RadioButton(getContext());
                rb[i].setText(" " + mDisplayModes[i].getPhysicalHeight()
                      + "x" + mDisplayModes[i].getPhysicalWidth() + "@"
                      + mDisplayModes[i].getRefreshRate());
                rb[i].setId(i + 1);
                rb[i].setGravity(Gravity.CENTER_VERTICAL);
                rb[i].setTextAppearance(android.R.style.TextAppearance_Medium);
                mRadioGroup.addView(rb[i]);
            }
        }
        int displayMode = 3; //Settings.System.getIntForUser(getContext().getContentResolver(),
                         //Settings.System.OMNI_DISPLAY_MODE, 3, UserHandle.USER_CURRENT);
        mRadioGroup.check(displayMode);
        mRadioGroup.setOnCheckedChangeListener(this);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mAm = (ActivityManager) getContext().getSystemService("activity");
        mDisplayManager = (DisplayManager)getContext().getSystemService(Context.DISPLAY_SERVICE);
        mDisplay = mDisplayManager.getDisplay(0);
        mDisplayModes = mDisplay.getSupportedModes();
        mModesCount = mDisplayModes.length;
        return inflater.inflate(R.layout.display_modes, container, false);
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
            int displayMode = 3; //Settings.System.getIntForUser(getContext().getContentResolver(),
                         //Settings.System.OMNI_DISPLAY_MODE, 3, UserHandle.USER_CURRENT);
            if (checkedId == displayMode)
                return;
            //Settings.System.putInt(getContext().getContentResolver(), Settings.System.OMNI_DISPLAY_MODE, checkedId);
            removeRunningTask();
            killRunningProcess();
    }

    private void removeRunningTask() {
        List<RecentTaskInfo> recentTaskInfos = null;
        try {
            recentTaskInfos = ActivityManager.getService().getRecentTasks(Integer.MAX_VALUE, 2, -2).getList();
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (recentTaskInfos != null) {
            boolean skipSettings = false;
            for (RecentTaskInfo recentTaskInfo : recentTaskInfos) {
                if (!skipSettings) {
                    ComponentName topActivity = recentTaskInfo != null ? recentTaskInfo.topActivity : null;
                    if (topActivity != null && "org.omnirom.device".equals(topActivity.getPackageName())) {
                        skipSettings = true;
                    }
                }
                if (recentTaskInfo != null) {
                    try {
                        ActivityManager.getService().removeTask(recentTaskInfo.persistentId);
                    } catch (RemoteException rex) {
                        //nothing to do
                    }
                }
            }
        }
    }

    private void killRunningProcess() {
        List<RunningAppProcessInfo> runningProcessInfos = mAm.getRunningAppProcesses();
        if (runningProcessInfos != null) {
            for (RunningAppProcessInfo runningProcessInfo : runningProcessInfos) {
                if (runningProcessInfo != null) {
                    if (!isSystemApplication(getContext(), runningProcessInfo.processName)) {
                        if (runningProcessInfo.uid > 10000) {
                                    mAm.killUid(runningProcessInfo.uid, "Update screen resolution");
                        }
                    }
                }
            }
        }
    }

    public boolean isSystemApplication(Context context, String packageName) {
        if (context == null) {
            return false;
        }
        return isSystemApplication(context.getPackageManager(), packageName);
    }

    public boolean isSystemApplication(PackageManager packageManager, String packageName) {
        boolean z = false;
        if (packageManager == null || packageName == null || packageName.length() == 0) {
            return false;
        }
        try {
            ApplicationInfo app = packageManager.getApplicationInfo(packageName, 0);
            if (app != null && (app.flags & 1) > 0) {
                z = true;
            }
            return z;
        } catch (NameNotFoundException e) {
            return false;
        }
    }
}
