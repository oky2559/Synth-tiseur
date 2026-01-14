#include "synthesizer.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define PI 3.14159265358979323846

// Audio buffer management

AudioBuffer* buf_new(size_t num_samples, int sample_rate, int channels)
{
    AudioBuffer *buffer = malloc(sizeof(AudioBuffer));
    if (!buffer) return NULL;
    
    buffer->samples = malloc(num_samples * sizeof(AudioSample));
    if (!buffer->samples) {
        free(buffer);
        return NULL;
    }
    
    buffer->length = num_samples;
    buffer->sample_rate = sample_rate;
    buffer->channels = channels;
    
    return buffer;
}

void buf_free(AudioBuffer *buffer)
{
    if (buffer) {
        free(buffer->samples);
        free(buffer);
    }
}

AudioSample to_sample(float value)
{
    if (value > 1.0f) value = 1.0f;
    if (value < -1.0f) value = -1.0f;
    return (AudioSample)(value * 32767.0f);
}

// Waveform generators

static float sine_wave(float phase)
{
    return sinf(2.0f * PI * phase);
}

static float square_wave(float phase)
{
    return (phase < 0.5f) ? 1.0f : -1.0f;
}

static float triangle_wave(float phase)
{
    if (phase < 0.25f) {
        return 4.0f * phase;
    } else if (phase < 0.75f) {
        return 2.0f - 4.0f * phase;
    } else {
        return 4.0f * phase - 4.0f;
    }
}

static float sawtooth_wave(float phase)
{
    return 2.0f * phase - 1.0f;
}

Oscillator* osc_new(float frequency, float amplitude,
                    WaveformType waveform, int sample_rate)
{
    Oscillator *osc = malloc(sizeof(Oscillator));
    if (!osc) return NULL;
    
    osc->frequency = frequency;
    osc->amplitude = amplitude;
    osc->phase = 0.0f;
    osc->waveform = waveform;
    osc->sample_rate = sample_rate;
    
    return osc;
}

void osc_free(Oscillator *osc)
{
    free(osc);
}

float osc_next(Oscillator *osc)
{
    float sample = 0.0f;
    
    switch (osc->waveform) {
        case WAVE_SINE:
            sample = sine_wave(osc->phase);
            break;
        case WAVE_SQUARE:
            sample = square_wave(osc->phase);
            break;
        case WAVE_TRIANGLE:
            sample = triangle_wave(osc->phase);
            break;
        case WAVE_SAWTOOTH:
            sample = sawtooth_wave(osc->phase);
            break;
    }
    
    sample *= osc->amplitude;
    osc->phase += osc->frequency / osc->sample_rate;
    
    if (osc->phase >= 1.0f) {
        osc->phase -= 1.0f;
    }
    
    return sample;
}

void osc_freq(Oscillator *osc, float frequency)
{
    osc->frequency = frequency;
}

void osc_amp(Oscillator *osc, float amplitude)
{
    osc->amplitude = amplitude;
}

// ADSR envelope

Envelope* env_new(float attack, float decay, float sustain,
                 float release, int sample_rate)
{
    Envelope *env = malloc(sizeof(Envelope));
    if (!env) return NULL;
    
    env->attack_time = attack;
    env->decay_time = decay;
    env->sustain_level = sustain;
    env->release_time = release;
    
    env->sample_rate = sample_rate;
    env->attack_samples = (int)(attack * sample_rate);
    env->decay_samples = (int)(decay * sample_rate);
    env->release_samples = (int)(release * sample_rate);
    
    env->current_level = 0.0f;
    env->sample_count = 0;
    env->state = 0;
    
    return env;
}

void env_free(Envelope *env)
{
    free(env);
}

void env_start(Envelope *env)
{
    env->current_level = 0.0f;
    env->sample_count = 0;
    env->state = 0;
}

void env_stop(Envelope *env)
{
    if (env->state != 3 && env->state != 4) {
        env->state = 3;
        env->sample_count = 0;
    }
}

float env_level(Envelope *env)
{
    if (env->state == 4) {
        return 0.0f;
    }
    
    if (env->state == 0) {  // attack
        if (env->sample_count < env->attack_samples) {
            env->current_level = (float)env->sample_count / env->attack_samples;
            env->sample_count++;
        } else {
            env->current_level = 1.0f;
            env->sample_count = 0;
            env->state = 1;
        }
    }
    
    if (env->state == 1) {  // decay
        if (env->sample_count < env->decay_samples) {
            float progress = (float)env->sample_count / env->decay_samples;
            env->current_level = 1.0f - progress * (1.0f - env->sustain_level);
            env->sample_count++;
        } else {
            env->current_level = env->sustain_level;
            env->state = 2;
        }
    }
    
    if (env->state == 2) {  // sustain
        env->current_level = env->sustain_level;
    }
    
    if (env->state == 3) {  // release
        if (env->sample_count < env->release_samples) {
            float progress = (float)env->sample_count / env->release_samples;
            env->current_level = env->sustain_level * (1.0f - progress);
            env->sample_count++;
        } else {
            env->current_level = 0.0f;
            env->state = 4;
        }
    }
    
    return env->current_level;
}

