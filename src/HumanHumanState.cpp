#include "HumanHumanState.h"
#include "SharedData.h"

HumanHumanState::HumanHumanState()
{
	this->currentMouseImportExportIndex = currentPointsImportExportIndex = 0;
	fbo.allocate( ofGetWindowWidth(), ofGetWindowHeight() );

	warper.setSourceRect( ofRectangle( 0, 0, ofGetWindowWidth(), ofGetWindowHeight() ) );              // this is the source rectangle which is the size of the image and located at ( 0, 0 )
	warper.setTopLeftCornerPosition( ofPoint( 0, 0 ) );             // this is position of the quad warp corners, centering the image on the screen.
	warper.setTopRightCornerPosition( ofPoint( ofGetWindowWidth(), 0 ) );        // this is position of the quad warp corners, centering the image on the screen.
	warper.setBottomLeftCornerPosition( ofPoint( 0, ofGetWindowHeight() ) );      // this is position of the quad warp corners, centering the image on the screen.
	warper.setBottomRightCornerPosition( ofPoint( ofGetWindowWidth(), ofGetWindowHeight() ) ); // this is position of the quad warp corners, centering the image on the screen.
	warper.setup();
}

HumanHumanState::~HumanHumanState()
{

}


void HumanHumanState::stateEnter()
{
	BaseStrokeState::setupPointCount( 100 );
	this->state = INIT;
	this->allowParallels = true;
	animationSpeed = 1.0f;
	*this->getPointCount() = 200;
	currentPointIndex = 0;
	currentDrawingIndex = 0;
	currentMouseDataIndex = 0;
	this->currentMouse = ofVec2f( ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 );
	
	//if( this->data.mouseData.empty() )
	{
		this->state = INIT;
	}

	setupGUI();
	this->gui->setVisible( false );
	//this->gui->setVisible( true );
	this->importPointData();
	this->importMouseData();
	this->state = DONE;
}

void HumanHumanState::stateExit()
{
	this->gui->setVisible( false );
}

void HumanHumanState::setupIdea( int pointNum )
{
	this->state = RECORDING;
}

void HumanHumanState::setupImplementation()
{
	this->state = DONE;
}

std::string HumanHumanState::getName()
{
	return getSharedData().HUMAN_HUMAN;
}

void HumanHumanState::setupGUI()
{
	setupIdeaButtonLabel = "SETUP_IDEA (i)";
	setupImplementationButtonLabel = "SETUP IMPLEMENTATION (a)";
	startAnimationButtonLabel = "START ANIMATION";
	allowParallelsButtonLabel = "ALLOW_PARALLELS (p)";

	gui = new ofxUISuperCanvas( getSharedData().HUMAN_HUMAN ); //shared_ptr<ofxUISuperCanvas>(new ofxUISuperCanvas( "COMPUTER_HUMAN" ) );

	gui->addSlider("POINTNUM", 5, 200, getPointCount() );
	gui->addSpacer();
	gui->addLabelButton( this->setupIdeaButtonLabel, false );
	gui->addSpacer();
	gui->addLabelButton( this->setupImplementationButtonLabel, false );
	gui->addSpacer();

	gui->addSpacer();
	gui->addLabelButton( this->startAnimationButtonLabel, false );
	gui->addSpacer();
	gui->addLabelButton( this->allowParallelsButtonLabel, false );
	gui->addSpacer();

	gui->addSlider( "ANIMATIONSPEED", animationSpeed, 50, &animationSpeed );
	gui->addSpacer();

	std::vector<std::string> stateNames;
	stateNames.push_back( getSharedData().COMPUTER_COMPUTER + " (1)" );
	stateNames.push_back( getSharedData().COMPUTER_HUMAN + " (2)" );
	stateNames.push_back( getSharedData().HUMAN_COMPUTER + " (3)" );
	stateNames.push_back( getSharedData().HUMAN_HUMAN + " (4)" );

	gui->addRadio( getSharedData().stateSelectionRadioButton, stateNames, OFX_UI_ORIENTATION_VERTICAL );

	gui->setGlobalButtonDimension( OFX_UI_GLOBAL_BUTTON_DIMENSION );
	gui->autoSizeToFitWidgets();
	ofAddListener( gui->newGUIEvent, this, &HumanHumanState::guiEvent );
}

void HumanHumanState::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	if( kind == OFX_UI_WIDGET_LABELBUTTON && e.getButton()->getValue() == 0 )
	{
		if( name == this->setupIdeaButtonLabel )
		{
			this->currentData.pointData.clear();
			setupIdea( *this->getPointCount() );
		}

		else if ( name == this->startAnimationButtonLabel )
		{
			switch( this->state )
			{
			case RUNNING:
				this->state = INIT;
				break;
			case INIT:
				this->state = RECORDING;
			}
			currentPointIndex = 0;
		}
		else if( name == this->allowParallelsButtonLabel )
		{
			this->allowParallels = !allowParallels;
		}
	}

	if( name == getSharedData().stateSelectionRadioButton )
	{
		ofxUIRadio *radio = ( ofxUIRadio *) e.widget;
		changeState( radio->getActiveName() );
	}
}

