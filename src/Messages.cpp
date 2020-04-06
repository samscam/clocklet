#include "Messages.h"

// ----------- RANDOM MESSAGES

const char* messages[] = {
  "Ball Boy",
  "I am your number one fan",
  "this is not a shoe",
  "obedience is mandatory",
  "boss clock",
  "decimalised time should be a thing",
  "If you spill tea on me I will melt",
  "Commit Commit Commit",
  "WOOP WOOP time to rub bellies WOOP WOOP",
  "All your base are belong to us",
  "Accuracy will improve",
  "Do the ham dance",
  "Just is the enemy of good",
  "Striving to be less shite",
  "Baby fart? It was Mr Nobody",
  "rainbow is the new green",
  "Cyclists Dismount",
  "bin fairies are real",
  "big eyed beans from venus",
  "moose",
  "rainbow detection is now active active",
  "I am socially distant",
  "babies are nice",
  "Say it in your COOL voice",
  "Make it so",
  "everything is going to be fine"
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char* randoMessage(){
  int messageIndex = random(0,numMessages);
  const char* randoMessage = messages[messageIndex];
  return randoMessage;
}
