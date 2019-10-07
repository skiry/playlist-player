#pragma once
#include <string.h>
#include <sstream>

class Pair {
    int minutes;
    int seconds;
    char name[100];
public:
    Pair();
    Pair(char *);
    int getMinutes();
    int getSeconds();
    char* getName();
};

class Melody {
private:
    char name[100];
    char url[44];
    Pair *duration;
public:
    Melody(const char* url);
    char* getName();
    char* getUrl();
    Pair getDuration();
};

class Playlist {
    int size = 0;
    int capacity = 100;
    int currentMelody;
    char *filePath;
    Melody **melodies;
public:
    Playlist(char *);
    ~Playlist();
    void addMelody(Melody *);
    void clear();
    void play();
    char* getCurrentName();
    Pair getDuration();
    void addMusic();
};