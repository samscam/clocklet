#include "Messages.h"

// ----------- RANDOM MESSAGES

const char *messages[] = {
  "CHRISTMAS MODE ACTIVE",
  "So here it is, Merry Christmas",
  "Silent night",
  "Ring out those bells tonight",
  "Santa Claus is coming to town",
  "Bah humbug",
  "Where the snow lay round about deep and crisp and even",
  "Brightly shone the moon that night",
  "little donkey",
  "I'm going to sort out who's naughty and nice",
  "Jingle bells",
  "Twelve drummers drumming",
  "Eleven pipers piping",
  "Ten lords a leaping",
  "Nine ladies dancing",
  "Eight maids a milking",
  "Seven swans a swimming",
  "Six geese a laying",
  "FIVE GOLD RINGS",
  "Four calling birds",
  "Three french hens",
  "Two turtle doves",
  "And a partridge in a pear tree",
  "Ding dong merrily on high",
  "I saw three ships",
  "The holly and the ivy",
  "Have yourself a merry little christmas",
  "As long as you've got your ELF"
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

const char * randoMessage(){
  int messageIndex = random(0,numMessages);
  const char *randoMessage = messages[messageIndex];
  return randoMessage;
}
