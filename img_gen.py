from PIL import Image, ImageDraw
import numpy as np

def rgb888_to_rgb565(r, g, b):
    return np.uint16(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

def generate_landscape_image(width=160, height=144):
    image = Image.new("RGB", (width, height), "skyblue")
    draw = ImageDraw.Draw(image)
    
    # Draw the sun
    draw.ellipse((120, 10, 150, 40), fill="yellow")
    
    # Draw mountains
    draw.polygon([(0, 100), (50, 50), (100, 100)], fill="gray")
    draw.polygon([(60, 100), (110, 40), (160, 100)], fill="darkgray")
    
    # Draw ground
    draw.rectangle((0, 100, width, height), fill="green")
    
    return image

def convert_image_to_rgb565(image):
    width, height = image.size
    rgb565_array = []
    
    for y in range(height):
        for x in range(width):
            r, g, b = image.getpixel((x, y))
            rgb565_array.append(rgb888_to_rgb565(r, g, b))
    
    return np.array(rgb565_array, dtype=np.uint16)

def write_header_file(filename, array_name, data, width=160, height=144):
    with open(filename, "w") as f:
        f.write("#ifndef STAR_IMAGE_H\n")
        f.write("#define STAR_IMAGE_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define IMAGE_WIDTH {width}\n")
        f.write(f"#define IMAGE_HEIGHT {height}\n\n")
        f.write(f"static const uint16_t {array_name}[{width * height}] = {{\n")
        
        for i in range(0, len(data), 10):
            f.write("    " + ", ".join(f"0x{val:04X}" for val in data[i:i+10]) + ",\n")
        
        f.write("};\n\n")
        f.write("#endif // STAR_IMAGE_H\n")

def main():
    image = generate_landscape_image()
    image.save("star_image.png")
    rgb565_data = convert_image_to_rgb565(image)
    write_header_file("star_image.h", "star_image", rgb565_data)
    print("Header file 'star_image.h' and PNG file 'star_image.png' generated successfully.")

if __name__ == "__main__":
    main()
