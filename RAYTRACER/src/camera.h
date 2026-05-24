#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <omp.h>
#include "hittable.h"
#include "material.h" // <<<<<<<<<<<<<<<<<<<<<<<<<<<<
#include "tile_renderer.h"

class camera {
private:
    /* Private Camera Variables Here */

    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    double pixel_samples_scale;

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    vec3   u, v, w;              // Camera frame basis vectors
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_samples_scale = 1.0 / samples_per_pixel;

        // center = point3(0, 0, 0);
        center = lookfrom; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        // Determine viewport dimensions.
        // auto focal_length = 1.0;
        //auto focal_length = (lookfrom - lookat).length();  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        //auto viewport_height = 2.0;
        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        // auto viewport_height = 2 * h * focal_length;  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        auto viewport_height = 2 * h * focus_dist;   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        //auto viewport_u = vec3(viewport_width, 0, 0);
        //auto viewport_v = vec3(0, -viewport_height, 0);

        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        // auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // auto viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        // choose a random direction to perform recursive ray tracing
        hit_record rec;
        // choose a random direction to perform recursive ray tracing
        /*
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0,0,0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
        */

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);

        return color_from_emission + color_from_scatter;

        //return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        //return (1.0 - a) * color(1.0, 0.0, 0.0) + a * color(0.0, 0.0, 1.0);
    }

    // Trace ray with exposure applied only at sensor level
    color trace_ray(const ray& r, const hittable& world) const {
        return ray_color(r, max_depth, world) * exposure;
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);

        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // auto ray_origin = center;
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    
public:
    /* Public Camera Parameters Here */

    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;
    int    max_depth = 10;   // Maximum number of ray bounces into scene

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    double vfov = 90;  // Vertical view angle (field of view)
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    point3 lookfrom = point3(0, 0, 0);   // Point camera is looking from
    point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    double exposure = 1.0;               // Camera exposure/sensitivity to light (1.0 = normal, < 1.0 = less sensitive, > 1.0 = more sensitive)
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    color  background;               // Scene background color
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


    void render(const hittable& world) {
        initialize();

        // Create a 2D buffer for all pixel data
        std::vector<std::vector<vec3>> pixel_data(image_height, std::vector<vec3>(image_width));

        // Parallel rendering using OpenMP
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += trace_ray(r, world);
                }
                pixel_data[j][i] = pixel_color * pixel_samples_scale;
            }
            
            // Progress update (only from main thread)
            #pragma omp critical
            {
                double progress = double(j + 1) / image_height;
                int bar_width = 40;
                int filled = int(progress * bar_width);
                
                std::string bar = "[";
                for (int k = 0; k < bar_width; k++) {
                    bar += (k < filled) ? "|" : "_";
                }
                bar += "]";
                
                int percent = int(progress * 100);
                std::clog << "\r" << bar << " " << percent << "% (" << (j + 1) << "/" << image_height << ") " << std::flush;
            }
        }

        std::clog << "\r[||||||||||||||||||||||||||||||||||||||||] 100% (" << image_height << "/" << image_height << ")\n";

        // Build output buffer from pixel data
        std::string pixel_buffer;
        pixel_buffer.reserve(image_height * image_width * 12);

        pixel_buffer += "P3\n";
        pixel_buffer += std::to_string(image_width) + " " + std::to_string(image_height) + "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                auto r = pixel_data[j][i].x();
                auto g = pixel_data[j][i].y();
                auto b = pixel_data[j][i].z();

                static const interval intensity(0.000, 0.999);
                int rbyte = int(256 * intensity.clamp(r));
                int gbyte = int(256 * intensity.clamp(g));
                int bbyte = int(256 * intensity.clamp(b));

                pixel_buffer += std::to_string(rbyte) + " " + std::to_string(gbyte) + " " + std::to_string(bbyte) + "\n";
            }
        }

        // Write all buffered data to stdout at once
        std::cout << pixel_buffer;
    }

    // Tiled rendering for improved memory locality and performance
    void render_tiled(const hittable& world, int tile_size = 32) {
        initialize();



        // Create a 2D buffer for all pixel data
        std::vector<std::vector<vec3>> pixel_data(image_height, std::vector<vec3>(image_width));

        // Create tile renderer
        TileRenderer tile_renderer(image_width, image_height, tile_size);
        const auto& tiles = tile_renderer.get_all_tiles();
        int total_tiles = tile_renderer.get_tile_count();

        std::clog << "Tiled Rendering: " << total_tiles << " tiles of size " << tile_size << "x" << tile_size << "\n";

        // Process tiles in parallel with dynamic scheduling
        #pragma omp parallel for schedule(dynamic) 
        for (int tile_idx = 0; tile_idx < total_tiles; tile_idx++) {
            const Tile& tile = tiles[tile_idx];

            // Render pixels within this tile
            for (int j = tile.y_start; j < tile.y_end; j++) {
                for (int i = tile.x_start; i < tile.x_end; i++) {
                    color pixel_color(0, 0, 0);
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += trace_ray(r, world);
                    }
                    pixel_data[j][i] = pixel_color * pixel_samples_scale;
                }
            }

            // Progress update (thread-safe)
            #pragma omp critical
            {
                static int tiles_completed = 0;
                tiles_completed++;
                double progress = double(tiles_completed) / total_tiles;
                int bar_width = 40;
                int filled = int(progress * bar_width);

                std::string bar = "[";
                for (int k = 0; k < bar_width; k++) {
                    bar += (k < filled) ? "|" : "_";
                }
                bar += "]";

                int percent = int(progress * 100);
                std::clog << "\r" << bar << " " << percent << "% (" << tiles_completed << "/" << total_tiles << " tiles) " << std::flush;
            }
        }

        std::clog << "\r[||||||||||||||||||||||||||||||||||||||||] 100% (" << total_tiles << "/" << total_tiles << " tiles)\n";

        // Build output buffer from pixel data
        std::string pixel_buffer;
        pixel_buffer.reserve(image_height * image_width * 12);

        pixel_buffer += "P3\n";
        pixel_buffer += std::to_string(image_width) + " " + std::to_string(image_height) + "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                auto r = pixel_data[j][i].x();
                auto g = pixel_data[j][i].y();
                auto b = pixel_data[j][i].z();

                static const interval intensity(0.000, 0.999);
                int rbyte = int(256 * intensity.clamp(r));
                int gbyte = int(256 * intensity.clamp(g));
                int bbyte = int(256 * intensity.clamp(b));

                pixel_buffer += std::to_string(rbyte) + " " + std::to_string(gbyte) + " " + std::to_string(bbyte) + "\n";
            }
        }

        // Write all buffered data to stdout at once
        std::cout << pixel_buffer;
    }

};

#endif