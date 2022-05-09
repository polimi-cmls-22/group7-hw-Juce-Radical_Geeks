/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    // fill circle
    g.setColour(Colours::grey);
    g.fillEllipse(bounds);
    
    // draw border of circle
    g.setColour(Colours::darkred);
    g.drawEllipse(bounds, 1.f);
    
    if( auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
        
        p.addRoundedRectangle(r, 2.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        // map angle and value
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        // draw rectangle, container of the text, and write the text
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());

        g.setColour(Colours::black);
        g.fillRect(r);
        
        g.setFont(rswl->getTextHeight());
        g.setColour(Colours::lightgrey);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    Path powerButton;
    
    auto bounds = toggleButton.getLocalBounds();
    
    auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
    
    float ang = 30.f; //30.f;
    
    size -= 6;
    
    powerButton.addCentredArc(r.getCentreX(),
                              r.getCentreY(),
                              size * 0.5,
                              size * 0.5,
                              0.f,
                              degreesToRadians(ang),
                              degreesToRadians(360.f - ang),
                              true);
    
    powerButton.startNewSubPath(r.getCentreX(), r.getY());
    powerButton.lineTo(r.getCentre());
    
    PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);
    
    auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colours::yellow;
    
    g.setColour(color);
    g.strokePath(powerButton, pst);
    g.drawEllipse(r, 2);
}
//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();
    
    auto bounds = getLocalBounds();
    
    g.setColour(Colours::black);
    g.drawFittedText(getName(),
                     bounds.removeFromTop(getTextHeight()+2),
                     Justification::centredBottom,
                     1);
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
    
    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;
    
    g.setColour(Colours::black);
    g.setFont(getTextHeight());
    
    auto numChoices = labels.size();
    for( int i = 0; i < numChoices; ++i )
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);
        
        // map position as angle and position as uniform value
        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
        
        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);
        
        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());
        
        // draw the text of the labels
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
    
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(getTextHeight() * 1.5);
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(bounds.getY());
    
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if( auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param) )
        return choiceParam->getCurrentChoiceName();
    
    juce::String str;
    bool addK = false;
    
    if( auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param) )
    {
        float val = getValue();
        
        // if val < 1000 Hz, use Hertz, else use Kilo Hertz
        if( val > 999.f )
        {
            val /= 1000.f; //1001 / 1000 = 1.001
            addK = true;
        }
        
        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse; //this shouldn't happen!
    }
    
    if( suffix.isNotEmpty() )
    {
        str << " ";
        if( addK )
            str << "k";
        
        str << suffix;
    }
    
    return str;
}
//==============================================================================
ResponseCurveComponent::ResponseCurveComponent(YetAnotherAutoWahAudioProcessor& p) :
audioProcessor(p),
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for( auto param : params )
    {
        param->addListener(this);
    }

    updateChain();
    
    // refresh rate: 60 Hz
    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for( auto param : params )
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::updateResponseCurve()
{
    using namespace juce;

    auto responseArea = getAnalysisArea();
    auto w = responseArea.getWidth();
    
    auto& peak = monoChain.get<ChainPositions::Peak>();
    
    auto sampleRate = audioProcessor.getSampleRate();
    
    std::vector<double> mags;
    // set the size of mags equal to w elements
    mags.resize(w);
    
    for( int i = 0; i < w; ++i )
    {
        double mag = 1.f;
        // frequency is shown as a logarithmic scale
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);
        
        // get magnitude of frequency response for the current value of frequency
        mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        // convert magnitude to Decibels
        mags[i] = Decibels::gainToDecibels(mag);
    }
    
    responseCurve.clear();
    
    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        // the gain is btw -24 dB and 24 dB, so map it btw min position and max position of the area
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };
    
    // we have the values of the magnitude in the mags vector, let's draw a line that follows the values
    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));
    
    for( size_t i = 1; i < mags.size(); ++i )
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }
}

