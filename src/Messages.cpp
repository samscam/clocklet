#include "Messages.h"

// ----------- RANDOM MESSAGES

const char* messages[] = {
  "Ball Boy",
  "You must obey the clock",
  "Commit Commit Commit",
  "WOOP WOOP time to rub bellies WOOP WOOP",
  "All your base are belong to us",
  "Accuracy is probably ok",
  "Do the ham dance",
  "Just is the enemy of good",
  "Striving to be less shite",
  "Baby fart? It was Mr Nobody",
  "Black is the new rainbow",
  "Baby clock doo doo doo du doo du doo",
  "Take your pills",
  "Cyclists Dismount",
  "Probably enough RAM",
  "bin fairies are real",
  "big eyed beans from venus",
  "moose",
  "rainbow detection is broken",
  "I want to live in the jungle",
  "this message intentionally left blank",
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char* randoMessage(){
  int messageIndex = random(0,numMessages);
  const char* randoMessage = messages[messageIndex];
  return randoMessage;
}
