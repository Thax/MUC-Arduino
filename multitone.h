#define MT_MAX_TONES 7 // DO NOT MAKE THIS BIGGER THAN THE WORD SIZE OF THE MACHINE

void init_multitone(int num_tones, int *freqs, int *pins); 
                                                // num_tones cannot exceed 
                                                // MT_MAX_TONES, multitone
                                                // will ignore past that

void change_frequency(int index, int desired_freq); // changes frequency of
                                                    // tone number index
                                                    // for out of range freqs
                                                    // it will turn sound off for
                                                    // that tone

void turn_off(int index); // turns off sound but retains frequency
void turn_on(int index);  // turns back on with previously stored frequency

