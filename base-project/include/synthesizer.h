#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <stdint.h>
#include <stddef.h>

// Audio format

#define SAMPLE_RATE 44100
#define BIT_DEPTH 16
#define CHANNELS 1
#define BYTES_PER_SAMPLE 2

typedef int16_t AudioSample;

typedef struct {
    AudioSample *samples;
    size_t length;
    int sample_rate;
    int channels;
} AudioBuffer;

AudioBuffer* buf_new(size_t num_samples, int sample_rate, int channels);
void buf_free(AudioBuffer *buffer);
AudioSample to_sample(float value);

// Waveforms

typedef enum {
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAWTOOTH
} WaveformType;

// Oscillator

typedef struct {
    float frequency;
    float amplitude;
    float phase;
    WaveformType waveform;
    int sample_rate;
} Oscillator;

Oscillator* osc_new(float frequency, float amplitude,
                    WaveformType waveform, int sample_rate);
void osc_free(Oscillator *osc);
float osc_next(Oscillator *osc);
void osc_freq(Oscillator *osc, float frequency);
void osc_amp(Oscillator *osc, float amplitude);

// ADSR envelope

typedef struct {
    float attack_time;
    float decay_time;
    float sustain_level;
    float release_time;
    
    int sample_rate;
    int attack_samples;
    int decay_samples;
    int release_samples;
    
    float current_level;
    int sample_count;
    int state;
} Envelope;

Envelope* env_new(float attack, float decay, float sustain,
                 float release, int sample_rate);
void env_free(Envelope *env);
void env_start(Envelope *env);
void env_stop(Envelope *env);
float env_level(Envelope *env);
int env_done(Envelope *env);

// Synthesizer & voice mixing

typedef struct {
    Oscillator *oscillator;
    Envelope *envelope;
    int active;
} Voice;

typedef struct {
    Voice *voices;
    int num_voices;
    float master_volume;
    int sample_rate;
} Synthesizer;

Synthesizer* synth_new(int num_voices, int sample_rate);
void synth_free(Synthesizer *synth);
void synth_on(Synthesizer *synth, int voice, float freq,
             WaveformType wave, float amp);
void synth_off(Synthesizer *synth, int voice);
void synth_vol(Synthesizer *synth, float vol);
float synth_next(Synthesizer *synth);
void synth_fill(Synthesizer *synth, float *buffer, size_t num_samples);

// WAV export

int wav_save(const char *filename, const AudioBuffer *buffer);

#endif // SYNTHESIZER_H
