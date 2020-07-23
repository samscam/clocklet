#include "Messages.h"

// ----------- RANDOM MESSAGES

const char *messages[] = {
  "Ball Boy",
  "I am your number one fan",
  "This is not a shoe",
  "Obedience is mandatory",
  "Boss clock",
  "Decimalised time should be a thing",
  "If you spill tea on me I will melt",
  "Commit Commit Commit",
  "WOOP WOOP time to rub bellies WOOP WOOP",
  "All your base are belong to us",
  "Accuracy is sub-millisecond! Hooray!",
  "Do the ham dance",
  "Just is the enemy of good",
  "Striving to be less shite",
  "Baby fart? It was Mr Nobody!",
  "Rainbow is the new green",
  "Cyclists Dismount",
  "Bin fairies are real",
  "Big eyed beans from Venus",
  "Moose",
  "Rainbow detection is active",
  "I am socially distant",
  "Babies are nice",
  "Say it in your COOL voice",
  "Make it so",
  "Everything is going to be fine",
  "Be a LERT!",
  "Clocklet wants a bagel",
  "Clocklet wants a fucking bagel!",
  "Not long now",
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char * randoMessage(){
  int messageIndex = random(0,numMessages);
  const char *randoMessage = messages[messageIndex];
  return randoMessage;
}
