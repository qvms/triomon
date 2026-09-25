#include "WavFileAssembler.hpp"
#include <stdexcept>
#include <QtEndian>

namespace PacketHandlerFramework {
namespace FileTransfer {

WavFileAssembler::WavFileAssembler(const QList<std::shared_ptr<FileStreamAssembler>>& fileStreamAssemblerList, const QString& filename, const QString& destinationDirectory, long startFrameNumber, const QDateTime& startTime)
    : FileStreamAssembler(fileStreamAssemblerList, /* fiveTuple stub */ nullptr, false, "WAV", filename, destinationDirectory, "WAV audio file", startFrameNumber, startTime) {
}

void WavFileAssembler::WriteWavHeader(uint32_t& wc, AudioFormat audioFormat, uint8_t nChannels, uint8_t bitsPerSampleOut, uint32_t nSamples) {
    // Stub
}

void WavFileAssembler::AssembleAsWavFileNative(QIODevice* rawSamplesFileStream, AudioFormat format) {
    // Stub
}

uint32_t WavFileAssembler::CountSamplesInStreams(QIODevice* pcm16BitSamplesChannelA, QIODevice* pcm16BitSamplesChannelB) {
    // Stub
    return 0;
}

void WavFileAssembler::WriteSampleStreamToFile(uint32_t nSamples, QIODevice* pcm16BitSamplesChannelA, QIODevice* pcm16BitSamplesChannelB) {
    // Stub
}

void WavFileAssembler::WriteSampleStreamToFile(uint32_t nSamples, QIODevice* pcm16BitSamplesChannelA) {
    // Stub
}

} // namespace FileTransfer
} // namespace PacketHandlerFramework
