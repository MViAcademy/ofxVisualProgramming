/*==============================================================================

    ofxVisualProgramming: A visual programming patching environment for OF

    Copyright (c) 2018 Emanuele Mazza aka n3m3da <emanuelemazza@d3cod3.org>

    ofxVisualProgramming is distributed under the MIT License.
    This gives everyone the freedoms to use ofxVisualProgramming in any context:
    commercial or non-commercial, public or private, open or closed source.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    See https://github.com/d3cod3/ofxVisualProgramming for documentation

==============================================================================*/

#pragma once

#include "PatchObject.h"

#include "ofxAudioAnalyzer.h"
#include "ofxBTrack.h"
#include "ofxHistoryPlot.h"

class AudioAnalyzer : public PatchObject {

public:

    AudioAnalyzer();

    void            newObject();
    void            setupObjectContent(shared_ptr<ofAppGLFWWindow> &mainWindow);
    void            updateObjectContent(map<int,PatchObject*> &patchObjects);
    void            drawObjectContent(ofxFontStash *font);
    void            removeObjectContent();
    void            audioInObject(ofSoundBuffer &inputBuffer);

    void            updateInWindow(ofEventArgs &e);
    void            drawInWindow(ofEventArgs &e);
    void            drawWindowHeader();

    bool            loadAudioSettings();

    void            mouseMovedObjectContent(ofVec3f _m);
    void            dragGUIObject(ofVec3f _m);
    void            mouseMoved(ofMouseEventArgs &e);
    void            mouseDragged(ofMouseEventArgs &e);
    void            mousePressed(ofMouseEventArgs &e);
    void            mouseReleased(ofMouseEventArgs &e);
    void            mouseScrolled(ofMouseEventArgs &e);

    void            onSliderEvent(ofxDatGuiSliderEvent e);

    // Window vars
    std::shared_ptr<ofAppGLFWWindow>        window;
    int                                     window_actual_width, window_actual_height;
    int                                     start_dragging_mouseX, start_dragging_mouseY;
    int                                     start_dragging_mouseXinScreen, start_dragging_mouseYinScreen;
    bool                                    shouldResetDrag;
    ofRectangle                             *windowHeader;
    int                                     windowHeaderHeight;
    ofxFontStash                            *windowFont;
    ofImage                                 *windowHeaderIcon;
    ofxDatGui*                              windowGui;
    ofxDatGuiThemeRetina*                   windowGuiThemeRetina;
    ofxDatGuiValuePlotter*                  rmsPlotter;
    ofxDatGuiLabel*                         rmsLabel;
    ofxDatGuiValuePlotter*                  powerPlotter;
    ofxDatGuiLabel*                         powerLabel;
    ofxDatGuiValuePlotter*                  pitchFreqPlotter;
    ofxDatGuiLabel*                         pitchFreqLabel;
    ofxDatGuiValuePlotter*                  hfcPlotter;
    ofxDatGuiLabel*                         hfcLabel;
    ofxDatGuiValuePlotter*                  centroidPlotter;
    ofxDatGuiLabel*                         centroidLabel;
    ofxDatGuiValuePlotter*                  inharmonicityPlotter;
    ofxDatGuiLabel*                         inharmonicityLabel;
    ofxDatGuiValuePlotter*                  dissonancePlotter;
    ofxDatGuiLabel*                         dissonanceLabel;
    ofxDatGuiValuePlotter*                  rollOffPlotter;
    ofxDatGuiLabel*                         rollOffLabel;
    ofImage                                 *freqDomainBG;
    ofxHistoryPlot                          *bpmPlot;

    // GUI vars
    ofxDatGui*                              gui;
    ofxDatGuiSlider*                        inputLevel;
    ofxDatGuiSlider*                        smoothing;
    float                                   smoothingValue;
    float                                   audioInputLevel;
    bool                                    isOverGui;

    // Audio Input Signal variables
    ofSoundBuffer                           lastBuffer;
    ofSoundBuffer                           monoBuffer;
    ofPolyline                              waveform;
    std::mutex                              audioMutex;

    // Analysis variables
    ofxAudioAnalyzer                        audioAnalyzer;
    ofxBTrack                               *beatTrack;
    vector<float>                           spectrum;
    vector<float>                           melBands;
    vector<float>                           mfcc;
    vector<float>                           hpcp;
    vector<float>                           tristimulus;
    float                                   rms;
    float                                   power;
    float                                   pitchFreq;
    float                                   hfc;
    float                                   centroid;
    float                                   centroidNorm;
    float                                   inharmonicity;
    float                                   dissonance;
    float                                   rollOff;
    float                                   rollOffNorm;
    float                                   bpm;
    bool                                    beat;
    bool                                    isOnset;

    // Object vars
    int                                     numINChannels;
    int                                     actualChannel;
    int                                     bufferSize;
    int                                     sampleRate;

};
