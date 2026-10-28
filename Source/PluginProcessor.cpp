#include "PluginProcessor.h"
#include "PluginEditor.h"

using C = juce::dsp::IIR::Coefficients<float>;

// ------------------------------------------------------------------ modos
const juce::StringArray& VozListaAudioProcessor::getModeNames()
{
    static const juce::StringArray n { "Limpio", "Pop", "Rap / Trap", "Rock", "Podcast", "Dinamico a Estudio" };
    return n;
}

static const ModeTarget kModes[6] = {
    //  hpf  prox  mud   box   pres  air   thr    ratio deess sat
    {   80, -1.0f, -1.5f,  0.0f, 2.0f, 2.0f, -18.f, 2.5f, 0.4f, 0.05f }, // Limpio
    {   90, -2.0f, -2.0f, -1.0f, 3.0f, 4.0f, -22.f, 3.5f, 0.6f, 0.15f }, // Pop
    {  100, -2.0f, -3.0f, -1.0f, 4.0f, 3.0f, -26.f, 4.5f, 0.5f, 0.25f }, // Rap / Trap
    {  110, -1.0f, -2.0f, -1.5f, 5.0f, 2.0f, -24.f, 4.0f, 0.4f, 0.40f }, // Rock
    {   90, -3.0f, -2.5f, -1.0f, 3.0f, 1.5f, -24.f, 3.0f, 0.5f, 0.10f }, // Podcast
    {  100, -4.0f, -3.0f, -2.0f, 5.0f, 6.0f, -22.f, 3.5f, 0.6f, 0.20f }, // Dinamico a Estudio
};

// ------------------------------------------------------------------ presets
const std::vector<Preset>& VozListaAudioProcessor::getPresets()
{
    //                    nombre                        modo assist cuerpo pres  air  comp deess sat
    static const std::vector<Preset> p = {
        { "Init (neutro)",                               0,   0,   0.f,  0.f, 0.f, 1.0f, 1.0f, 1.0f },
        // --- Microfono dinamico barato
        { "Dinamico a Estudio - Suave",                  5,  45,   0.f,  0.f, 0.f, 1.0f, 1.0f, 1.0f },
        { "Dinamico a Estudio - Estandar",               5,  70,   0.f,  0.f, 0.f, 1.0f, 1.0f, 1.0f },
        { "Dinamico a Estudio - Maximo",                 5, 100,   0.5f, 0.f, 0.f, 1.2f, 1.0f, 1.0f },
        { "SM58 Brillante",                              5,  80,   0.f,  1.f, 1.f, 1.0f, 1.2f, 0.8f },
        { "Micro barato sin cuerpo",                     5,  75,   3.f,  0.f, 0.f, 1.0f, 1.0f, 1.2f },
        { "Micro barato muy sibilante",                  5,  75,   0.f, -1.f, 0.f, 1.0f, 1.5f, 1.0f },
        { "Micro opaco / apagado",                       5,  85,  -1.f,  2.f, 3.f, 0.9f, 1.2f, 0.8f },
        { "Cuarto con eco (cuarto sin tratar)",          5,  90,  -1.f,  1.f, 1.f, 1.3f, 1.0f, 1.0f },
        // --- Pop / urbano
        { "Pop Limpio",                                  1,  60,   0.f,  0.f, 0.f, 1.0f, 1.0f, 1.0f },
        { "Pop Radio",                                   1,  85,   0.5f, 1.f, 1.f, 1.2f, 1.0f, 1.1f },
        { "Pop Aireado",                                 1,  75,  -0.5f, 0.f, 3.f, 0.9f, 1.1f, 0.7f },
        { "Balada Intima",                               0,  70,   2.f, -1.f, 1.f, 0.8f, 0.8f, 0.6f },
        { "Reggaeton Vocal",                             2,  70,   0.5f, 1.f, 1.f, 1.1f, 1.1f, 0.9f },
        { "Urbano Moderno",                              2,  60,   1.f,  0.f, 2.f, 1.0f, 1.0f, 0.8f },
        // --- Rap / Trap
        { "Rap Agresivo",                                2,  90,   1.f,  2.f, 0.f, 1.4f, 1.0f, 1.3f },
        { "Rap Melodico",                                2,  65,   0.f,  0.f, 2.f, 1.0f, 1.1f, 0.8f },
        { "Trap Oscuro",                                 2,  75,   2.f, -2.f, 0.f, 1.2f, 0.8f, 1.2f },
        { "Trap Brillante",                              2,  80,  -0.5f, 2.f, 3.f, 1.2f, 1.2f, 1.0f },
        { "Ad-libs",                                     2,  85,  -1.f,  2.f, 2.f, 1.4f, 1.0f, 1.4f },
        // --- Rock
        { "Rock Potente",                                3,  85,   1.f,  1.f, 0.f, 1.2f, 0.9f, 1.4f },
        { "Rock Suave",                                  3,  55,   0.f,  0.f, 1.f, 0.9f, 0.9f, 0.9f },
        // --- Podcast / voz hablada
        { "Podcast Clasico",                             4,  70,   0.f,  0.f, 0.f, 1.0f, 1.0f, 1.0f },
        { "Locutor Profundo",                            4,  75,   3.f, -1.f, 0.f, 1.1f, 0.9f, 0.8f },
        { "Radio FM",                                    4,  90,   1.f,  2.f, 0.f, 1.5f, 1.1f, 1.2f },
        { "Narracion / Audiolibro",                      4,  55,   1.f, -1.f, 0.f, 0.8f, 0.9f, 0.5f },
        { "Streaming / Directo",                         4,  80,   0.f,  1.f, 0.f, 1.3f, 1.2f, 0.8f },
        // --- Coros / extras
        { "Coros Brillantes",                            1,  80,  -1.f,  1.f, 3.f, 1.3f, 1.2f, 0.7f },
        { "Backing Vocals (atras de la mezcla)",         0,  75,  -1.f, -1.f, 1.f, 1.4f, 1.1f, 0.6f },
        { "Susurro / ASMR",                              0,  60,   1.f, -1.f, 2.f, 0.7f, 1.5f, 0.3f },
        { "Demo Rapido",                                 1,  50,   0.f,  0.f, 0.f, 1.0f, 1.0f, 1.0f },
    };
    return p;
}