void HumanHumanState::mouseMoved(int x, int y)
{
	this->currentMouse = ofVec2f( x, y );
	if( this->state == RECORDING )
	{
		ofVec2f v( x, y );
		if( currentPointIndex > 0 )
		{
			//this->data.mouseData.push_back( v );
			this->currentData.currMouseData[currentPointIndex - 1].push_back( v );
		}
	}
}

void HumanHumanState::mousePressed(int x, int y, int key)
{
}

void HumanHumanState::mouseReleased(int x, int y, int button)
{
	if( this->state == RECORDING )
	{
		this->currentData.currMouseData.resize( currentPointIndex + 1 );
		this->currentData.pointData.push_back( ofVec2f( x, y ) );
		currentPointIndex++;
	}
}


void HumanHumanState::keyPressed(int key)
{
	BaseStrokeState::keyPressed( key );

	switch( key )
	{
	case 'a':
		setupImplementation();
		break;
	case 'i':
		setupIdea( *getPointCount() );
		break;
	}
}

void HumanHumanState::update()
{
	if( this->state == DONE )
	{
		currentMouseDataIndex += animationSpeed;
	}
}

void HumanHumanState::draw()
{
	BaseStrokeState::beforeDrawing();
	BaseStrokeState::setupColors();

	if( allowParallels && this->state == RECORDING )
	{
		debugDraw();
		if( this->currentData.pointData.size() > 0 )
		{
			ofVec2f from = this->currentData.pointData.at( this->currentData.pointData.size() - 1 );
			ofVec2f to = this->getCurrentMouse();
			from.limited( from.length() - 2 );
			from += (to - from).getNormalized() * 2 / 2;
			to += (from - to).getNormalized() * 2;
			ofLine( from, to );
			ofEllipse( from, getSharedData().strokeWeight, getSharedData().strokeWeight );
			ofEllipse( to, getSharedData().strokeWeight, getSharedData().strokeWeight );
		}

	} 
	if( this->state == RECORDING ) 
	{
		debugDraw();
	}

	if( this->state == DONE )
	{

		for( size_t i = 0; i < currentDrawingIndex; i++ ) 
		{
			ofVec2f from = this->currentData.pointData.at( i );
			ofVec2f to = this->currentData.pointData.at( i + 1 );
			from.limited( from.length() - 2 );
			from += (to - from).getNormalized() * 2 / 2;
			to += (from - to).getNormalized() * 2;
			ofLine( from, to );
			ofEllipse( from, getSharedData().strokeWeight, getSharedData().strokeWeight );
			ofEllipse( to, getSharedData().strokeWeight, getSharedData().strokeWeight );
		}

		
		
		if( currentMouseDataIndex > 0)
		{
			ofVec2f from = this->currentData.pointData.at( currentDrawingIndex );
			//std::cout << this->currentData.currMouseData.size() << " " << this->currentData.currMouseData.at(currentDrawingIndex).size() << " " << this->currentData.pointData.size() << std::endl;
			ofVec2f to = this->currentData.currMouseData.at( currentDrawingIndex ).at( currentMouseDataIndex - 1 );
			from.limited( from.length() - 2 );
			from += (to - from).getNormalized() * 2 / 2;
			to += (from - to).getNormalized() * 2;
			ofLine( from, to );
			ofEllipse( from, getSharedData().strokeWeight, getSharedData().strokeWeight );
			ofEllipse( to, getSharedData().strokeWeight, getSharedData().strokeWeight );
		}

		if( currentMouseDataIndex > this->currentData.currMouseData.at(currentDrawingIndex).size() - 1 )
		{
			currentDrawingIndex++;
			if( currentDrawingIndex + 1 >= this->currentData.pointData.size() )
			{
				currentDrawingIndex = 0;
				//currentMouseImportExportIndex++;
				//currentPointsImportExportIndex++;
				end();
			}
			currentMouseDataIndex = 0;
		}
	}

	BaseStrokeState::afterDrawing();
}

void HumanHumanState::debugDraw()
{
	for( size_t i = 0; i < this->currentData.pointData.size() - 1 && this->currentData.pointData.size() > 1; i++ ) 
	{
		ofVec2f from = this->currentData.pointData.at( i );
		ofVec2f to = this->currentData.pointData.at( i + 1 );
		from.limited( from.length() - 2 );
		from += (to - from).getNormalized() * 2 / 2;
		to += (from - to).getNormalized() * 2;
		ofLine( from, to );
		ofEllipse( from, getSharedData().strokeWeight, getSharedData().strokeWeight );
		ofEllipse( to, getSharedData().strokeWeight, getSharedData().strokeWeight );
	}
}

ofVec2f HumanHumanState::getCurrentMouse()
{
	return this->currentMouse;
}

void HumanHumanState::end()
{
	changeState( getSharedData().HUMAN_COMPUTER );
}