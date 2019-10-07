#include <iostream>
#include "Playlist.h"

int main() {
    char *filePath = (char*)malloc(sizeof(char) * 40);
    if (filePath == NULL) {
        printf("Error allocating memory\n");
        return -1;
    }

    printf("Enter the path of the playlist: ");
    gets_s(filePath, 40);
    Playlist* playlist = new Playlist(filePath);
    playlist->addMusic();
    playlist->play();
    //write: pause option.
    free(filePath);
    return 0;
}
