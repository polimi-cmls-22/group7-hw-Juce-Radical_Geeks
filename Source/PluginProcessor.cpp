/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// JUCE DEFAULT DECLARATIONS - START - DON'T EDIT
//==============================================================================
YetAnotherAutoWahAudioProcessor::YetAnotherAutoWahAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

YetAnotherAutoWahAudioProcessor::~YetAnotherAutoWahAudioProcessor()
{
}

//==============================================================================
const juce::String YetAnotherAutoWahAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YetAnotherAutoWahAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool YetAnotherAutoWahAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool YetAnotherAutoWahAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double YetAnotherAutoWahAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int YetAnotherAutoWahAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int YetAnotherAutoWahAudioProcessor::getCurrentProgram()
{
    return 0;
}

void YetAnotherAutoWahAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String YetAnotherAutoWahAudioProcessor::getProgramName (int index)
{
    return {};
}

void YetAnotherAutoWahAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}
// JUCE DEFAULT DECLARATIONS - END

//==============================================================================
void YetAnotherAutoWahAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    
    spec.numChannels = 1;
    
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    updateFilters();
    
    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);

    // phase of the sweep function
    phase = 0.0;

    // buffer for sound processing
    tempBufferLeft = juce::dsp::AudioBlock<float>(tempBufferMemoryLeft, spec.numChannels, spec.maximumBlockSize);
    tempBufferRight = juce::dsp::AudioBlock<float>(tempBufferMemoryRight, spec.numChannels, spec.maximumBlockSize);
}

// JUCE DEFAULT DECLARATION
void YetAnotherAutoWahAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

float YetAnotherAutoWahAudioProcessor::sweepFunction(float phase)
{
    auto sweepType = apvts.getRawParameterValue("Sweep Type")->load();

    // SINE
    if(sweepType == 0)
        return sin(phase);

    // TRIANGLE
    if (sweepType == 1) {
        if (phase < juce::MathConstants<float>::pi)
            return 2 * phase / juce::MathConstants<float>::pi - 1;
        else
            return 3 - 2 * phase / juce::MathConstants<float>::pi;
    }
    
    // SAWTOOTH
    if(sweepType == 2)
        return 1 - phase / juce::MathConstants<float>::pi;

    // INVERTED SAWTOOTH
    if (sweepType == 3)
        return phase / juce::MathConstants<float>::pi - 1;

    // RECTANGLE
    if (sweepType == 4) {
        if (phase < juce::MathConstants<float>::pi) {
            return 1;
        }
        else {
            return -1;
        }
    }
    
    return 0;
}


// JUCE DEFAULT DECLARATIONS - START - EDITED TO ONLY SUPPORT STEREO
#ifndef JucePlugin_PreferredChannelConfigurations
bool YetAnotherAutoWahAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported
    // We only support stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
// JUCE DEFAULT DECLARATIONS - END

void YetAnotherAutoWahAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	// START DEFAULT
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
	// END DEFAULT

    updateFilters();
    
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    // allocating buffer for wahwah wet
    tempBufferLeft.copyFrom(leftBlock);
    tempBufferRight.copyFrom(rightBlock);

    auto mix = apvts.getRawParameterValue("Mix")->load() / 100;

    // wet = mix
    tempBufferLeft.multiplyBy(static_cast<float> (mix));
    tempBufferRight.multiplyBy(static_cast<float> (mix));

    juce::dsp::ProcessContextReplacing<float> leftContext(tempBufferLeft);
    juce::dsp::ProcessContextReplacing<float> rightContext(tempBufferRight);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    // dry = 1 - mix
    leftBlock.multiplyBy(static_cast<float> (1.0 - mix));
    rightBlock.multiplyBy(static_cast<float> (1.0 - mix));

    // summing wet and dry
    leftBlock.add(tempBufferLeft);
    rightBlock.add(tempBufferRight);
    
    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);

    //********************************************************************************************//
    // Main audio processing loop
    auto sr = getSampleRate();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        for (int n = 0; n < buffer.getNumSamples(); ++n) {
			// ..do something to the data...

            float fc, minf, maxf, sweepFreq;
            minf = apvts.getRawParameterValue("Sweep Min Freq")->load();
            maxf = apvts.getRawParameterValue("Sweep Max Freq")->load();
            sweepFreq = apvts.getRawParameterValue("Sweep Freq")->load();
            // Update filter parameters depending on phase
            fc = (minf + maxf)/2 + (maxf - minf) / 2 * sweepFunction(phase);

            // updating phase modulo 2PI
            phase += 1/sr * sweepFreq;

            if (phase > juce::MathConstants<float>::twoPi)
                phase -= juce::MathConstants<float>::twoPi;

            // updating peak center frequency
            apvts.getRawParameterValue("Peak Freq")->store(fc);
        }
    }
    //********************************************************************************************//
}

// JUCE DEFAULT DECLARATIONS - START - DON'T EDIT
//==============================================================================
bool YetAnotherAutoWahAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* YetAnotherAutoWahAudioProcessor::createEditor()
{
    return new YetAnotherAutoWahAudioProcessorEditor (*this);
}
// JUCE DEFAULT DECLARATIONS - END

//==============================================================================
void YetAnotherAutoWahAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void YetAnotherAutoWahAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if( tree.isValid() )
    {
        apvts.replaceState(tree);
        updateFilters();
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    
    settings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    settings.peakMinFreq = apvts.getRawParameterValue("Sweep Min Freq")->load();
    settings.peakMaxFreq = apvts.getRawParameterValue("Sweep Max Freq")->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    
    return settings;
}

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                               chainSettings.peakFreq,
                                                               chainSettings.peakQuality,
                                                               juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));
}

void YetAnotherAutoWahAudioProcessor::updatePeakFilter(const ChainSettings &chainSettings)
{
    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate());
    
    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

void updateCoefficients(Coefficients &old, const Coefficients &replacements)
{
    *old = *replacements;
}

void YetAnotherAutoWahAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(apvts);
    
    updatePeakFilter(chainSettings);
}

juce::AudioProcessorValueTreeState::ParameterLayout YetAnotherAutoWahAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // add parameters to the layout

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
        750.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Sweep Min Freq",
                                                           "Sweep Min Freq",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                           750.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Sweep Max Freq",
        "Sweep Max Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
        750.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Gain",
                                                           "Peak Gain",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality",
                                                           "Peak Quality",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Sweep Freq",
        "Sweep Freq",
        juce::NormalisableRange<float>(0.5f, 20.f, 1.f, 0.25f),
        750.f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>("Analyzer Enabled", "Analyzer Enabled", false));

    layout.add(std::make_unique<juce::AudioParameterChoice>("Sweep Type", "Sweep Type", juce::StringArray("Sine", "Triangle", "Sawtooth", "Inverted sawtooth", "Square"), 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mix",
        "Mix",
        juce::NormalisableRange<float>(0.f, 100.f, 5.f),
        100.f));

   return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YetAnotherAutoWahAudioProcessor();
}
