// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

#include "sound.h"

CAudio Audio;
CMusic Music;
CSound Sound;

/*
Used data types of SDL_mixer

Mix_Music
Mix_Chunk

Used function of SDL_mixer:

Mix_OpenAudio
Mix_CloseAudio
Mix_QuerySpec
Mix_HaltMusic
Mix_PlayingMusic
Mix_PlayMusic
Mix_LoadMUS
Mix_VolumeMusic
Mix_Volume
Mix_VolumeChunk
Mix_PlayChannel
Mix_HaltChannel
Mix_LoadWAV
*/

// --------------------------------------------------------------------
//				class CAudio
// --------------------------------------------------------------------

CAudio::CAudio () {}
CAudio::~CAudio () {}

void CAudio::Init () {
	if (SDL_Init (SDL_INIT_AUDIO) < 0) 
	    Message ("Couldn't initialize SDL", SDL_GetError());
	int hz = 22050;
   	Uint16 format = AUDIO_S16SYS;
    int channels = 2;
	int buffer = 2048;
	if (Mix_OpenAudio (hz, format, channels, buffer) < 0)
		Message ("could not open mixer");
}

void CAudio::Shutdown () {
	if (IsOpen()) Mix_CloseAudio();
}

bool CAudio::IsOpen() {
    int freq;
    Uint16 format;
    int channels;
    return (bool) Mix_QuerySpec (&freq, &format, &channels);
}

void CAudio::Load () {
	Music.LoadFiles ();
	Sound.LoadFiles ();
}

void CAudio::Update () {
	Music.Update ();
	Sound.Update ();
}

// --------------------------------------------------------------------
//				class CMusic
// --------------------------------------------------------------------

CMusic::CMusic () {
	MusicIndex = ""; 
	curr_name = "";
	curr_idx = -1;
	numMusics = 0;
}

void CMusic::SetStatus (string name, bool playing) {
	int idx = SPIntN (MusicIndex, name, -1);
	if (idx < 0) return;
	musics[idx]->playing = playing;
}

void CMusic::SetStatus2 (int idx, bool playing) {
	musics[idx]->playing = playing;
}

bool CMusic::Play (string name) {
    if (!Audio.IsOpen()) return false;
	
	int idx = SPIntN (MusicIndex, name, -1);
	if (idx < 0) return false;
	TMusic *music = musics[idx];
    if  (curr_idx != idx) {
		if  (curr_name.size() > 0) SetStatus (curr_name, false);
		Mix_HaltMusic();
    }

    if  (!Mix_PlayingMusic ()) {
		Mix_PlayMusic (music->music, music->loop);
		SetStatus (music->name, true);
    }
    curr_idx = idx;
    curr_name = music->name;
	return true;
}

int CMusic::LoadPiece (const char *name, const char *filename, int loop) {
    if (!Audio.IsOpen ()) return -1;
	if (numMusics >= MAX_MUSICS) return -1; 
	Mix_Music *load = Mix_LoadMUS (filename);
	if (load == 0) return -1;

	musics[numMusics] = new TMusic;
	musics[numMusics]->name = name;
	musics[numMusics]->music = load;
	musics[numMusics]->playing = False;
	musics[numMusics]->loop = loop;

	MusicIndex += "[";	
	MusicIndex += name;
	MusicIndex += "]";	
	MusicIndex += Int_StrN (numMusics);

	numMusics++;
	return numMusics-1;
}

void CMusic::LoadFiles () {
	CSPList list(200);
	int loops;
	string name;
	string musicfile;
	string path;
	string line;
	int musid;
	if (list.Load (cfg.music_dir, "music.lst")) { 
		for (int i=0; i<list.Count(); i++) {
			line = list.Line(i);
			loops = SPIntN (line, "loops", -1);
			name = SPStrN (line, "name", "");		
			musicfile = SPStrN (line, "file", "");		
			path = MakePathStr (cfg.music_dir, musicfile);
			musid = LoadPiece (name.c_str(), path.c_str(), loops);
		}
	}
}

