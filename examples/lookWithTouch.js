//
//  lookWithTouch.js
//  hifi
//
//  Created by Brad Hefta-Gaub on 1/28/14.
//  Copyright (c) 2014 HighFidelity, Inc. All rights reserved.
//
//  This is an example script that demonstrates use of the Controller class
//
//

var lastX = 0;
var lastY = 0;
var yawFromMouse = 0;
var pitchFromMouse = 0;
var wantDebugging = false;

function touchBeginEvent(event) {
    if (wantDebugging) {
        print("touchBeginEvent event.x,y=" + event.x + ", " + event.y);
    }
    lastX = event.x;
    lastY = event.y;
}

function touchEndEvent(event) {
    if (wantDebugging) {
        print("touchEndEvent event.x,y=" + event.x + ", " + event.y);
    }
}

function touchUpdateEvent(event) {
    if (wantDebugging) {
        print("touchUpdateEvent event.x,y=" + event.x + ", " + event.y);
    }

    var MOUSE_YAW_SCALE = -0.25;
    var MOUSE_PITCH_SCALE = -12.5;
    var FIXED_MOUSE_TIMESTEP = 0.016;
    yawFromMouse += ((event.x - lastX) * MOUSE_YAW_SCALE * FIXED_MOUSE_TIMESTEP);
    pitchFromMouse += ((event.y - lastY) * MOUSE_PITCH_SCALE * FIXED_MOUSE_TIMESTEP);
    lastX = event.x;
    lastY = event.y;
}

function update() {
    // rotate body yaw for yaw received from mouse
    var newOrientation = Quat.multiply(MyAvatar.orientation, Quat.fromVec3( { x: 0, y: yawFromMouse, z: 0 } ));
    if (wantDebugging) {
        print("changing orientation"
            + " [old]MyAvatar.orientation="+MyAvatar.orientation.x + "," + MyAvatar.orientation.y + "," 
            + MyAvatar.orientation.z + "," + MyAvatar.orientation.w
            + " newOrientation="+newOrientation.x + "," + newOrientation.y + "," + newOrientation.z + "," + newOrientation.w);
    }
    MyAvatar.orientation = newOrientation;
    yawFromMouse = 0;

    // apply pitch from mouse
    var newPitch = MyAvatar.headPitch + pitchFromMouse;
    if (wantDebugging) {
        print("changing pitch [old]MyAvatar.headPitch="+MyAvatar.headPitch+ " newPitch="+newPitch);
    }
    MyAvatar.headPitch = newPitch;
    pitchFromMouse = 0;
}

// Map the mouse events to our functions
Controller.touchBeginEvent.connect(touchBeginEvent);
Controller.touchUpdateEvent.connect(touchUpdateEvent);
Controller.touchEndEvent.connect(touchEndEvent);

// disable the standard application for mouse events
Controller.captureTouchEvents();

function scriptEnding() {
    // re-enabled the standard application for mouse events
    Controller.releaseTouchEvents();
}

MyAvatar.bodyYaw = 0;
MyAvatar.bodyPitch = 0;
MyAvatar.bodyRoll = 0;

// would be nice to change to update
Script.willSendVisualDataCallback.connect(update);
Script.scriptEnding.connect(scriptEnding);
