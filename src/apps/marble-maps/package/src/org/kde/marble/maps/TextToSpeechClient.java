//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

package org.kde.marble.maps;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.util.Log;
import android.content.Context;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.media.AudioAttributes;
import java.util.Locale;
import java.lang.String;

public class TextToSpeechClient extends QtActivity implements TextToSpeech.OnInitListener
{
    private static TextToSpeech m_tts = null;
    private static TextToSpeechClient m_instance = null;
    private static final String TAG = "org.kde.marble.maps.TextToSpeechClient";
    private static String m_language = "en";
    
    public TextToSpeechClient()
    {
        m_instance = this;
    }
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
    }
    
    public void initSpeaker(String language)
    {
        if (m_tts != null) {
            m_tts.shutdown();
        }

        m_tts = new TextToSpeech(this, this);
        m_language = language;
    }

    public void onInit(int status)
    {
        if (status == TextToSpeech.SUCCESS) {
            Locale locale = new Locale(m_language);
            if(m_tts.isLanguageAvailable(locale) >= TextToSpeech.LANG_AVAILABLE) {
                m_tts.setLanguage(locale);
            } else {
                Log.i(TAG, "Text to speech language " + m_language + " is not available");
            }
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP_MR1) {
                m_tts.setAudioAttributes(new AudioAttributes.Builder().setUsage(AudioAttributes.USAGE_MEDIA).setContentType(AudioAttributes.USAGE_ASSISTANCE_NAVIGATION_GUIDANCE).build());
            }
        }
    }

    @Override
    protected void onDestroy() {
            super.onDestroy();
            if (m_tts != null) {
                m_tts.shutdown();
            }
    }

    public static void read(String msg)
    {
        if (m_tts != null) {
            m_tts.speak(msg, TextToSpeech.QUEUE_FLUSH, null);
        }
    }
    
    public static void initSpeakerStatic(String locale)
    {
        if (m_instance != null) {
            m_instance.initSpeaker(locale);
        }
    }
}
