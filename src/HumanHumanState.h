#pragma once

#include "BaseStrokeState.h"

class HumanHumanState : public BaseStrokeState
{
public:
	void stateEnter();
	void stateExit();

	void update();
	void draw();
	void debugDraw();

	void setupIdea( int pointNum );
	void setupImplementation();

	std::string getName();
	void setupGUI();
	void guiEvent( ofxUIEventArgs &e );
	void mouseMoved( int x, int y );
	void mousePressed( int x, int y, int key );
	void mouseReleased( int x, int y, int button );
	void keyPressed( int key );

private:
	bool recording, recorded, allowParallels;
	std::string setupIdeaButtonLabel, setupImplementationButtonLabel, startAnimationButtonLabel, allowParallelsButtonLabel;
	float animationSpeed;
	int currentPointIndex;
	int currentDrawingIndex, currentMouseDataIndex;

	ofVec2f currentMouse;
	ofVec2f getCurrentMouse();

	std::vector<std::vector<ofVec2f>> mouseData;
};
