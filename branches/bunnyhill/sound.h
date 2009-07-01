// --------------------------------------------------------------------
// 
// Extreme Tuxracer - Bunny Hill
//
// Copyright (C) 2009 ETR Team <http://www.extremetuxracer.com>
//
// --------------------------------------------------------------------

// The sound unit is completely rewritten since the original audio code
// of Tuxracer, PPRacer and ETRacer was extremely bloated and unclear.

#ifndef SOUND_H
#define SOUND_H

#include "bh.h"

// --------------------------------------------------------------------
//				class CAudio
// --------------------------------------------------------------------

class CAudio {
private:
public:
	CAudio ();
	~CAudio ();
	void Init ();
	void Shutdown ();
	bool IsOpen ();
	void Load ();
	void Update ();
};

// --------------------------------------------------------------------
//				class CMusic
// --------------------------------------------------------------------

#define MAX_MUSICS 32

typedef struct {
    string name;
    int loop;
    Mix_Music *music;  
    bool playing;   
} TMusic;

class CMusic {
private:
	TMusic *musics[MAX_MUSICS];
	int numMusics;
	string MusicIndex;
	int curr_idx;
	string curr_name;

	void SetStatus (string name, bool playing);
	void SetStatus2 (int idx, bool playing);
	int LoadPiece (const char *name, const char *filename, int loop); 
public:
	CMusic (); 
	~CMusic () {}
	void LoadFiles ();
	bool Play (string name);
	void Update ();
};

// --------------------------------------------------------------------
//				class CSound
// --------------------------------------------------------------------

#define MAX_SOUNDS 64

typedef struct {
    int loop_count;
    int channel;
    int volume;
    Mix_Chunk *chunk;  
} TSound;

class CSound {
private:
	TSound *sounds[MAX_SOUNDS];
	int numSounds;
	string SoundIndex;

	int LoadChunk (const char *name, const char *filename);
public:
	CSound (); 
	~CSound () {}
	void LoadFiles ();
	void Play (int soundid, int loop);
	void Play (int soundid, int loop, int volume);
	void Play (string name, int loop); // -1 infinite, 0 once ...
	void Play (string name, int loop, int volume); 
	void Halt (int soundid);
	void Halt (string name);
	void SetVolume (int soundid, int volume);
	void SetVolume (string name, int volume);
	void Update ();
};


extern CAudio Audio;
extern CMusic Music;
extern CSound Sound;

#endif
