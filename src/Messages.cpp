#import "Messages.h"

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
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

void randoMessage(){
  int messageIndex = random(0,numMessages-1);
  const char* randoMessage = messages[messageIndex];

  // Do it three times
  for (int i=0;i<3;i++){
    scrollText(randoMessage);
  }
}
