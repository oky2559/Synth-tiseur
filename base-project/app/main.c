#include <stdio.h>
#include <stdlib.h>

#include "synthesizer.h"

int main(int argc, char **argv) {
  Synthesizer *synth = synth_new(1, SAMPLE_RATE);
  if (!synth) {
    fprintf(stderr, "Failed to create synthesizer\n");
    return 1;
  }
  
  printf("Synthesizer Audio Generator\n");
  printf("Sample Rate: %d Hz\n", SAMPLE_RATE);
  printf("Bit Depth: %d bits\n", BIT_DEPTH);
  printf("Channels: %d\n\n", CHANNELS);
  
  float notes[] = {
    262.0f, 294.0f, 330.0f, 349.0f, 392.0f, 440.0f, 494.0f, 523.0f
  };
  
  int num_notes = sizeof(notes) / sizeof(notes[0]);
  int total_samples = 0;
  
  AudioBuffer *full_buffer = buf_new(
    (int)(SAMPLE_RATE * 0.5f * num_notes), SAMPLE_RATE, CHANNELS);
  
  if (!full_buffer) {
    fprintf(stderr, "Failed to create audio buffer\n");
    synth_free(synth);
    return 1;
  }
  
  float *temp = malloc(SAMPLE_RATE * sizeof(float));
  
  for (int i = 0; i < num_notes; i++) {
    printf("Generating note %d: %.1f Hz...\n", i + 1, notes[i]);
    
    int num_samples = (int)(0.5f * SAMPLE_RATE);
    
    synth_on(synth, 0, notes[i], WAVE_SINE, 0.3f);
    synth_fill(synth, temp, num_samples);
    
    for (int j = 0; j < num_samples; j++) {
      if (total_samples < full_buffer->length) {
        full_buffer->samples[total_samples++] = to_sample(temp[j]);
      }
    }
  }
  
  free(temp);
  
  full_buffer->length = total_samples;
  
  /* Write to WAV file */
  const char *output_file = "synthesizer_output.wav";
  if (wav_save(output_file, full_buffer)) {
    printf("\nAudio saved to: %s\n", output_file);
  } else {
    fprintf(stderr, "Failed to write WAV file\n");
  }
  
  buf_free(full_buffer);
  synth_free(synth);
  
  return 0;
}
