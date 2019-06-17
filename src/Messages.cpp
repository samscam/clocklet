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
  "Striving to be less shite",
  "Baby fart? It was Mr Nobody",
  "Black is the new rainbow",
  "Baby clock doo doo doo du doo du doo",
  "Sarah: Take your pills",
  "Cyclists Dismount",
  "Probably enough RAM",
  "I want to live in the jungle",
  "Intentionally left blank",
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char* randoMessage(){
  int messageIndex = random(0,numMessages-1);
  const char* randoMessage = messages[messageIndex];
  return randoMessage;
}
