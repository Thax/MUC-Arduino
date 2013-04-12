#include <TimerOne.h>
#include "multitone.h"
#include <Arduino.h>

int master_freq = 8000;   // set master clock frequency to 2 kHz
int actual_num_tones = 0; // must be initialized to prevent accidental
                          // calling in case init is not called first

int count_to[MT_MAX_TONES]; // value to count to to achieve desired frequency
int counters[MT_MAX_TONES]; // counters to keep track of how long the wave
                            // has been in a desired state

int tone_pins[MT_MAX_TONES];
int tone_state;

int tone_on = 0;

void toggle(int index);
int  getTicksFromFreq(int freq);
void force_off(int index);

void tone_interrupt_callback() {
   int i;
   for (i = 0; i < actual_num_tones; i++) {
      if (counters[i] >= count_to[i]) {
         counters[i] = 0;
         toggle(i);
      } else if (counters[i] >= 0) {
         // if counter is turned on (-1 means off)
         counters[i]++;
      }
   }
}

void init_master_clock() {
   Timer1.initialize(1000000/master_freq); // initialize with period for
                                     // master_freq

   Timer1.attachInterrupt(tone_interrupt_callback);
   Timer1.start();
}

void change_frequency(int index, int desired_freq) {
   // changes frequency of already initialized sound
   // this function will turn off sound for requested
   // values that are out of range
   // max desired frequency is 1/2 clock frequency;
   // @param index        - index of frequency to change
   // @param desired_freq - new desired frequency
   if ((index >= 0) && (index < actual_num_tones)) {
      // sanity check. if it's valid, we need to reset counter either way 
      counters[index] = 0;
      
      if ((desired_freq < 60) || (desired_freq > (master_freq>>1))) {
         // if desired frequency is out of range, stop counting and make
         // sure it's not just sitting at HIGH
         count_to[index] = 0;
         force_off(index);
      } else {
         // if it's a valid frequency, then set the count to value appropriately
         count_to[index] = getTicksFromFreq(desired_freq);
      }
   }
   /*
   Serial.print("Index: ");
   Serial.print(index);
   Serial.print("; count_to: ");
   Serial.print(count_to[index]);
   Serial.print("; Desired freq: ");
   Serial.print(desired_freq);
   Serial.print(";\n");
   */
}



void toggle(int index) {
   if ((actual_num_tones > 0) && (index < actual_num_tones) && (index >= 0)) {
   // sanity check
      int flag = 1<<index;

      // if the tone is on, toggle it, otherwise just force it off.
      if (tone_on & flag) {
        

         // if the state is on, turn it off, else turn it back on. update state at end.
         if (tone_state & flag) {
            Serial.println("here");
            digitalWrite(tone_pins[index], LOW);
            tone_state = tone_state & ~flag;
         } else {
            digitalWrite(tone_pins[index], HIGH);
            tone_state = tone_state & flag;
         }
      } else {
         digitalWrite(tone_pins[index], LOW);
         tone_state = tone_state & ~flag;
      }
   }
}

void force_off(int index) {
   if ((actual_num_tones > 0) && (index < actual_num_tones) && (index >= 0)) {
      // sanity check
      digitalWrite(tone_pins[index], LOW);
   }
}

void turn_on(int index) {
   if ((actual_num_tones > 0) && (index < actual_num_tones) && (index >= 0)) {
      // sanity check
      tone_on = tone_on | (1<<index);
   }
}
void turn_off(int index) {
   if ((actual_num_tones > 0) && (index < actual_num_tones) && (index >= 0)) {
      // sanity check
      tone_on = tone_on | (~(1<<index));
   }
}

int getTicksFromFreq(int desired_freq) {
   // calculates the number of clock ticks
   // by the master clock for multitone for
   // one period of the waveform with
   // frequency = desired_freq

   // @param desired_freq - frequency of waveform
   //                       to be synthesized
   // @return             - number of ticks to count
   //                       for full period of desired
   //                       waveform
   int tmp = master_freq;
   int i   = 0;
   while (tmp > 0) {
      tmp = tmp - desired_freq;
      i++; 
   }
   return i;
}

void init_multitone(int num_tones, int *freqs, int *pins) {
   // initialization for multiple tone playing
   // sets internal variables
   //
   // Only initializes first MT_MAX_TONES tones,
   // and simply ignores any after that
   //
   // @param num_tones    - number of tones desired
   //                       for application
   // @param freqs        - pointer to list of length
   //                       num_tones of desired frequencies
   //                       to start with. can be 0 if no
   //                       sound is desired at initialization
   int i;

   // make sure we don't cycle past max number of tones
   if (num_tones > MT_MAX_TONES) {
      actual_num_tones = MT_MAX_TONES;
   } else {
      actual_num_tones = num_tones;
   }

   // initialize counters
   for (i = 0; i < actual_num_tones; i++) {
      counters[i] = 0;
      tone_pins[i] = pins[i];
      if (freqs[i] > 0) {
         count_to[i] = getTicksFromFreq(freqs[i]);
         turn_on(i);
      } else {
         count_to[i] = 0;
         turn_off(i);
      }
   }
   
   init_master_clock();
}
