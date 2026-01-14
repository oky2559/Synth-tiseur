# Synthétiseur Audio Numérique

Un synthétiseur simple en C qui génère des sons à partir de paramètres de base (fréquence, amplitude, durée).

## Ce qu'il fait

- **4 formes d'onde** : sine (lisse), square (rêche), triangle (doux), sawtooth (brillant)
- **Enveloppe ADSR** : contrôle du volume (attack, decay, sustain, release)
- **Polyphonie** : plusieurs notes simultanées
- **Export WAV** : fichiers audio standard lisibles partout

## Installation rapide

### Sur Windows avec MSYS2 (recommandé)

1. Installer MSYS2 : https://www.msys2.org/
2. Ouvrir terminal "MSYS2 UCRT64" et exécuter :
```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-meson mingw-w64-ucrt-x86_64-ninja
```

### Sur Linux (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install -y build-essential meson ninja-build
```

### Sur macOS

```bash
brew install meson ninja
xcode-select --install
```

## Utilisation

```bash
cd base-project
meson setup build          # Configurer
meson compile -C build     # Compiler
meson test -C build        # Tester
```

## Exécuter les exemples

```bash
cd build/app

# Exemple 1 : Gamme simple (Do Ré Mi Fa Sol La Si Do)
./base_project_app.exe

# Exemple 2 : Gamme + accords + sweep
./synthesizer_example.exe
```

Fichiers générés : `synthesizer_output.wav`, `example_output.wav`

Écoutez-les avec : VLC, Audacity, Windows Media Player, etc.


## Structure du code

```
include/
├── synthesizer.h    → Tout l'API (audio, oscillateur, enveloppe, synthé, WAV)
└── custom_math.h    → Fonctions mathématiques utilitaires

src/
├── synthesizer.c    → Toute l'implémentation (Un seul fichier!)
└── custom_math.c

app/
├── main.c           → Exemple simple (gamme)
└── example.c        → Exemple avancé (gamme + accords + sweep)
```

### Créer un oscillateur

```c
Oscillator *osc = osc_new(440.0f, 0.5f, WAVE_SINE, 44100);
float sample = osc_next(osc);
osc_free(osc);
```

### Créer un synthétiseur

```c
Synthesizer *synth = synth_new(4, 44100);  // 4 voix max

synth_on(synth, 0, 440.0f, WAVE_SINE, 0.8f);
float sample = synth_next(synth);

synth_off(synth, 0);
synth_free(synth);
```

### Enveloppe ADSR

```c
Envelope *env = env_new(
    0.01f,   // Attack  (10ms)
    0.1f,    // Decay   (100ms)
    0.7f,    // Sustain (70% du volume)
    0.2f,    // Release (200ms)
    44100
);

env_start(env);           // Commencer
float level = env_level(env);

env_stop(env);            // Relâcher et arrêter
```

---