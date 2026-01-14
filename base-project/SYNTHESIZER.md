# Synthétiseur Audio

Génère des sons numériques.

## Composants

- **Oscillateur** : génère l'onde (sine, square, triangle, sawtooth)
- **Enveloppe ADSR** : contrôle le volume (attack, decay, sustain, release)
- **Synthétiseur** : mélange plusieurs voix
- **Buffer WAV** : exporte en fichier

## API

### Oscillateur

```c
// Créer un oscillateur 440 Hz (La)
Oscillator *osc = osc_new(440.0f,    // fréquence
                          0.8f,      // amplitude
                          WAVE_SINE, // forme
                          44100);    // sample rate

// Générer un échantillon à la fois
float sample = osc_next(osc);

// Changer la fréquence en direct
osc_freq(osc, 880.0f);  // Maintenant La 880 Hz

// Libérer la mémoire
osc_free(osc);
### Enveloppe ADSR

```c
Envelope *env = env_new(
    0.01f,   // Attack:  10 ms
    0.1f,    // Decay:   100 ms
    0.7f,    // Sustain: 70% du volume
    0.2f,    // Release: 200 ms
    44100
);

// Déclencher la note (commence attack)
env_start(env);
float level = env_level(env);

// Relâcher la note (commence release)
env_stop(env);

// Vérifier si fini
if (env_done(env)) {
    printf("Son fini!\n");
}

env_free(env);
```

### Synthétiseur

```c
// Créer un synthétiseur avec 4 voix max
Synthesizer *synth = synth_new(4, 44100);

// Jouer une note sur la voix 0
synth_on(synth, 0,      // voix
         440.0f,        // fréquence (Hz)
         WAVE_SINE,     // forme
         0.8f);         // amplitude

// Générer 1 échantillon audio (mélange de toutes les voix)
float sample = synth_next(synth);

// Arrêter la note
synth_off(synth, 0);

synth_free(synth);
```

### Exporter en WAV

```c
AudioBuffer *buffer = ...;  // Votre audio

// Sauvegarder dans un fichier
int success = wav_save("mon_son.wav", buffer);

if (success) {
    printf("Fichier sauvegardé!\n");
}

// Libérer la mémoire
buf_free(buffer);
```

## Exemple Complet : Générer une Note

```c
#include "synthesizer.h"

int main() {
    // Créer le synthé
    Synthesizer *synth = synth_new(1, SAMPLE_RATE);
    
    // Générer 1 seconde de La (440 Hz) en sine
    int samp = SAMPLE_RATE;
    float temp[samp];
    synth_on(synth, 0, 440.0f, WAVE_SINE, 0.8f);
    synth_fill(synth, temp, samp);
    
    AudioBuffer *buf = buf_new(samp, SAMPLE_RATE, 1);
    for (int i = 0; i < samp; i++) {
        buf->samples[i] = to_sample(temp[i]);
    }
    
    // Sauvegarder
    wav_save("la.wav", buf);
    
    // Nettoyer
    buf_free(buf);
    synth_free(synth);
    
    printf("Fichier généré!\n");
    return 0;
}
```

## Exemple : Jouer Plusieurs Notes

```c
float notes[] = {
    261.63f,  // Do (C4)
    293.66f,  // Ré (D4)
    329.63f,  // Mi (E4)
    349.23f,  // Fa (F4)
    392.00f,  // Sol (G4)
    440.00f   // La (A4)
};

Synthesizer *synth = synth_new(1, SAMPLE_RATE);
float temp[SAMPLE_RATE / 2];

// Générer chaque note
for (int i = 0; i < 6; i++) {
    int samp = (int)(0.5f * SAMPLE_RATE);
    synth_on(synth, 0, notes[i], WAVE_SINE, 0.5f);
    synth_fill(synth, temp, samp);
    // Ajouter à un buffer global...
}

synth_free(synth);
```

## Exemple : Polyphonie (Plusieurs Notes Simultanées)

```c
Synthesizer *synth = synth_new(3, SAMPLE_RATE);  // 3 voix
float temp[SAMPLE_RATE];

// Jouer un accord Do-Mi-Sol
synth_on(synth, 0, 261.63f, WAVE_SINE, 0.5f);  // Do
synth_on(synth, 1, 329.63f, WAVE_SINE, 0.5f);  // Mi
synth_on(synth, 2, 392.00f, WAVE_SINE, 0.5f);  // Sol

// Générer 1 seconde
synth_fill(synth, temp, SAMPLE_RATE);

synth_free(synth);
```


## Tableau Récapitulatif

| Fonction | Utilité |
|----------|---------|
| `osc_new()` | Créer un générateur d'onde |
| `osc_next()` | Générer 1 échantillon |
| `env_new()` | Créer une enveloppe ADSR |
| `env_start()` | Commencer la note |
| `env_stop()` | Terminer la note |
| `synth_new()` | Créer un synthé multi-voix |
| `synth_on()` | Jouer une note |
| `synth_off()` | Arrêter une note |
| `wav_save()` | Exporter en WAV |

