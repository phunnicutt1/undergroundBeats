#include "undergroundBeats/audio/SamplePlayer.h"

namespace undergroundBeats {
namespace audio {

SamplePlayer::SamplePlayer(double sampleRate, int blockSize)
    : sourceSampleRate(44100.0),  // Default source sample rate
      playbackSampleRate(sampleRate),
      sourceBlockSize(blockSize),
      playbackSpeed(1.0),
      playbackVolume(1.0f),
      looping(false),
      playing(false),
      currentPlayPosition(0),
      positionIncrement(0.0)
{
    audioFile = std::make_unique<file::AudioFile>();
    volumeSmoothed.reset(sampleRate, 0.01);  // 10ms smoothing time
    volumeSmoothed.setTargetValue(playbackVolume);
}

SamplePlayer::~SamplePlayer()
{
    // audioFile will be automatically deleted
}

bool SamplePlayer::loadSample(const juce::String& filePath)
{
    if (audioFile->loadFromFile(filePath))
    {
        // Reset playback state
        stop();
        
        // Store the sample rate from the file
        sourceSampleRate = audioFile->getSampleRate();
        
        // Update position increment based on sample rates and speed
        positionIncrement = (sourceSampleRate / playbackSampleRate) * playbackSpeed;
        
        return true;
    }
    return false;
}

void SamplePlayer::setPosition(double positionInSeconds)
{
    if (audioFile->isLoaded())
    {
        // Convert position in seconds to samples
        int64_t positionInSamples = static_cast<int64_t>(positionInSeconds * sourceSampleRate);
        
        // Constrain to valid range
        currentPlayPosition = juce::jlimit<int64_t>(0, audioFile->getLengthInSamples() - 1, positionInSamples);
    }
}

double SamplePlayer::getPosition() const
{
    if (audioFile->isLoaded() && sourceSampleRate > 0)
    {
        return static_cast<double>(currentPlayPosition) / sourceSampleRate;
    }
    return 0.0;
}

void SamplePlayer::setSpeed(double speedFactor)
{
    // Constrain to a reasonable range (0.1x to 4.0x)
    playbackSpeed = juce::jlimit(0.1, 4.0, speedFactor);
    
    // Update position increment
    positionIncrement = (sourceSampleRate / playbackSampleRate) * playbackSpeed;
}

double SamplePlayer::getSpeed() const
{
    return playbackSpeed;
}

void SamplePlayer::setVolume(float volume)
{
    // Constrain to valid range
    playbackVolume = juce::jlimit(0.0f, 1.0f, volume);
    volumeSmoothed.setTargetValue(playbackVolume);
}

float SamplePlayer::getVolume() const
{
    return playbackVolume;
}

void SamplePlayer::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
}

bool SamplePlayer::isLooping() const
{
    return looping;
}

void SamplePlayer::play()
{
    if (audioFile->isLoaded())
    {
        playing = true;
    }
}

void SamplePlayer::pause()
{
    playing = false;
}

void SamplePlayer::stop()
{
    playing = false;
    currentPlayPosition = 0;
}

bool SamplePlayer::isPlaying() const
{
    return playing;
}

void SamplePlayer::processBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    // Clear the output buffer first
    buffer.clear();
    
    if (!audioFile->isLoaded() || !playing)
    {
        return;
    }
    
    const juce::AudioBuffer<float>& sourceBuffer = audioFile->getAudioBuffer();
    int numSourceChannels = sourceBuffer.getNumChannels();
    int numOutputChannels = buffer.getNumChannels();
    int64_t sourceLength = sourceBuffer.getNumSamples();
    
    // Don't process if the source is empty
    if (sourceLength <= 0)
    {
        return;
    }
    
    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Check if we've reached the end
        if (currentPlayPosition >= sourceLength)
        {
            if (looping)
            {
                currentPlayPosition = 0;  // Loop back to the beginning
            }
            else
            {
                playing = false;
                break;  // Stop processing
            }
        }
        
        // Get the current smoothed volume value
        float currentVolume = volumeSmoothed.getNextValue();
        
        // Get the integer sample position and the fraction for interpolation
        int pos1 = static_cast<int>(currentPlayPosition);
        int pos2 = pos1 + 1;
        if (pos2 >= sourceLength)
        {
            if (looping)
            {
                pos2 = 0;  // Wrap around for interpolation
            }
            else
            {
                pos2 = pos1;  // Clamp to last sample
            }
        }
        
        // Calculate the fractional part for linear interpolation
        double alpha = currentPlayPosition - pos1;
        
        // Copy and interpolate each channel
        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            int sourceChannel = channel % numSourceChannels;  // Handle case where output channels > source channels
            
            // Get samples for interpolation
            float sample1 = sourceBuffer.getSample(sourceChannel, pos1);
            float sample2 = sourceBuffer.getSample(sourceChannel, pos2);
            
            // Linear interpolation between the two samples
            float interpolatedSample = static_cast<float>((1.0 - alpha) * sample1 + alpha * sample2);
            
            // Apply volume and write to output buffer
            buffer.addSample(channel, sample, interpolatedSample * currentVolume);
        }
        
        // Increment the playback position
        currentPlayPosition += positionIncrement;
        
        // Handle looping
        if (currentPlayPosition >= sourceLength)
        {
            if (looping)
            {
                currentPlayPosition = currentPlayPosition - sourceLength;  // Wrap around preserving the fractional part
            }
            else
            {
                currentPlayPosition = sourceLength;  // Clamp at the end
                playing = false;
                break;  // Stop processing
            }
        }
    }
}

void SamplePlayer::prepareToPlay(double sampleRate, int blockSize)
{
    playbackSampleRate = sampleRate;
    sourceBlockSize = blockSize;
    
    // Update position increment
    positionIncrement = (sourceSampleRate / playbackSampleRate) * playbackSpeed;
    
    // Reset the volume smoother
    volumeSmoothed.reset(sampleRate, 0.01);  // 10ms smoothing time
    volumeSmoothed.setTargetValue(playbackVolume);
}

} // namespace audio
} // namespace undergroundBeats