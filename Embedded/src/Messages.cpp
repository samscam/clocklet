#include "Messages.h"

// ----------- RANDOM MESSAGES

const char *messages[] = {
  "Always be excellent",
  "Good vibes",
  "Everything is going to be fine",
  "You got this",
  "Be kind",
  "Have a lovely day",
  "Things can only get better",
  "One step at a time",
  "Never give up, never surrender!",
  "You are awesome",
  "It's a them problem",
  "We'll fix it all",
  "Rub bellies NOW" 
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char * randoMessage(){
  int messageIndex = random(0,numMessages);
  const char *randoMessage = messages[messageIndex];
  return randoMessage;
}
