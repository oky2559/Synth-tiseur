#include <stdio.h>
#include <stdlib.h>

#include "synthesizer.h"

int main() {
    Synthesizer *synth = synth_new(2, SAMPLE_RATE);
    if (!synth) {
        fprintf(stderr, "Failed to create synthesizer\n");
        return 1;
    }
    
    synth_vol(synth, 0.6f);
    
    int total_samples = SAMPLE_RATE * 5;
    AudioBuffer *buf = buf_new(total_samples, SAMPLE_RATE, 1);
    if (!buf) {
        synth_free(synth);
        return 1;
    }
    
    float temp[SAMPLE_RATE * 2];
    int idx = 0;
    
    // Scale
    float notes[] = {262.0f, 294.0f, 330.0f, 349.0f, 392.0f, 440.0f, 494.0f, 523.0f};
    int n = sizeof(notes) / sizeof(notes[0]);
    
    for (int i = 0; i < n && idx < total_samples; i++) {
        int samp = (int)(0.3f * SAMPLE_RATE);
        synth_on(synth, 0, notes[i], WAVE_SINE, 0.4f);
        synth_fill(synth, temp, samp);
        for (int j = 0; j < samp && idx < total_samples; j++) {
            buf->samples[idx++] = to_sample(temp[j]);
        }
    }
    
    // Silence
    int sil = (int)(0.3f * SAMPLE_RATE);
    for (int i = 0; i < sil && idx < total_samples; i++) {
        buf->samples[idx++] = 0;
    }
    
    // Chords
    float chords[][2] = {{65.4f, 130.8f}, {82.4f, 164.8f}, {98.0f, 196.0f}};
    for (int i = 0; i < 3 && idx < total_samples; i++) {
        int samp = (int)(0.5f * SAMPLE_RATE);
        synth_on(synth, 0, chords[i][0], WAVE_SQUARE, 0.35f);
        synth_on(synth, 1, chords[i][1], WAVE_SQUARE, 0.35f);
        synth_fill(synth, temp, samp);
        for (int j = 0; j < samp && idx < total_samples; j++) {
            buf->samples[idx++] = to_sample(temp[j]);
        }
        synth_off(synth, 0);
        synth_off(synth, 1);
    }
    
    buf->length = idx;
    
    if (wav_save("example_output.wav", buf)) {
        printf("Done! Generated %d samples (%.1f sec)\n", idx, (float)idx / SAMPLE_RATE);
    } else {
        fprintf(stderr, "Error saving WAV\n");
    }
    
    buf_free(buf);
    synth_free(synth);
    return 0;
}
