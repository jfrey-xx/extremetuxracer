/* 
 * ETRacer 
 * Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "event_select.h"

#include "ppgltk/audio/audio.h"

#include "course_load.h"
#include "textures.h"
#include "joystick.h"

#include "player.h"
#include "game_mgr.h"
	
EventSelect::EventSelect()
{
	if(GameMode::prevmode==EVENT_RACE_SELECT){
		m_curEvent = gameMgr->currentEvent;
		m_curCup = gameMgr->currentCup;
		if(players[0].isCupComplete((*m_curEvent).name,(*m_curCup).name)){
			if(m_curCup != --(*m_curEvent).cupList.end()){
				m_curCup++;
			}				
		}
	}else{
		m_curEvent = eventList.begin();
		m_curCup = (*m_curEvent).cupList.begin();
	}
	
		
	pp::Vec2d pos( getparam_x_resolution()/2,
				   getparam_y_resolution()/2+90);
	
	mp_titleLbl = new pp::Label(pos,"menu_label",_("Select campaign and cup"));
	mp_titleLbl->alignment.center();
	
	
	pos.x-=200;
	pos.y-=40;
	mp_eventLbl = new pp::Label(pos,"event_and_cup_label",_("Campaign:"));	
	
	pos.y-=50;
	mp_eventListbox = new pp::Listbox<EventData>( pos,
				   pp::Vec2d(400, 40),
				   "listbox_item",
				   eventList);
	mp_eventListbox->setCurrentItem( m_curEvent );
	mp_eventListbox->signalChange.Connect(pp::CreateSlot(this,&EventSelect::eventChanged));
	
	pos.y-=40;
	mp_cupLbl = new pp::Label(pos,"event_and_cup_label",_("Cup:"));
		
	pos.y-=50;
	mp_cupListbox_size = pp::Vec2d(400, 40);
	mp_cupListbox_pos = pos;
	mp_cupListbox = new pp::Listbox<CupData>( mp_cupListbox_pos,
				   mp_cupListbox_size,
				   "listbox_item",
				   (*m_curEvent).cupList );
	mp_cupListbox->setCurrentItem( m_curCup );	
    mp_cupListbox->signalChange.Connect(pp::CreateSlot(this,&EventSelect::cupChanged));

	pos.y-=30;
	mp_statusLbl = new pp::Label( pp::Vec2d(pos.x+200,pos.y),
								  "cup_status","");
	mp_statusLbl->alignment.center();

     pos.y-=80;
     pp::Vec2d size(176,32);
     pp::Vec2d pos2(pos.x+150,pos.y-10);
     mp_nameLbl = new pp::Label( pos,"event_and_cup_label",_("Player name:"));
	mp_nameEnt = new pp::Entry(pos2,size,"event_and_cup_label",players[0].name.c_str());
	mp_nameEnt->setMaxChars(13);

     pos.y-=65;
	pos.x-=50;	
	mp_backBtn = new pp::Button( pos,
			      pp::Vec2d(150, 40), 
			      "button_label", 
			      _("Back") );
    mp_backBtn->setHilitFontBinding( "button_label_hilit" );
    mp_backBtn->signalClicked.Connect(pp::CreateSlot(this,&EventSelect::back));

	pos.x+=350;	
	mp_continueBtn = new pp::Button( pos,
			       pp::Vec2d(150, 40),
			       "button_label",
			       _("Continue") );
    mp_continueBtn->setHilitFontBinding( "button_label_hilit" );
    mp_continueBtn->setDisabledFontBinding( "button_label_disabled" );
    mp_continueBtn->signalClicked.Connect(pp::CreateSlot(this,&EventSelect::apply));
    
    
    pos.x+=50;
    pos.y+=50;
    
    #define MODELID_TUX 0
    #define MODELID_SAMUEL 1
    #define MODELID_TRIXI 2
    #define MODELID_SPEEDY 3
    // model icon
	mp_modelSSBtn = new pp::SSButton( pos,
				      pp::Vec2d(89, 99),
				      4 );
    mp_modelSSBtn->setStateImage(MODELID_TUX,"modelpreviews_button",
				  pp::Vec2d( 0.0, 0.0 ),
				  pp::Vec2d( 0.25, 1.0 ),
				  pp::Color::white );
	mp_modelSSBtn->setStateImage(MODELID_SAMUEL,"modelpreviews_button",
				  pp::Vec2d( 0.25, 0.0 ),
				  pp::Vec2d( 0.50, 1.0 ),
				  pp::Color::white );

	mp_modelSSBtn->setStateImage(MODELID_TRIXI,"modelpreviews_button",
				  pp::Vec2d( 0.50, 0.0 ),
				  pp::Vec2d( 0.75, 1.0 ),
				  pp::Color::white );
                  
    mp_modelSSBtn->setStateImage(MODELID_SPEEDY,"modelpreviews_button",
				  pp::Vec2d( 0.75, 0.0 ),
				  pp::Vec2d( 1.0, 1.0 ),
				  pp::Color::white );
    mp_modelSSBtn->signalClicked.Connect(pp::CreateSlot(this,&EventSelect::buttonModelChange));

    m_modelList = ModelHndl->l_models;
    pos.y -= -45;
    pos.x -= 200;
    mp_modelEnt = new pp::Listbox<model_t>( pos, pp::Vec2d(139, 32), "listbox_item", m_modelList);
    mp_modelEnt->signalChange.Connect(pp::CreateSlot(this,&EventSelect::listboxModelChange));
    std::list<model_t>::iterator modelit = mp_modelEnt->getCurrentItem();
    mp_modelSSBtn->setState(modelit->id);
    
	
	updateCupStates();
	updateButtonEnabledStates();
}

EventSelect::~EventSelect()
{
	delete mp_backBtn;
	delete mp_continueBtn;	
	delete mp_eventListbox;
	delete mp_cupListbox;
	
	delete mp_titleLbl;
	delete mp_eventLbl;
	delete mp_cupLbl;	
	delete mp_statusLbl;
	delete mp_nameLbl;
    
    //if we don't delete the buttons, the game just crashes when the player leaves the event selection screen
    delete mp_modelSSBtn;
    delete mp_modelEnt;
	
	delete mp_nameEnt;
}

void
EventSelect::loop(float timeStep)
{
    update_audio();
    set_gl_options( GUI );
    clear_rendering_context();
    UIMgr.setupDisplay();
	drawSnow(timeStep);
    theme.drawMenuDecorations();
    UIMgr.draw();
    reshape( getparam_x_resolution(), getparam_y_resolution() );
    winsys_swap_buffers();
}

void
EventSelect::updateCupStates()
{
	m_curCupComplete=false;
	m_curCupPlayable=false;
	
	if ( players[0].isCupComplete( (*m_curEvent).name, (*m_curCup).name) ) {
		m_curCupComplete=true;
		m_curCupPlayable=true;
    } else if ( players[0].isFirstIncompleteCup( m_curEvent, m_curCup ) ) {
		m_curCupPlayable=true;
    }
	
	const char* string;
	if(m_curCupComplete){
		string = _("You've won this cup!");
	}else if (m_curCupPlayable){
		string = _("You must complete this cup next");
	}else{
		string = _("You cannot enter this cup yet"); 
	}
	mp_statusLbl->setText(string);
}

void
EventSelect::updateButtonEnabledStates()
{
	if(m_curCupPlayable){
		mp_continueBtn->setSensitive(true);
	}else{
		mp_continueBtn->setSensitive(false);
	}
}

void
EventSelect::cupChanged()
{
	m_curCup = mp_cupListbox->getCurrentItem();
	updateCupStates();
	updateButtonEnabledStates();
}

void
EventSelect::eventChanged()
{
	m_curEvent = mp_eventListbox->getCurrentItem();
	m_curCup = (*m_curEvent).cupList.begin();
	delete mp_cupListbox;
	mp_cupListbox = new pp::Listbox<CupData>( mp_cupListbox_pos,
				   mp_cupListbox_size,
				   "listbox_item",
				   (*m_curEvent).cupList );
	mp_cupListbox->setCurrentItem( m_curCup );	
	mp_cupListbox->signalChange.Connect(pp::CreateSlot(this,&EventSelect::cupChanged));
	updateCupStates();
	updateButtonEnabledStates();
}

void EventSelect::listboxModelChange() {
    std::list<model_t>::iterator modelit = mp_modelEnt->getCurrentItem();
    mp_modelSSBtn->setState(modelit->id);
}

void EventSelect::buttonModelChange() {
    std::list<model_t>::iterator modelit = mp_modelEnt->getCurrentItem();
    modelit++;
    if(modelit!=mp_modelEnt->getItemList().end()) {
        mp_modelEnt->setCurrentItem(modelit);
    } else {
        mp_modelEnt->setCurrentItem(mp_modelEnt->getItemList().begin());
    }
}

void
EventSelect::back()
{
	set_game_mode( GAME_TYPE_SELECT );
	UIMgr.setDirty();
}

void
EventSelect::apply()
{
	if(mp_continueBtn->getSensitive()) {
		players[0].name = mp_nameEnt->getContent();
	     m_curEvent = mp_eventListbox->getCurrentItem();
		m_curCup = mp_cupListbox->getCurrentItem();
        


		gameMgr->setupEventAndCup(	m_curEvent,
									m_curCup );

		if(!getparam_always_save_event_race_data()){	
			if(!players[0].isCupComplete((*m_curEvent).name, (*m_curCup).name )){
				players[0].clearCupData((*m_curEvent).name, (*m_curCup).name);
			}
		}
		players[0].resetLives();
        std::list<model_t>::iterator modelit = mp_modelEnt->getCurrentItem();
        ModelHndl->load_model((*modelit).id);
	    set_game_mode( EVENT_RACE_SELECT );
	    UIMgr.setDirty();
    }
}

bool
EventSelect::keyPressEvent(SDLKey key)
{
	switch (key){
		case SDLK_UP:
			mp_cupListbox->gotoPrevItem();
	    	return true;
		case SDLK_DOWN:
			mp_cupListbox->gotoNextItem();
	    	return true;
		case SDLK_LEFT:
			mp_eventListbox->gotoPrevItem();
	    	return true;
		case SDLK_RIGHT:
			mp_eventListbox->gotoPrevItem();
	    	return true;
		case SDLK_RETURN: 
	    	mp_continueBtn->simulateMouseClick();
			UIMgr.setDirty();
			return true;
		case SDLK_ESCAPE: 
	    	mp_backBtn->simulateMouseClick();
			UIMgr.setDirty();
			return true;
		default:
			return false;
	}	
}