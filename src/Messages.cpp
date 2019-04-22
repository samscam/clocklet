#include "Messages.h"

// ----------- RANDOM MESSAGES

const char* messages[] = {
  "ball boy",
  "bread for everybody",
  "too darn hot",
  "you must obey the clock",
  "commit",
  "woop woop time to rub bellies woop woop",
  "all your base are belong to us",
  "accuracy is guaranteed",
  "do the ham dance",
  "striving to be less shite",
  "just a baby fart",
  "i support thunder again",
  "baby clock doo doo doo du doo du doo",
  "this message deliberately left blank"
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char* randoMessage(){
  int messageIndex = random(0,numMessages-1);
  const char* randoMessage = messages[messageIndex];
  return randoMessage;
}
