#pragma once
#include "FileStreamAssembler.hpp"
#include <QString>
#include <memory>
#include <QIODevice>
#include <vector>
#include <cstdint>

namespace PacketHandlerFramework {
namespace FileTransfer {

class WavFileAssembler : public FileStreamAssembler {
public:
    enum class AudioFormat : uint16_t {
        WAVE_FORMAT_PCM = 0x0001,
        WAVE_FORMAT_ALAW = 0x0006,
        WAVE_FORMAT_MULAW = 0x0007,
        WAVE_FORMAT_G729 = 0x0083
    };

    WavFileAssembler(const QList<std::shared_ptr<FileStreamAssembler>>& fileStreamAssemblerList, const QString& filename, const QString& destinationDirectory, long startFrameNumber, const QDateTime& startTime);
    virtual ~WavFileAssembler() = default;

    void AssembleAsWavFileNative(QIODevice* rawSamplesFileStream, AudioFormat format);
    uint32_t CountSamplesInStreams(QIODevice* pcm16BitSamplesChannelA, QIODevice* pcm16BitSamplesChannelB);
    void WriteSampleStreamToFile(uint32_t nSamples, QIODevice* pcm16BitSamplesChannelA, QIODevice* pcm16BitSamplesChannelB);
    void WriteSampleStreamToFile(uint32_t nSamples, QIODevice* pcm16BitSamplesChannelA);

private:
    void WriteWavHeader(uint32_t& wc, AudioFormat audioFormat, uint8_t nChannels, uint8_t bitsPerSampleOut, uint32_t nSamples);
};

} // namespace FileTransfer
} // namespace PacketHandlerFramework