void ResponseCurveComponent::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::white);

    drawBackgroundGrid(g);

    auto renderArea = getAnalysisArea();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();

    auto left = renderArea.getX();
    auto width = renderArea.getWidth();

    auto chainSettings = getChainSettings(audioProcessor.apvts);

    auto minFreq = chainSettings.peakMinFreq;
    auto maxFreq = chainSettings.peakMaxFreq;

    // convert sweep freq to decade scale
    auto normMinFreq = juce::mapFromLog10(minFreq, 20.f, 20000.f);
    auto normMaxFreq = juce::mapFromLog10(maxFreq, 20.f, 20000.f);

    auto xMinFreq = left + width * normMinFreq;
    auto xMaxFreq = left + width * normMaxFreq;

    // draw the line of min sweep freq
    g.setColour(Colours::red);
    g.drawVerticalLine(xMinFreq, top, bottom);

    // draw the line of max sweep freq
    g.setColour(Colours::green);
    g.drawVerticalLine(xMaxFreq, top, bottom);
    
    auto responseArea = getAnalysisArea();
    
    if( shouldShowFFTAnalysis )
    {
        // FFT is stereo so we have a color for left and another for right
        auto leftChannelFFTPath = leftPathProducer.getPath();
        leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
        
        g.setColour(Colours::blue);
        g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));
        
        auto rightChannelFFTPath = rightPathProducer.getPath();
        rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
        
        g.setColour(Colours::yellow);
        g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
    }
    
    g.setColour(Colours::darkblue);
    g.strokePath(responseCurve, PathStrokeType(2.f));
    
    Path border;
    
    border.setUsingNonZeroWinding(false);
    
    border.addRoundedRectangle(getRenderArea(), 4);
    border.addRectangle(getLocalBounds());
    
    g.setColour(Colours::lightgrey);
    
    g.fillPath(border);
    
    drawTextLabels(g);
    
    g.setColour(Colours::black);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
}

std::vector<float> ResponseCurveComponent::getFrequencies()
{
    return std::vector<float>
    {
        // frequencies in decades
        20, 50, 100,
        200, 500, 1000,
        2000, 5000, 10000,
        20000
    };
}

std::vector<float> ResponseCurveComponent::getGains()
{
    return std::vector<float>
    {
        // Decibels
        -24, -12, 0, 12, 24
    };
}

std::vector<float> ResponseCurveComponent::getXs(const std::vector<float> &freqs, float left, float width)
{
    std::vector<float> xs;
    for( auto f : freqs )
    {
        // get decade scale for the frequency
        auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
        xs.push_back( left + width * normX );
    }
    
    return xs;
}

void ResponseCurveComponent::drawBackgroundGrid(juce::Graphics &g)
{
    using namespace juce;
    auto freqs = getFrequencies();
    
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    auto xs = getXs(freqs, left, width);
    
    g.setColour(Colours::black);
    for( auto x : xs )
    {
        // draw the decade scale for frequency
        g.drawVerticalLine(x, top, bottom);
    }
    
    auto gain = getGains();
    
    for( auto gDb : gain )
    {
        // draw the dB scale for magnitude
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        
        g.setColour(gDb == 0.f ? Colours::black : Colours::black );
        g.drawHorizontalLine(y, left, right);
    }
}

void ResponseCurveComponent::drawTextLabels(juce::Graphics &g)
{
    using namespace juce;
    g.setColour(Colours::darkred);
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    auto freqs = getFrequencies();
    auto xs = getXs(freqs, left, width);
    
    for( int i = 0; i < freqs.size(); ++i )
    {
        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        String str;
        if( f > 999.f )
        {
            addK = true;
            f /= 1000.f;
        }

        // Hz or kHz
        str << f;
        if( addK )
            str << "k";
        str << "Hz";
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;

        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(1);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    auto gain = getGains();

    for( auto gDb : gain )
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        
        // positive or negative
        String str;
        if( gDb > 0 )
            str << "+";
        str << gDb;
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);
        
        g.setColour(gDb == 0.f ? Colours::black : Colours::black );
        
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
        
        str.clear();
        str << (gDb - 24.f);

        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::black);
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    
    responseCurve.preallocateSpace(getWidth() * 3);
    updateResponseCurve();
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    while( leftChannelFifo->getNumCompleteBuffersAvailable() > 0 )
    {
        if( leftChannelFifo->getAudioBuffer(tempIncomingBuffer) )
        {
            auto size = tempIncomingBuffer.getNumSamples();

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                                              monoBuffer.getReadPointer(0, size),
                                              monoBuffer.getNumSamples() - size);

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                                              tempIncomingBuffer.getReadPointer(0, 0),
                                              size);
            
            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
        }
    }
    
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / double(fftSize);

    while( leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0 )
    {
        std::vector<float> fftData;
        if( leftChannelFFTDataGenerator.getFFTData( fftData) )
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }
    
    while( pathProducer.getNumPathsAvailable() > 0 )
    {
        pathProducer.getPath( leftChannelFFTPath );
    }
}