int env_done(Envelope *env)
{
    return env->state == 4;
}

// Synthesizer voice mixing

Synthesizer* synth_new(int num_voices, int sample_rate)
{
    Synthesizer *synth = malloc(sizeof(Synthesizer));
    if (!synth) return NULL;
    
    synth->voices = malloc(num_voices * sizeof(Voice));
    if (!synth->voices) {
        free(synth);
        return NULL;
    }
    
    for (int i = 0; i < num_voices; i++) {
        synth->voices[i].oscillator = NULL;
        synth->voices[i].envelope = NULL;
        synth->voices[i].active = 0;
    }
    
    synth->num_voices = num_voices;
    synth->master_volume = 1.0f;
    synth->sample_rate = sample_rate;
    
    return synth;
}

void synth_free(Synthesizer *synth)
{
    if (synth) {
        for (int i = 0; i < synth->num_voices; i++) {
            if (synth->voices[i].oscillator) {
                osc_free(synth->voices[i].oscillator);
            }
            if (synth->voices[i].envelope) {
                env_free(synth->voices[i].envelope);
            }
        }
        free(synth->voices);
        free(synth);
    }
}

void synth_on(Synthesizer *synth, int voice, float freq,
             WaveformType wave, float amp)
{
    if (voice < 0 || voice >= synth->num_voices) return;
    
    Voice *v = &synth->voices[voice];
    

    if (v->oscillator) osc_free(v->oscillator);
    if (v->envelope) env_free(v->envelope);
    
    v->oscillator = osc_new(freq, amp, wave, synth->sample_rate);
    v->envelope = env_new(0.01f, 0.1f, 0.7f, 0.2f, synth->sample_rate);
    
    if (v->oscillator && v->envelope) {
        env_start(v->envelope);
        v->active = 1;
    }
}

void synth_off(Synthesizer *synth, int voice)
{
    if (voice < 0 || voice >= synth->num_voices) return;
    
    Voice *v = &synth->voices[voice];
    if (v->envelope) {
        env_stop(v->envelope);
    }
}

void synth_vol(Synthesizer *synth, float vol)
{
    synth->master_volume = vol;
}

float synth_next(Synthesizer *synth)
{
    float sample = 0.0f;
    
    for (int i = 0; i < synth->num_voices; i++) {
        Voice *voice = &synth->voices[i];
        
        if (voice->active && voice->oscillator && voice->envelope) {
            float osc_sample = osc_next(voice->oscillator);
            float env_l = env_level(voice->envelope);
            
            sample += osc_sample * env_l;
            
            if (env_done(voice->envelope)) {
                voice->active = 0;
            }
        }
    }
    
    sample *= synth->master_volume;
    
    if (sample > 1.0f) sample = 1.0f;
    if (sample < -1.0f) sample = -1.0f;
    
    return sample;
}

void synth_fill(Synthesizer *synth, float *buffer, size_t num_samples)
{
    for (size_t i = 0; i < num_samples; i++) {
        buffer[i] = synth_next(synth);
    }
}

// WAV file export

#pragma pack(push, 1)
typedef struct {
    char riff[4];
    uint32_t file_size;
    char wave[4];
} WAVHeader;

typedef struct {
    char fmt[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} WAVFmtChunk;

typedef struct {
    char data[4];
    uint32_t data_size;
} WAVDataChunk;
#pragma pack(pop)

int wav_save(const char *filename, const AudioBuffer *buffer)
{
    FILE *file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }
    
    uint32_t data_size = buffer->length * buffer->channels * BYTES_PER_SAMPLE;
    uint32_t byte_rate = buffer->sample_rate * buffer->channels * BYTES_PER_SAMPLE;
    uint16_t block_align = buffer->channels * BYTES_PER_SAMPLE;
    
    WAVHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.file_size = 36 + data_size;
    memcpy(header.wave, "WAVE", 4);
    fwrite(&header, sizeof(WAVHeader), 1, file);
    
    WAVFmtChunk fmt;
    memcpy(fmt.fmt, "fmt ", 4);
    fmt.fmt_size = 16;
    fmt.audio_format = 1;
    fmt.num_channels = buffer->channels;
    fmt.sample_rate = buffer->sample_rate;
    fmt.byte_rate = byte_rate;
    fmt.block_align = block_align;
    fmt.bits_per_sample = BIT_DEPTH;
    fwrite(&fmt, sizeof(WAVFmtChunk), 1, file);
    
    WAVDataChunk data_chunk;
    memcpy(data_chunk.data, "data", 4);
    data_chunk.data_size = data_size;
    fwrite(&data_chunk, sizeof(WAVDataChunk), 1, file);
    
    fwrite(buffer->samples, BYTES_PER_SAMPLE, buffer->length, file);
    
    fclose(file);
    return 1;
}