// ------------------------------------------------------------------ parametros
juce::AudioProcessorValueTreeState::ParameterLayout VozListaAudioProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    l.add (std::make_unique<AudioParameterChoice> (ParameterID { "mode", 1 }, "Modo", getModeNames(), 5));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "assist", 1 }, "Assist", NormalisableRange<float> (0.f, 100.f, 0.1f), 60.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "cuerpo", 1 }, "Cuerpo", NormalisableRange<float> (-6.f, 6.f, 0.1f), 0.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "pres", 1 },   "Presencia", NormalisableRange<float> (-6.f, 6.f, 0.1f), 0.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "air", 1 },    "Aire", NormalisableRange<float> (-6.f, 6.f, 0.1f), 0.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "comp", 1 },   "Compresion", NormalisableRange<float> (0.f, 1.5f, 0.01f), 1.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "deess", 1 },  "De-ess", NormalisableRange<float> (0.f, 1.5f, 0.01f), 1.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "sat", 1 },    "Saturacion", NormalisableRange<float> (0.f, 1.5f, 0.01f), 1.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "mix", 1 },    "Mix", NormalisableRange<float> (0.f, 1.f, 0.01f), 1.f));
    l.add (std::make_unique<AudioParameterFloat>  (ParameterID { "output", 1 }, "Salida", NormalisableRange<float> (-12.f, 12.f, 0.1f), 0.f));
    return l;
}

VozListaAudioProcessor::VozListaAudioProcessor()
    : AudioProcessor (BusesProperties()
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
}

bool VozListaAudioProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    auto in = l.getMainInputChannelSet(), out = l.getMainOutputChannelSet();
    return in == out && (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo());
}

