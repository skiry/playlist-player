#include "Playlist.h"
#include <string>
#include <Windows.h>
#include <WinINet.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib, "WinINet.lib")
Pair::Pair(char *url) {
    HINTERNET connect = InternetOpen("MyPlaylist", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    if (!connect) {
        printf("Connection Failed or Syntax error\n");
        exit(EXIT_FAILURE);
    }

    HINTERNET OpenAddress = InternetOpenUrl(connect,
        url, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

    if (!OpenAddress)
    {
        DWORD ErrorNum = GetLastError();
        printf("Failed to open URL \nError No: %d", ErrorNum);
        InternetCloseHandle(connect);
        exit(EXIT_FAILURE);
    }

    char DataReceived[4096];
    DWORD NumberOfBytesRead = 0;
    while (InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead)
    {
        char* name = strstr(DataReceived, "<meta property=\"og:title\" content=\"");
        char* duration = strstr(DataReceived, "<meta itemprop=\"duration\" content=\"PT");
        int padding = 0;

        if (duration && duration[37]) {
            //found the description of the duration
            //the 37th segment has the duration in minutes until the 'M' delimitator
            this->minutes = duration[37] - 48;
            if (duration[38] && duration[38] != 'M') {
                this->minutes = this->minutes * 10 + duration[38] - 48;
                ++padding;
            }
            //now we get the seconds, skipping the letter 'M' and eventually the second digit from the minutes
            if (duration[39 + padding]) {
                this->seconds = duration[39 + padding] - 48;
                if (duration[39 + padding + 1] && duration[39 + padding + 1] != 'S') {
                    this->seconds = this->seconds * 10 + duration[39 + padding + 1] - 48;
                }
            }
        }
        
        padding = 0;
        int relativePosition = 0, absolutePosition = 0;
        while (name && name[absolutePosition] && padding < 134) {
            if (padding > 34) {
                if (name[absolutePosition] == '"') {
                    padding = 134;
                }
                else {
                    if (name) {
                        if (name[absolutePosition] == '&') {
                            if (name[absolutePosition + 1] == 'a') {
                                //&amp;
                                this->name[relativePosition] = name[absolutePosition];
                                absolutePosition += 4;
                            }
                            else if (name[absolutePosition + 1] == 'q') {
                                //&quot;
                                this->name[relativePosition] = '"';
                                absolutePosition += 5;
                            }
                            else {
                                //&#39
                                this->name[relativePosition] = '\'';
                                absolutePosition += 4;
                            }
                        }
                        else if (name[absolutePosition] == -30) {
                            //ÔÇÿ, ÔÇÖ
                            this->name[relativePosition] = '\'';
                            absolutePosition += 2;
                        }
                        else if (name[absolutePosition] == -61) {
                            //é
                            this->name[relativePosition] = 130;
                            ++absolutePosition;
                        }
                        else {
                            this->name[relativePosition] = name[absolutePosition];
                        }
                    }
                    ++relativePosition;
                }
            }
            ++padding;
            ++absolutePosition;
            this->name[relativePosition] = '\0';
        }
    }
    InternetCloseHandle(OpenAddress);
    InternetCloseHandle(connect);
    /*
    A SECOND OPTION. GOOD 2 KNOW.

    std::ofstream fout(L"c:\\test\\_test.htm");
    std::wstring url = L"url.com";
    HINTERNET hopen = InternetOpen(L"MyAppName",
                            INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    if (hopen)
    {
        DWORD flags = INTERNET_FLAG_DONT_CACHE;
        if (url.find(L"https://") == 0)
            flags |= INTERNET_FLAG_SECURE;
        HINTERNET hinternet = InternetOpenUrl(hopen, "url.com", NULL, 0, flags, 0);
        if (hinternet)
        {
            char buf[1024];
            DWORD received = 0;
            while (InternetReadFile(hinternet, buf, sizeof(buf), &received))
            {
                if (!received) break;
                printf("%s\n", buf);
            }
            InternetCloseHandle(hinternet);
        }
        InternetCloseHandle(hopen);
    }
    */
}

int Pair::getMinutes() {
    return this->minutes;
}

int Pair::getSeconds() {
    return this->seconds;
}

char* Pair::getName() {
    return this->name;
}
Melody::Melody(const char* url) {
    strcpy_s(this->url, 44, url);
    this->duration = new Pair(this->url);
    strcpy_s(this->name, strlen(this->duration->getName()) + 1, this->duration->getName());
}

char* Melody::getName() {
    return this->name;
}
char* Melody::getUrl() {
    return this->url;
}

Pair Melody::getDuration() {
    return *this->duration;
}

Playlist::Playlist(char *path) {
    this->melodies = (Melody**)malloc(capacity * sizeof(Melody*));

    if (melodies == NULL) {
        printf("Error allocating memory!\n");
        exit(EXIT_FAILURE);
    }
    filePath = (char*)malloc(sizeof(char) * 40);
    if (filePath == NULL) {
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }

    strcpy_s(this->filePath, strlen(path) + 1, path);
}

void Playlist::addMelody(Melody *M) {
    if (this->size == capacity) {
        capacity *= 2;
        melodies = (Melody**)realloc(melodies, capacity * sizeof(Melody*));

        if (melodies == NULL) {
            printf("Error allocating memory!\n");
            exit(EXIT_FAILURE);
        }
    }

    melodies[this->size++] = M;
}

void Playlist::clear() {
    for (int i = 0; i < this->size; ++i) {
        free(melodies[i]);
    }
    size = 0;
}

void Playlist::play() {
    srand(time(0));
    while (1) {
        currentMelody = rand() % this->size;
        printf("Now playing %s \n", this->getCurrentName());
        ShellExecute(NULL, "open", this->melodies[currentMelody]->getUrl(), NULL, NULL, SW_SHOWNORMAL);
        Pair duration = this->melodies[currentMelody]->getDuration();
        int toSleep = duration.getMinutes() * 60 + duration.getSeconds();
        Sleep(toSleep * 1000);
    }
}

char* Playlist::getCurrentName() {
    return melodies[currentMelody]->getName();
}

Pair Playlist::getDuration() {
    return melodies[currentMelody]->getDuration();
}

Playlist::~Playlist() {
    this->clear();
    free(melodies);
    free(filePath);
    melodies = NULL;
}

void Playlist::addMusic() {
    FILE *openFile;
    char *toRead = (char*)malloc(sizeof(char) * 44);
    if (toRead == NULL) {
        printf("Error allocating memory!\n");
        exit(EXIT_FAILURE);
    }

    fopen_s(&openFile, filePath, "r");
    if (openFile == NULL) {
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    while (fgets(toRead, 44, openFile) != NULL){
        fgetc(openFile);
        this->addMelody(new Melody(toRead));
        //printf("%d %d %s\n", this->melodies[size - 1]->getDuration().getMinutes(), this->melodies[size - 1]->getDuration().getSeconds(),
            //this->melodies[size - 1]->getDuration().getName());
    }

    free(toRead);
    fclose(openFile);
}
