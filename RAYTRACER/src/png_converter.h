#ifndef PNG_CONVERTER_H
#define PNG_CONVERTER_H

#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>

// Simple PNG writer without external dependencies
// Based on minimal PNG specification implementation

class PNGWriter {
private:
    uint32_t crc_table[256];

    void init_crc_table() {
        for (uint32_t n = 0; n < 256; n++) {
            uint32_t c = n;
            for (int k = 0; k < 8; k++) {
                if (c & 1) {
                    c = 0xedb88320 ^ (c >> 1);
                } else {
                    c = c >> 1;
                }
            }
            crc_table[n] = c;
        }
    }

    uint32_t crc(const uint8_t* buf, size_t len) {
        uint32_t c = 0xffffffff;
        for (size_t n = 0; n < len; n++) {
            c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
        }
        return c ^ 0xffffffff;
    }

    void write_big_endian_u32(std::vector<uint8_t>& buffer, uint32_t value) {
        buffer.push_back((value >> 24) & 0xff);
        buffer.push_back((value >> 16) & 0xff);
        buffer.push_back((value >> 8) & 0xff);
        buffer.push_back(value & 0xff);
    }

    void write_chunk(std::vector<uint8_t>& file_buffer, const char* type, const std::vector<uint8_t>& data) {
        // Write length
        write_big_endian_u32(file_buffer, static_cast<uint32_t>(data.size()));

        // Write type
        file_buffer.push_back(type[0]);
        file_buffer.push_back(type[1]);
        file_buffer.push_back(type[2]);
        file_buffer.push_back(type[3]);

        // Write data
        file_buffer.insert(file_buffer.end(), data.begin(), data.end());

        // Calculate and write CRC
        std::vector<uint8_t> crc_data;
        crc_data.push_back(type[0]);
        crc_data.push_back(type[1]);
        crc_data.push_back(type[2]);
        crc_data.push_back(type[3]);
        crc_data.insert(crc_data.end(), data.begin(), data.end());

        uint32_t chunk_crc = crc(crc_data.data(), crc_data.size());
        write_big_endian_u32(file_buffer, chunk_crc);
    }

    std::vector<uint8_t> compress_deflate(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> result;

        // Deflate zlib header
        result.push_back(0x78); // CMF
        result.push_back(0x9C); // FLG

        // Create uncompressed deflate blocks
        size_t pos = 0;
        while (pos < data.size()) {
            size_t remaining = data.size() - pos;
            size_t block_size = std::min(size_t(65535), remaining);
            bool is_last = (pos + block_size >= data.size());

            // Block header byte: BFINAL (1 bit) | BTYPE (2 bits = 00 for uncompressed)
            uint8_t bheader = is_last ? 0x01 : 0x00;
            result.push_back(bheader);

            // Block size in little-endian
            uint16_t len = static_cast<uint16_t>(block_size);
            result.push_back(len & 0xff);
            result.push_back((len >> 8) & 0xff);

            // One's complement of block size in little-endian
            uint16_t nlen = ~len;
            result.push_back(nlen & 0xff);
            result.push_back((nlen >> 8) & 0xff);

            // Write block data
            result.insert(result.end(), data.begin() + pos, data.begin() + pos + block_size);

            pos += block_size;
        }

        // Adler-32 checksum
        uint32_t a = 1, b = 0;
        for (size_t i = 0; i < data.size(); i++) {
            a = (a + data[i]) % 65521;
            b = (b + a) % 65521;
        }
        uint32_t adler = (b << 16) | a;

        result.push_back((adler >> 24) & 0xff);
        result.push_back((adler >> 16) & 0xff);
        result.push_back((adler >> 8) & 0xff);
        result.push_back(adler & 0xff);

        return result;
    }

public:
    bool write_png(const std::string& filename, int width, int height, const std::vector<uint8_t>& rgb_data) {
        if (width <= 0 || height <= 0 || rgb_data.empty()) {
            std::cerr << "Error: Invalid image dimensions or data\n";
            return false;
        }

        if (rgb_data.size() < static_cast<size_t>(width * height * 3)) {
            std::cerr << "Error: RGB data size mismatch\n";
            return false;
        }

        init_crc_table();

        std::vector<uint8_t> file_buffer;

        try {
            // PNG signature
            uint8_t png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
            file_buffer.insert(file_buffer.end(), png_signature, png_signature + 8);

            // IHDR chunk
            std::vector<uint8_t> ihdr_data(13);
            ihdr_data[0] = (width >> 24) & 0xff;
            ihdr_data[1] = (width >> 16) & 0xff;
            ihdr_data[2] = (width >> 8) & 0xff;
            ihdr_data[3] = width & 0xff;
            ihdr_data[4] = (height >> 24) & 0xff;
            ihdr_data[5] = (height >> 16) & 0xff;
            ihdr_data[6] = (height >> 8) & 0xff;
            ihdr_data[7] = height & 0xff;
            ihdr_data[8] = 8;  // bit depth
            ihdr_data[9] = 2;  // color type (RGB)
            ihdr_data[10] = 0; // compression method
            ihdr_data[11] = 0; // filter method
            ihdr_data[12] = 0; // interlace method
            write_chunk(file_buffer, "IHDR", ihdr_data);

            // IDAT chunk - image data
            std::vector<uint8_t> raw_data;
            raw_data.reserve(static_cast<size_t>(height) * (1 + width * 3));

            for (int y = 0; y < height; y++) {
                raw_data.push_back(0); // filter type for this scanline
                for (int x = 0; x < width; x++) {
                    size_t idx = (static_cast<size_t>(y) * width + x) * 3;
                    raw_data.push_back(rgb_data[idx]);     // R
                    raw_data.push_back(rgb_data[idx + 1]); // G
                    raw_data.push_back(rgb_data[idx + 2]); // B
                }
            }

            std::clog << "Raw scanline data size: " << raw_data.size() << " bytes\n";

            // Compress data with deflate
            std::vector<uint8_t> compressed = compress_deflate(raw_data);
            std::clog << "Compressed data size: " << compressed.size() << " bytes\n";
            write_chunk(file_buffer, "IDAT", compressed);

            // IEND chunk
            std::vector<uint8_t> empty;
            write_chunk(file_buffer, "IEND", empty);

            // Write to file
            std::ofstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open file for writing: " << filename << "\n";
                return false;
            }

            file.write(reinterpret_cast<const char*>(file_buffer.data()), file_buffer.size());
            file.close();

            std::clog << "PNG file written successfully: " << file_buffer.size() << " bytes\n";
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during PNG write: " << e.what() << "\n";
            return false;
        }
    }
};

#endif
