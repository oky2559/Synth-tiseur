#include <stdio.h>
#include <stdlib.h>
#include "../include/synthesizer.h"

int main(void) {

    float frequency;
    float duration;
    float amplitude;
    float volume;
    int wave_choice;
    WaveformType wave;

    /* ====== SAISIE UTILISATEUR ====== */
    printf("Frequence (Hz) : ");
    scanf("%f", &frequency);

    printf("Duree (secondes) : ");
    scanf("%f", &duration);

    printf("Amplitude (0.0 a 1.0) : ");
    scanf("%f", &amplitude);

    printf("Volume global (0.0 a 1.0) : ");
    scanf("%f", &volume);

    printf("Type d'onde :\n");
    printf("  1 - Sinus\n");
    printf("  2 - Carre\n");
    printf("  3 - Triangle\n");
    printf("  4 - Dent de scie\n");
    printf("Choix : ");
    scanf("%d", &wave_choice);

    switch (wave_choice) {
        case 1: wave = WAVE_SINE;     break;
        case 2: wave = WAVE_SQUARE;   break;
        case 3: wave = WAVE_TRIANGLE; break;
        case 4: wave = WAVE_SAWTOOTH; break;
        default:
            printf("Choix invalide, sinus par defaut\n");
            wave = WAVE_SINE;
    }

    /* ====== INITIALISATION SYNTH ====== */
    Synthesizer *synth = synth_new(1, SAMPLE_RATE);
    if (!synth) {
        fprintf(stderr, "Erreur: impossible de creer le synthesizer\n");
        return 1;
    }

    synth_vol(synth, volume);

    int total_samples = (int)(duration * SAMPLE_RATE);
    AudioBuffer *buf = buf_new(total_samples, SAMPLE_RATE, CHANNELS);
    if (!buf) {
        synth_free(synth);
        return 1;
    }

    float *temp = malloc(sizeof(float) * total_samples);
    if (!temp) {
        fprintf(stderr, "Erreur allocation buffer temporaire\n");
        buf_free(buf);
        synth_free(synth);
        return 1;
    }

    printf("Generation du son...\n");

    /* ====== GENERATION ====== */
    synth_on(synth, 0, frequency, wave, amplitude);
    synth_fill(synth, temp, total_samples);
    synth_off(synth, 0);

    /* ====== ENVELOPPE SIMPLE ====== */
    int attack = total_samples / 10;   // 10 %
    int release = total_samples / 10;  // 10 %

    for (int i = 0; i < total_samples; i++) {
        float env;

        if (i < attack)
            env = (float)i / attack;
        else if (i > total_samples - release)
            env = (float)(total_samples - i) / release;
        else
            env = 1.0f;

        buf->samples[i] = to_sample(temp[i] * env);
    }

    buf->length = total_samples;

    /* ====== SAUVEGARDE WAV ====== */
    if (wav_save("sound_output.wav", buf)) {
        printf("✓ Fichier cree : sound_output.wav\n");
    } else {
        fprintf(stderr, "Erreur: impossible de sauvegarder le fichier\n");
    }

    free(temp);
    buf_free(buf);
    synth_free(synth);

    return 0;
}
