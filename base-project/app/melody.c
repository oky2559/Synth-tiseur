#include <stdio.h>
#include <stdlib.h>
#include "../include/synthesizer.h"

int main(void) {
    // Initialiser le synthesizer avec 1 voix
    Synthesizer *synth = synth_new(1, SAMPLE_RATE);
    if (!synth) {
        fprintf(stderr, "Erreur: impossible de créer le synthesizer\n");
        return 1;
    }

    synth_vol(synth, 0.6f);
    
    int total_samples = SAMPLE_RATE * 10;
    AudioBuffer *buf = buf_new(total_samples, SAMPLE_RATE, CHANNELS);
    if (!buf) {
        synth_free(synth);
        return 1;
    }

    float temp[SAMPLE_RATE];
    int idx = 0;

    // Notes de la mélodie "Twinkle Twinkle Little Star"
    float notes[] = {261.63f, 261.63f, 392.00f, 392.00f, 440.00f, 440.00f, 392.00f, 349.23f, 329.63f, 293.66f, 261.63f};
    float durations[] = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f};
    int num_notes = sizeof(notes) / sizeof(notes[0]);

    printf("Génération de la mélodie 'Twinkle Twinkle Little Star'...\n");

    // Jouer chaque note
    for (int i = 0; i < num_notes && idx < total_samples; i++) {
        int samp = (int)(durations[i] * SAMPLE_RATE);
        synth_on(synth, 0, notes[i], WAVE_SINE, 0.4f);
        synth_fill(synth, temp, samp);
        for (int j = 0; j < samp && idx < total_samples; j++) {
            buf->samples[idx++] = to_sample(temp[j]);
        }
        synth_off(synth, 0);

        // Petite pause entre les notes
        int pause = (int)(0.1f * SAMPLE_RATE);
        for (int j = 0; j < pause && idx < total_samples; j++) {
            buf->samples[idx++] = 0;
        }
    }

    buf->length = idx;

    // Sauvegarder le fichier
    if (wav_save("melody_output.wav", buf)) {
        printf("✓ Fichier créé: melody_output.wav\n");
    } else {
        fprintf(stderr, "Erreur: impossible de sauvegarder le fichier\n");
    }

    buf_free(buf);
    synth_free(synth);

    return 0;
}