void ResponseCurveComponent::timerCallback()
{
    if( shouldShowFFTAnalysis )
    {
        auto fftBounds = getAnalysisArea().toFloat();
        auto sampleRate = audioProcessor.getSampleRate();
        
        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }

    // update chain and response curve everytime this callback is called (60 Hz)
    updateChain();
    updateResponseCurve();
    
    repaint();
}

void ResponseCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    
    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    
    return bounds;
}


juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
//==============================================================================
YetAnotherAutoWahAudioProcessorEditor::YetAnotherAutoWahAudioProcessorEditor (YetAnotherAutoWahAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

// associating the parameters to the sliders, setting the unit of measurement and the title of the slider
peakMinFreqSlider(*audioProcessor.apvts.getParameter("Sweep Min Freq"), "Hz", "Sweep Min Freq"),
peakMaxFreqSlider(*audioProcessor.apvts.getParameter("Sweep Max Freq"), "Hz", "Sweep Max Freq"),
peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB", "Peak Gain"),
peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), "", "Peak Quality"),
sweepFreqSlider(*audioProcessor.apvts.getParameter("Sweep Freq"), "Hz", "Sweep Freq"),
mixSlider(*audioProcessor.apvts.getParameter("Mix"), "%", "Mix"),

responseCurveComponent(audioProcessor),

// associating the labels
peakMinFreqSliderAttachment(audioProcessor.apvts, "Sweep Min Freq", peakMinFreqSlider),
peakMaxFreqSliderAttachment(audioProcessor.apvts, "Sweep Max Freq", peakMaxFreqSlider),
peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
sweepFreqSliderAttachment(audioProcessor.apvts, "Sweep Freq", sweepFreqSlider),
mixSliderAttachment(audioProcessor.apvts, "Mix", mixSlider),

// button to toggle FFT of the input
analyzerEnabledButtonAttachment(audioProcessor.apvts, "Analyzer Enabled", analyzerEnabledButton),

comboBoxAttachment(audioProcessor.apvts, "Sweep Type", comboBox)
{
    // setting the labels
    peakMinFreqSlider.labels.add({ 0.f, "0.5Hz" });
    peakMinFreqSlider.labels.add({ 1.f, "20kHz" });

    peakMaxFreqSlider.labels.add({ 0.f, "0.5Hz" });
    peakMaxFreqSlider.labels.add({ 1.f, "20kHz" });
    
    peakGainSlider.labels.add({ 0.f, "-24dB" });
    peakGainSlider.labels.add({ 1.f, "+24dB" });
    
    peakQualitySlider.labels.add({ 0.f, "0.1" });
    peakQualitySlider.labels.add({ 1.f, "10.0" });

    sweepFreqSlider.labels.add({ 0.f, "0.5Hz" });
    sweepFreqSlider.labels.add({ 1.f, "20Hz" });

    mixSlider.labels.add({ 0.f, "0%" });
    mixSlider.labels.add({ 1.f, "100%" });
    
    // show all the components
    for( auto* comp : getComps() )
    {
        addAndMakeVisible(comp);
    }

    analyzerEnabledButton.setLookAndFeel(&lnf);

    // click listener for the button
    auto safePtr = juce::Component::SafePointer<YetAnotherAutoWahAudioProcessorEditor>(this);

    analyzerEnabledButton.onClick = [safePtr]()
    {
        if( auto* comp = safePtr.getComponent() )
        {
            auto enabled = ! comp->analyzerEnabledButton.getToggleState();
            comp->responseCurveComponent.toggleAnalysisEnablement(enabled);
        }
    };

    peakMinFreqSlider.onValueChange = [this]()
    {
        // the max sweep freq shouldn't go below the min sweep freq
        // so, if the min sweep freq increases and surpasses the max sweep freq, set the max sweep freq to the minimum valid value
        peakMaxFreqSlider.setRange(peakMinFreqSlider.getValue() - std::numeric_limits<float>::epsilon(), 20000.f);
    };
    
	// Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (480, 500);
}

YetAnotherAutoWahAudioProcessorEditor::~YetAnotherAutoWahAudioProcessorEditor()
{
    analyzerEnabledButton.setLookAndFeel(nullptr);
}

