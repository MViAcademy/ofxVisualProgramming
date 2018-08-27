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

#include "AudioDevice.h"

//--------------------------------------------------------------
AudioDevice::AudioDevice() : PatchObject(){

    this->numInlets     = 0;
    this->numOutlets    = 0;

    in_channels         = 0;
    out_channels        = 0;

    sampleRateIN        = 0;
    sampleRateOUT       = 0;

    isSystemObject      = true;

    isAudioINObject     = true;
    isAudioOUTObject    = true;

    deviceLoaded        = false;
    
}

//--------------------------------------------------------------
void AudioDevice::newObject(){
    this->setName("audio device");
}

//--------------------------------------------------------------
void AudioDevice::setupObjectContent(shared_ptr<ofAppGLFWWindow> &mainWindow){
    loadDeviceInfo();
}

//--------------------------------------------------------------
void AudioDevice::updateObjectContent(map<int,PatchObject*> &patchObjects){

}

//--------------------------------------------------------------
void AudioDevice::drawObjectContent(ofxFontStash *font){
    ofSetColor(255);
    ofEnableAlphaBlending();
    //font->draw(ofToString(sampleRateIN),this->fontSize,this->width/2,this->headerHeight*2);
    //font->draw(ofToString(sampleRateOUT),this->fontSize,this->width/2,this->headerHeight*4);
    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void AudioDevice::removeObjectContent(){
    
}

//--------------------------------------------------------------
void AudioDevice::audioInObject(ofSoundBuffer &inputBuffer){
    if(deviceLoaded && in_channels>0){
        if(in_channels == 1){
            inputBuffer.copyTo(IN_CH.at(0), inputBuffer.getNumFrames(), 1, 0);
            *static_cast<ofSoundBuffer *>(_outletParams[0]) = IN_CH.at(0);
        }else{
            for(size_t c=0;c<static_cast<size_t>(in_channels);c++){
                inputBuffer.getChannel(IN_CH.at(c),c);
                *static_cast<ofSoundBuffer *>(_outletParams[c]) = IN_CH.at(c);
            }
        }
    }
}

//--------------------------------------------------------------
void AudioDevice::audioOutObject(ofSoundBuffer &outBuffer){
    if(deviceLoaded && out_channels>0){
        for(size_t c=0;c<static_cast<size_t>(out_channels);c++){
            if(this->inletsConnected[c]){
                monoBuffer = outBuffer;
                OUT_CH.at(c) = *static_cast<ofSoundBuffer *>(_inletParams[c]);
                OUT_CH.at(c).copyTo(monoBuffer, OUT_CH.at(c).getNumFrames(), 1, 0);
                outBuffer.setChannel(monoBuffer,c);
            }
        }
    }
}

//--------------------------------------------------------------
void AudioDevice::resetSystemObject(){
    ofxXmlSettings XML;

    deviceLoaded      = false;

    if (XML.loadFile(patchFile)){
        int totalObjects = XML.getNumTags("object");

        if (XML.pushTag("settings")){
            in_channels  = XML.getValue("input_channels",0);
            out_channels = XML.getValue("output_channels",0);
            sampleRateIN = XML.getValue("sample_rate_in",0);
            sampleRateOUT= XML.getValue("sample_rate_out",0);
            XML.popTag();
        }

        this->numInlets  = out_channels;
        this->numOutlets = in_channels;

        IN_CH.clear();
        OUT_CH.clear();

        for( int i = 0; i < out_channels; i++){
            _inletParams[i] = new ofSoundBuffer();
            ofSoundBuffer temp;
            OUT_CH.push_back(temp);
        }

        for( int i = 0; i < in_channels; i++){
            _outletParams[i] = new ofSoundBuffer();
            ofSoundBuffer temp;
            IN_CH.push_back(temp);
        }

        this->inlets.clear();
        this->inletsNames.clear();

        for( int i = 0; i < out_channels; i++){
            this->addInlet(VP_LINK_AUDIO,"OUT_"+ofToString(i));
        }

        this->outlets.clear();
        for( int i = 0; i < in_channels; i++){
            this->addOutlet(VP_LINK_AUDIO);
        }

        this->inletsConnected.clear();
        this->initInletsState();

        // Save new object config
        for(int i=0;i<totalObjects;i++){
            if(XML.pushTag("object", i)){
                if(XML.getValue("id", -1) == this->nId){
                    // Dynamic reloading outlets
                    XML.removeTag("outlets");
                    int newOutlets = XML.addTag("outlets");
                    if(XML.pushTag("outlets",newOutlets)){
                        for(int j=0;j<static_cast<int>(this->outlets.size());j++){
                            int newLink = XML.addTag("link");
                            if(XML.pushTag("link",newLink)){
                                XML.setValue("type",this->outlets.at(j));
                                XML.popTag();
                            }
                        }
                        XML.popTag();
                    }
                }else{
                    // remove links to the this object
                    if(XML.pushTag("outlets")){
                        int totalLinks = XML.getNumTags("link");
                        for(int l=0;l<totalLinks;l++){
                            if(XML.pushTag("link",l)){
                                int totalTo = XML.getNumTags("to");
                                vector<bool> delLinks;
                                for(int t=0;t<totalTo;t++){
                                    if(XML.pushTag("to",t)){
                                        if(XML.getValue("id", -1) == this->nId){
                                            delLinks.push_back(true);
                                        }else{
                                            delLinks.push_back(false);
                                        }
                                        XML.popTag();
                                    }
                                }
                                for(int d=delLinks.size()-1;d>=0;d--){
                                    if(delLinks.at(d)){
                                        XML.removeTag("to",d);
                                    }
                                }
                                XML.popTag();
                            }
                        }
                        XML.popTag();
                    }
                }
                XML.popTag();
            }
        }

        XML.saveFile();

        deviceLoaded      = true;
    }
}

//--------------------------------------------------------------
void AudioDevice::loadDeviceInfo(){
    ofxXmlSettings XML;

    if (XML.loadFile(patchFile)){
        if (XML.pushTag("settings")){
            in_channels  = XML.getValue("input_channels",0);
            out_channels = XML.getValue("output_channels",0);
            sampleRateIN = XML.getValue("sample_rate_in",0);
            sampleRateOUT= XML.getValue("sample_rate_out",0);
            XML.popTag();
        }

        bool isNewObject = true;

        int totalObjects = XML.getNumTags("object");
        for (int i=0;i<totalObjects;i++){
            if(XML.pushTag("object", i)){
                if(XML.getValue("id",-1) == this->nId){
                    isNewObject = false;
                }
                XML.popTag();
            }
        }

        this->numInlets  = out_channels;
        this->numOutlets = in_channels;

        IN_CH.clear();
        OUT_CH.clear();

        for( int i = 0; i < out_channels; i++){
            _inletParams[i] = new ofSoundBuffer();
            ofSoundBuffer temp;
            OUT_CH.push_back(temp);
        }

        for( int i = 0; i < in_channels; i++){
            _outletParams[i] = new ofSoundBuffer();
            ofSoundBuffer temp;
            IN_CH.push_back(temp);
        }

        this->inlets.clear();
        this->inletsNames.clear();

        for( int i = 0; i < out_channels; i++){
            this->addInlet(VP_LINK_AUDIO,"OUT_"+ofToString(i));
        }

        if(isNewObject){
            this->outlets.clear();
            for( int i = 0; i < in_channels; i++){
                this->addOutlet(VP_LINK_AUDIO);
            }
        }

        this->inletsConnected.clear();
        this->initInletsState();

        deviceLoaded      = true;
    }
}
