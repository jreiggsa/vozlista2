#pragma once
#include <JuceHeader.h>

// Valores objetivo de cada modo cuando Assist = 100%
struct ModeTarget
{
    float hpf;     // Hz
    float prox;    // dB low shelf 150 Hz (efecto proximidad del mic dinamico)
    float mud;     // dB bell 300 Hz
    float box;     // dB bell 550 Hz
    float pres;    // dB bell 4.5 kHz
    float air;     // dB high shelf 11 kHz
    float thr;     // dB umbral compresor
    float ratio;
    float deess;   // 0..1
    float sat;     // 0..1
};

struct Preset
{
    const char* name;
    int   mode;
    float assist;   // 0..100
    float cuerpo, pres, air;   // dB
    float comp, deess, sat;    // 0..1.5
};

class VozListaAudioProcessor : public juce::AudioProcessor
{
public:
    VozListaAudioProcessor();
    ~VozListaAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Voz Lista"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    static const std::vector<Preset>& getPresets();
    static const juce::StringArray& getModeNames();

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void setParam (const juce::String& id, float plainValue);
    float get (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    using Band = std::array<juce::dsp::IIR::Filter<float>, 2>;
    Band hpf, lowShelf, mud, box, pres, air, sibHP;
    juce::dsp::Compressor<float> compressor;
    juce::AudioBuffer<float> dry;

    double sr = 44100.0;
    float sibEnv = 0.f, fullEnv = 0.f, lastOutGain = 1.f;
    int currentPreset = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VozListaAudioProcessor)
};