void CMusic::Update () {
    if (!Audio.IsOpen()) return;

    if (!cfg.play_music) Mix_HaltMusic();
    int volume = cfg.music_volume;
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    cfg.music_volume = volume;
	Mix_VolumeMusic (volume);

    if  (curr_name.size() > 0 && !Mix_PlayingMusic ()) {
		SetStatus (curr_name, false);
		curr_name = "";
		curr_idx = -1;
    }
}

// --------------------------------------------------------------------
//				class CSound
// --------------------------------------------------------------------

CSound::CSound () {
	SoundIndex = ""; 
	numSounds = 0;
}

void CSound::Update () {
    if (!Audio.IsOpen()) return;
    int volume = cfg.sound_volume;
    if  (volume < 0) volume = 0;
    if  (volume > 128) volume = 128;
    cfg.sound_volume = volume;
    Mix_Volume (-1, volume); 
}

void CSound::Play (int soundid, int loop) {
    if (!Audio.IsOpen()) return;
	TSound *sound = sounds[soundid];
	if (sound->chunk == NULL) return;
	if (sound->loop_count < 0) return;
    Mix_VolumeChunk (sound->chunk, sound->volume);  
    sound->channel = Mix_PlayChannel (-1, sound->chunk, loop);
    sound->loop_count = loop;
}

void CSound::Play (int soundid, int loop, int volume) {
    if (!Audio.IsOpen()) return;
	TSound *sound = sounds[soundid];
	if (sound->chunk == NULL) return;
	if (sound->loop_count < 0) return;
    Mix_VolumeChunk (sound->chunk, volume);  
    sound->channel = Mix_PlayChannel (-1, sound->chunk, loop);
    sound->loop_count = loop;
}

void CSound::Play (string name, int loop) {
	int idx = SPIntN (SoundIndex, name, -1);
	if (idx < 0) return;
	Play (idx, loop);
}

void CSound::Play (string name, int loop, int volume) {
	int idx = SPIntN (SoundIndex, name, -1);
	if (idx < 0) return;
	Play (idx, loop, volume);
}

void CSound::Halt (int soundid) {
    if (!Audio.IsOpen()) return;
	TSound *sound = sounds[soundid];
	if (sound->chunk == 0) return;
	if (sound->loop_count < 0) Mix_HaltChannel (sound->channel);
    sound->loop_count = 0;
   	sound->channel = 0;
}

void CSound::Halt (string name) {
	int idx = SPIntN (SoundIndex, name, -1);
	if (idx < 0) return;
	Halt (idx);
}

void CSound::SetVolume (int soundid, int volume) {
    if (!Audio.IsOpen()) return;
	TSound *sound = sounds[soundid];
	if (sound->chunk == 0) return;
	sound->volume = volume;
	Mix_VolumeChunk (sound->chunk, sound->volume);
}

void CSound::SetVolume (string name, int volume) {
	int idx = SPIntN (SoundIndex, name, -1);
	if (idx < 0) return;
	SetVolume (idx, volume);
}

int CSound::LoadChunk (const char *name, const char *filename) {
    if (!Audio.IsOpen()) return -1;
	if (numSounds >= MAX_SOUNDS) return -1; 
	Mix_Chunk *load = Mix_LoadWAV (filename);
	if (load == NULL) return -1;

	sounds[numSounds] =  new TSound;
	sounds[numSounds]->chunk = load;
//	sounds[numSounds]->volume = 100;
//	sounds[numSounds]->channel = 0;
//	sounds[numSounds]->loop_count = 0;

	SoundIndex += "[";	
	SoundIndex += name;
	SoundIndex += "]";	
	SoundIndex += Int_StrN (numSounds);

	numSounds++;
	return numSounds-1;
}

void CSound::LoadFiles () {
	CSPList list(200);
	string name;
	string soundfile;
	string path;
	string line;
	int soundid;
	if (list.Load (cfg.sound_dir, "sounds.lst")) { 
		for (int i=0; i<list.Count(); i++) {
			line = list.Line(i);
			name = SPStrN (line, "name", "");		
			soundfile = SPStrN (line, "file", "");		
			path = MakePathStr (cfg.sound_dir, soundfile);
			soundid = LoadChunk (name.c_str(), path.c_str());
		}
	}
}



