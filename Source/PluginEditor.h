#pragma once
#include "PluginProcessor.h"

class VozListaEditor : public juce::AudioProcessorEditor
{
public:
    explicit VozListaEditor (VozListaAudioProcessor&);
    ~VozListaEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
    };
    void addKnob (const juce::String& id, const juce::String& text);

    VozListaAudioProcessor& proc;
    juce::ComboBox presetBox, modeBox;
    juce::TextButton prevBtn { "<" }, nextBtn { ">" };
    juce::Slider assistSlider;
    juce::Label assistLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> assistAtt;
    std::vector<std::unique_ptr<Knob>> knobs;

    void selectPreset (int idx);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VozListaEditor)
};
