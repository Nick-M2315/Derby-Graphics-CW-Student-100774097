// main.cpp


#include "rtweekend.h"
#include "bvh.h"
#include "camera.h"
#include "constant_medium.h" // <<<<<<<<<<<<<<<<<<<<<<<<
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "texture.h" 
#include "quad.h"  
#include "png_converter.h"
#include "image_utils.h"
#include <fstream>
#include <filesystem>

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void final_scene(int image_width, int samples_per_pixel, int max_depth) {

    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    //auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    //auto green = make_shared<lambertian>(color(.12, .45, .15));
    //auto light = make_shared<diffuse_light>(color(15, 15, 15));
    //auto light = make_shared<diffuse_light>(color(30, 30, 30));
    auto light = make_shared<diffuse_light>(color(80, 20, 80)); //Purple Light at the top
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));
    // Add a second light source
    auto light2 = make_shared<diffuse_light>(color(200, 2, 2));  // Red light
    world.add(make_shared<quad>(point3(400, 300, 200), vec3(100, 0, 0), vec3(0, 100, 0), light2));
    // Add a third light source
    auto light3 = make_shared<diffuse_light>(color(2, 2, 200));  // Blue light
    world.add(make_shared<quad>(point3(-20, 100, 20), vec3(100, 0, 0), vec3(0, 100, 0), light3));
    // Add a fourth light source
    auto light4 = make_shared<diffuse_light>(color(250, 250, 170));  // Yellow Light
    world.add(make_shared<quad>(point3(-500, 300, 800), vec3(200, 0, 0), vec3(0, 200, 0), light4));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    //world.add(make_shared<sphere>(center1, center2, 50, sphere_material));
    world.add(make_shared<sphere>(center1, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    /*
    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));
    */

    auto checker = make_shared<checker_texture>(24, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(300, 250, 400), 100, make_shared<lambertian>(checker)));

    auto fog = make_shared<sphere>(point3(220, 280, 300), 80, white);
    world.add(make_shared<constant_medium>(fog, 0.004, color(1, 1, 1)));

    hittable_list boxes2;
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    world.add(make_shared<translate>(make_shared<rotate_y>(make_shared<bvh_node>(boxes2), 15), vec3(-100, 270, 395)));


    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    // Set camera exposure
    cam.exposure = 0.2;  // 1.0 = normal, < 1.0 = less sensitive (darker), > 1.0 = more sensitive (brighter)

    cam.render(world);
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int main() {
    // Create output directory structure
    std::filesystem::path output_dir = "out/Render Outputs";
    std::filesystem::create_directories(output_dir);

    // Redirect std::cout to file
    std::string ppm_file = output_dir.string() + "/render_output.ppm";
    std::string png_file = output_dir.string() + "/render_output.png";
    std::ofstream file_stream(ppm_file);
    std::cout.rdbuf(file_stream.rdbuf());

    std::clog << "Starting render... Output will be saved to " << ppm_file << "\n";

    final_scene(200, 250, 4); // ~2 min
    //final_scene(400, 250, 40); // ~4 min
    //final_scene(400, 1000, 40);  // ~15 min
    //final_scene(400, 4000, 40);  // ~15 min
    //final_scene(800, 10000, 40);

    std::clog << "\nRender complete! Image saved to " << ppm_file << "\n";

    // Close the file stream before converting
    file_stream.close();

    // Convert PPM to PNG
    std::clog << "Converting PPM to PNG...\n";
    std::string converted_png = ImageUtils::ppm_to_png(ppm_file, png_file);

    if (!converted_png.empty()) {
        std::clog << "PNG saved to " << png_file << "\n";

        // Open the image with default viewer
        ImageUtils::open_image(png_file);
    } else {
        std::cerr << "Error: Failed to convert PPM to PNG\n";
    }

    return 0;
}