//==============================================================================
void YetAnotherAutoWahAudioProcessorEditor::paint(juce::Graphics &g)
{
    using namespace juce;

    // drawing the rectangles to delimit the knobs

    g.fillAll(Colours::lightgrey);

    Path curve;

    auto bounds = getLocalBounds();
    auto center = bounds.getCentre();

    auto cornerSize = 20;
    auto curvePos = curve.getCurrentPosition();
    curve.quadraticTo(curvePos.getX() - cornerSize, curvePos.getY(),
        curvePos.getX() - cornerSize, curvePos.getY() - 16);
    curvePos = curve.getCurrentPosition();
    curve.quadraticTo(curvePos.getX(), 2,
        curvePos.getX() - cornerSize, 2);

    curve.lineTo({ 0.f, 2.f });
    curve.lineTo(0.f, 0.f);
    curve.lineTo(center.x, 0.f);
    curve.closeSubPath();

    g.setColour(Colours::blue);
    g.fillPath(curve);

    curve.applyTransform(AffineTransform().scaled(-1, 1));
    curve.applyTransform(AffineTransform().translated(getWidth(), 0));
    g.fillPath(curve);

    g.setColour(Colours::black);
    g.setFont(14);

    auto height = bounds.getHeight();
    auto sweepArea = bounds.removeFromTop(height * 0.33).toFloat().reduced(10.0f);
    auto peakArea = bounds.removeFromBottom(height * 0.33).toFloat().reduced(10.0f);
    g.setColour(Colours::darkred);
    g.drawRoundedRectangle(sweepArea, 5.0f, 3.0f);
    g.drawRoundedRectangle(peakArea, 5.0f, 3.0f);
    
    // draw the title of the toggle button
    g.setColour(Colours::black);
    g.drawFittedText("Show FFT", 365, 365, 100, 18, juce::Justification::centred, 1);
    g.drawFittedText("of input", 365, 380, 100, 18, juce::Justification::centred, 1);
}

void YetAnotherAutoWahAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromTop(8);
    bounds.removeFromLeft(8);

    auto height = bounds.getHeight();
    auto width = bounds.getWidth();
    auto sweepArea = bounds.removeFromTop(height * 0.33);
    auto peakAreaI = bounds.removeFromBottom(height * 0.33);
    auto peakArea = peakAreaI.removeFromBottom(peakAreaI.getHeight() * 0.90);
    auto responseArea = bounds;

    auto heightSweep = sweepArea.getHeight();
    auto widthSweep = sweepArea.getWidth();
    auto heightPeak = peakArea.getHeight();
    auto widthPeak = peakArea.getWidth();

    responseCurveComponent.setBounds(responseArea);
    peakMinFreqSlider.setBounds(sweepArea.removeFromLeft(widthSweep * 0.25));
    peakMaxFreqSlider.setBounds(sweepArea.removeFromLeft(widthSweep * 0.25));
    sweepFreqSlider.setBounds(sweepArea.removeFromLeft(widthSweep * 0.25));
    peakGainSlider.setBounds(peakArea.removeFromLeft(widthPeak * 0.25));
    peakQualitySlider.setBounds(peakArea.removeFromLeft(widthPeak * 0.25));
    mixSlider.setBounds(peakArea.removeFromLeft(widthPeak * 0.25));

    auto peakAreaButton = peakArea.removeFromTop(heightPeak * 0.70);
    auto peakAreaButton2 = peakAreaButton.removeFromBottom(heightPeak * 0.35);
    auto peakAreaButton3 = peakAreaButton2.removeFromLeft(widthPeak * 0.30);
    auto peakAreaButton4 = peakAreaButton3.removeFromRight(widthPeak * 0.30);

    analyzerEnabledButton.setBounds(peakAreaButton4);

    // filling the drop down menu with all the options
    comboBox.addItemList(audioProcessor.apvts.getParameter("Sweep Type")->getAllValueStrings(), 1);

    auto sweepTypeArea = sweepArea.removeFromTop(heightSweep * 0.65);
    auto sweepTypeArea2 = sweepTypeArea.removeFromBottom(sweepTypeArea.getHeight() * 0.65);
    auto sweepTypeArea3 = sweepTypeArea2.removeFromLeft(sweepTypeArea2.getWidth() * 0.80);

    comboBox.setBounds(sweepTypeArea3);

    comboBox.setSelectedId(1);
}

std::vector<juce::Component*> YetAnotherAutoWahAudioProcessorEditor::getComps()
{
    return
    {
        &peakMinFreqSlider,
        &peakMaxFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &sweepFreqSlider,
        &comboBox,
        &mixSlider,
        &responseCurveComponent,
        &analyzerEnabledButton
    };
}
