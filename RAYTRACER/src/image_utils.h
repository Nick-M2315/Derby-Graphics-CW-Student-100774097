#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include "png_converter.h"

#ifdef _WIN32
    #include <windows.h>
#endif

class ImageUtils {
public:
    // Convert PPM file to PNG and return the PNG filename
    static std::string ppm_to_png(const std::string& ppm_file, const std::string& png_file) {
        std::ifstream ppm(ppm_file, std::ios::binary);
        if (!ppm.is_open()) {
            std::cerr << "Error: Cannot open PPM file: " << ppm_file << "\n";
            return "";
        }

        std::string magic;
        ppm >> magic;

        if (magic != "P3") {
            std::cerr << "Error: Invalid PPM file format. Expected P3.\n";
            return "";
        }

        int width, height, max_val;
        ppm >> width >> height >> max_val;

        // Read pixel data
        std::vector<uint8_t> rgb_data(width * height * 3);
        for (int i = 0; i < width * height * 3; i++) {
            int value;
            ppm >> value;
            rgb_data[i] = static_cast<uint8_t>(value);
        }
        ppm.close();

        // Write PNG
        PNGWriter png_writer;
        if (png_writer.write_png(png_file, width, height, rgb_data)) {
            return png_file;
        }

        return "";
    }

    // Open an image file with the default viewer on Windows
    static void open_image(const std::string& image_path) {
#ifdef _WIN32
        // Convert relative path to absolute path
        char full_path[MAX_PATH];
        if (GetFullPathNameA(image_path.c_str(), MAX_PATH, full_path, NULL) == 0) {
            std::cerr << "Error: Could not convert path to absolute: " << image_path << "\n";
            return;
        }

        // Use Windows ShellExecute to open with default viewer
        int result = reinterpret_cast<int>(ShellExecuteA(
            NULL,           // hwnd
            "open",         // operation
            full_path,      // file (absolute path)
            NULL,           // parameters
            NULL,           // directory
            SW_SHOW         // show command
        ));

        if (result <= 32) {
            std::cerr << "Error: Could not open image viewer. Error code: " << result << "\n";
            std::cerr << "Tried to open: " << full_path << "\n";
        } else {
            std::clog << "Opening image with default viewer: " << full_path << "\n";
        }
#else
        // For non-Windows systems, try common image viewers
        std::string command = "xdg-open \"" + image_path + "\"";
        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << "Warning: Could not open image viewer.\n";
        } else {
            std::clog << "Opening image with default viewer...\n";
        }
#endif
    }
};

#endif