// ------------------------------------------------------------------ presets API
int VozListaAudioProcessor::getNumPrograms() { return (int) getPresets().size(); }

const juce::String VozListaAudioProcessor::getProgramName (int i)
{
    return juce::isPositiveAndBelow (i, getNumPrograms()) ? getPresets()[(size_t) i].name : "";
}

void VozListaAudioProcessor::setParam (const juce::String& id, float v)
{
    if (auto* p = apvts.getParameter (id))
        p->setValueNotifyingHost (p->convertTo0to1 (v));
}

void VozListaAudioProcessor::setCurrentProgram (int index)
{
    if (! juce::isPositiveAndBelow (index, getNumPrograms())) return;
    currentPreset = index;
    const auto& p = getPresets()[(size_t) index];
    setParam ("mode", (float) p.mode);
    setParam ("assist", p.assist);
    setParam ("cuerpo", p.cuerpo);
    setParam ("pres", p.pres);
    setParam ("air", p.air);
    setParam ("comp", p.comp);
    setParam ("deess", p.deess);
    setParam ("sat", p.sat);
}

// ------------------------------------------------------------------ audio
void VozListaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock,
                                  (juce::uint32) juce::jmax (1, getTotalNumInputChannels()) };
    compressor.prepare (spec);
    compressor.setAttack (6.0f);
    compressor.setRelease (90.0f);
    dry.setSize (2, samplesPerBlock);

    for (auto* b : { &hpf, &lowShelf, &mud, &box, &pres, &air, &sibHP })
        for (auto& f : *b) f.reset();

    auto sib = C::makeHighPass (sr, 6500.0f, 0.707f);
    for (auto& f : sibHP) f.coefficients = sib;

    sibEnv = fullEnv = 0.f;
    lastOutGain = 1.f;
}

static inline float saturate (float x, float drive)
{
    if (drive < 0.001f) return x;
    const float k = 1.0f + drive * 3.0f;
    const float bias = 0.05f * drive;   // un poco de asimetria = armonicos pares
    return (std::tanh (k * (x + bias)) - std::tanh (k * bias)) / k;
}

void VozListaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numCh = juce::jmin (buffer.getNumChannels(), 2);
    const int n = buffer.getNumSamples();
    for (int c = numCh; c < buffer.getNumChannels(); ++c) buffer.clear (c, 0, n);

    if (dry.getNumSamples() < n) dry.setSize (2, n, false, false, true);
    for (int c = 0; c < numCh; ++c) dry.copyFrom (c, 0, buffer, c, 0, n);

    // ---- parametros -> ajustes efectivos
    const int mode = juce::jlimit (0, 5, (int) get ("mode"));
    const auto& t = kModes[mode];
    const float a = get ("assist") / 100.0f;
    const float cScale = get ("comp"), dScale = get ("deess"), sScale = get ("sat");
    const float mixAmt = get ("mix");
    const float outGain = juce::Decibels::decibelsToGain (get ("output"));
    const float fs = (float) sr;

    const float hpfHz   = 20.0f + (t.hpf - 20.0f) * a;
    const float proxDb  = t.prox * a + get ("cuerpo");
    const float mudDb   = t.mud * a;
    const float boxDb   = t.box * a;
    const float presDb  = t.pres * a + get ("pres");
    const float airDb   = t.air * a + get ("air");
    const float ac      = juce::jmin (1.0f, a * cScale);
    const float thr     = t.thr * ac;
    const float ratio   = 1.0f + (t.ratio - 1.0f) * ac;
    const float makeup  = juce::Decibels::decibelsToGain (-thr * (1.0f - 1.0f / ratio) * 0.5f);
    const float satAmt  = t.sat * a * sScale;
    const float deAmt   = juce::jmin (1.5f, t.deess * a * dScale);

    auto setB = [] (Band& b, C::Ptr p) { for (auto& f : b) f.coefficients = p; };
    setB (hpf,      C::makeHighPass (sr, juce::jlimit (20.f, 400.f, hpfHz), 0.707f));
    setB (lowShelf, C::makeLowShelf (sr, 150.f, 0.7f,  juce::Decibels::decibelsToGain (proxDb)));
    setB (mud,      C::makePeakFilter (sr, 300.f, 1.0f, juce::Decibels::decibelsToGain (mudDb)));
    setB (box,      C::makePeakFilter (sr, 550.f, 1.2f, juce::Decibels::decibelsToGain (boxDb)));
    setB (pres,     C::makePeakFilter (sr, 4500.f, 0.9f, juce::Decibels::decibelsToGain (presDb)));
    setB (air,      C::makeHighShelf (sr, 11000.f, 0.7f, juce::Decibels::decibelsToGain (airDb)));

    // ---- 1) HPF + EQ sustractivo
    for (int c = 0; c < numCh; ++c)
    {
        auto* d = buffer.getWritePointer (c);
        for (int i = 0; i < n; ++i)
        {
            float x = d[i];
            x = hpf[(size_t) c].processSample (x);
            x = lowShelf[(size_t) c].processSample (x);
            x = mud[(size_t) c].processSample (x);
            x = box[(size_t) c].processSample (x);
            d[i] = x;
        }
    }

    // ---- 2) Compresor
    if (ratio > 1.01f)
    {
        compressor.setThreshold (thr);
        compressor.setRatio (ratio);
        juce::dsp::AudioBlock<float> block (buffer);
        auto sub = block.getSubsetChannelBlock (0, (size_t) numCh);
        juce::dsp::ProcessContextReplacing<float> ctx (sub);
        compressor.process (ctx);
        for (int c = 0; c < numCh; ++c) buffer.applyGain (c, 0, n, makeup);
    }

    // ---- 3) Presencia + aire + saturacion + de-esser (relativo, no depende del nivel)
    const float envC = 1.0f - std::exp (-1.0f / (0.02f * fs));
    const float sibAtk = 1.0f - std::exp (-1.0f / (0.001f * fs));
    const float sibRel = 1.0f - std::exp (-1.0f / (0.03f * fs));
    const float ratioThr = 0.40f;

    float* ch[2] = { buffer.getWritePointer (0), numCh > 1 ? buffer.getWritePointer (1) : nullptr };

    for (int i = 0; i < n; ++i)
    {
        float xs[2] = { 0, 0 }, ss[2] = { 0, 0 }, sibMax = 0, fullMax = 0;
        for (int c = 0; c < numCh; ++c)
        {
            float x = pres[(size_t) c].processSample (ch[c][i]);
            x = air[(size_t) c].processSample (x);
            x = saturate (x, satAmt);
            xs[c] = x;
            ss[c] = sibHP[(size_t) c].processSample (x);
            sibMax  = juce::jmax (sibMax, std::abs (ss[c]));
            fullMax = juce::jmax (fullMax, std::abs (x));
        }
        sibEnv  += (sibMax > sibEnv ? sibAtk : sibRel) * (sibMax - sibEnv);
        fullEnv += envC * (fullMax - fullEnv);

        float g = 1.0f;
        if (deAmt > 0.001f)
        {
            const float rel = sibEnv / (fullEnv + 1.0e-5f);
            if (rel > ratioThr)
                g = juce::jmax (0.2f, std::pow (ratioThr / rel, deAmt * 1.2f));
        }
        for (int c = 0; c < numCh; ++c)
            ch[c][i] = xs[c] - ss[c] * (1.0f - g);
    }

    // ---- 4) Mix + salida
    for (int c = 0; c < numCh; ++c)
    {
        auto* w = buffer.getWritePointer (c);
        auto* dr = dry.getReadPointer (c);
        for (int i = 0; i < n; ++i) w[i] = dr[i] * (1.0f - mixAmt) + w[i] * mixAmt;
    }
    buffer.applyGainRamp (0, n, lastOutGain, outGain);
    lastOutGain = outGain;
}

// ------------------------------------------------------------------ estado
void VozListaAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);
}

void VozListaAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* VozListaAudioProcessor::createEditor() { return new VozListaEditor (*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VozListaAudioProcessor(); }
