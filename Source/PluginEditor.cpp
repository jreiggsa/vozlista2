#include "PluginEditor.h"

static const juce::Colour kBg    (0xff14161c);
static const juce::Colour kPanel (0xff1e222b);
static const juce::Colour kAccent(0xff35e0c1);

VozListaEditor::VozListaEditor (VozListaAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setSize (720, 420);

    getLookAndFeel().setColour (juce::Slider::rotarySliderFillColourId, kAccent);
    getLookAndFeel().setColour (juce::Slider::thumbColourId, juce::Colours::white);
    getLookAndFeel().setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff3a3f4b));
    getLookAndFeel().setColour (juce::ComboBox::backgroundColourId, kPanel);
    getLookAndFeel().setColour (juce::TextButton::buttonColourId, kPanel);

    // presets
    for (int i = 0; i < proc.getNumPrograms(); ++i)
        presetBox.addItem (proc.getProgramName (i), i + 1);
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this] { selectPreset (presetBox.getSelectedId() - 1); };
    prevBtn.onClick = [this] { selectPreset ((proc.getCurrentProgram() + proc.getNumPrograms() - 1) % proc.getNumPrograms()); };
    nextBtn.onClick = [this] { selectPreset ((proc.getCurrentProgram() + 1) % proc.getNumPrograms()); };
    addAndMakeVisible (presetBox); addAndMakeVisible (prevBtn); addAndMakeVisible (nextBtn);

    // modo
    modeBox.addItemList (VozListaAudioProcessor::getModeNames(), 1);
    addAndMakeVisible (modeBox);
    modeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (proc.apvts, "mode", modeBox);

    // assist grande
    assistSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    assistSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    assistSlider.setTextValueSuffix (" %");
    addAndMakeVisible (assistSlider);
    assistLabel.setText ("ASSIST", juce::dontSendNotification);
    assistLabel.setJustificationType (juce::Justification::centred);
    assistLabel.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    assistLabel.setColour (juce::Label::textColourId, kAccent);
    addAndMakeVisible (assistLabel);
    assistAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, "assist", assistSlider);

    addKnob ("cuerpo", "Cuerpo");
    addKnob ("pres",   "Presencia");
    addKnob ("air",    "Aire");
    addKnob ("comp",   "Compresion");
    addKnob ("deess",  "De-ess");
    addKnob ("sat",    "Saturacion");
    addKnob ("mix",    "Mix");
    addKnob ("output", "Salida");
}

void VozListaEditor::selectPreset (int idx)
{
    proc.setCurrentProgram (idx);
    presetBox.setSelectedId (idx + 1, juce::dontSendNotification);
}

void VozListaEditor::addKnob (const juce::String& id, const juce::String& text)
{
    auto k = std::make_unique<Knob>();
    k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
    k->label.setText (text, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, id, k->slider);
    addAndMakeVisible (k->slider);
    addAndMakeVisible (k->label);
    knobs.push_back (std::move (k));
}

void VozListaEditor::paint (juce::Graphics& g)
{
    g.fillAll (kBg);
    g.setColour (kPanel);
    g.fillRoundedRectangle (16.f, 70.f, 688.f, 334.f, 12.f);
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText ("VOZ LISTA", 20, 12, 250, 30, juce::Justification::left);
    g.setColour (kAccent);
    g.setFont (juce::FontOptions (13.0f));
    g.drawText ("Vocal Assist - de mic dinamico a sonido de estudio", 20, 40, 400, 20, juce::Justification::left);
}

void VozListaEditor::resized()
{
    auto top = getLocalBounds().removeFromTop (60).reduced (16, 14);
    nextBtn.setBounds (top.removeFromRight (34));
    top.removeFromRight (4);
    presetBox.setBounds (top.removeFromRight (230));
    top.removeFromRight (4);
    prevBtn.setBounds (top.removeFromRight (34));

    auto area = getLocalBounds().withTrimmedTop (80).reduced (30, 10);
    auto left = area.removeFromLeft (230);
    modeBox.setBounds (left.removeFromTop (28));
    left.removeFromTop (8);
    assistLabel.setBounds (left.removeFromTop (24));
    assistSlider.setBounds (left.reduced (10, 0));

    const int cols = 4, rows = 2;
    const int w = area.getWidth() / cols, h = area.getHeight() / rows;
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        auto cell = juce::Rectangle<int> (area.getX() + (int) (i % cols) * w,
                                          area.getY() + (int) (i / cols) * h, w, h).reduced (6);
        knobs[i]->label.setBounds (cell.removeFromTop (20));
        knobs[i]->slider.setBounds (cell);
    }
}
