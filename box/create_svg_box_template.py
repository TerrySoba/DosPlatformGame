import svgwrite
import cairosvg
import argparse

paper_sizes = { "A0": (841, 1189), "A1": (594, 841), "A2": (420, 594), "A3": (297, 420), "A4": (210, 297), "A5": (148, 210), "A6": (105, 148) }

def get_center_polygon_offset(polygon, width_mm, height_mm):
    # find bounding box
    min_x = min([point[0] for point in polygon])
    max_x = max([point[0] for point in polygon])
    min_y = min([point[1] for point in polygon])
    max_y = max([point[1] for point in polygon])

    # find center of bounding box
    center_x = (max_x - min_x) / 2 + min_x
    center_y = (max_y - min_y) / 2 + min_y

    # find center of paper
    center_paper_x = width_mm / 2
    center_paper_y = height_mm / 2

    # calculate offset
    offset_x = center_paper_x - center_x
    offset_y = center_paper_y - center_y

    return [offset_x, offset_y] 

def apply_offset(polygon, offset):
    return [(point[0] + offset[0], point[1] + offset[1]) for point in polygon]

def mm_to_px(polygon, dpi=96):
    return [(point[0] * dpi / 25.4, point[1] * dpi / 25.4) for point in polygon]


def get_cardboard_template_polygons(width_mm, height_mm, depth_mm, overlap_mm):
    cardboard_polygons = []

    # Define the polygons for the cardboard box template
    # front_rectangle = ((0, 0), (width_mm, 0), (width_mm, height_mm), (0, height_mm), (0, 0))
    top_flap = ((0,0), (0, -depth_mm), (width_mm, -depth_mm), (width_mm, 0))
    right_flap = ((width_mm, height_mm), (width_mm + depth_mm, height_mm), (width_mm + depth_mm, 0), (width_mm, 0))  
    bottom_flap = ((0, height_mm), (0, height_mm + depth_mm), (width_mm, height_mm + depth_mm), (width_mm, height_mm))  
    left_flap = ((0, 0), (-depth_mm, 0), (-depth_mm, height_mm), (0, height_mm))    
    #cardboard_polygons.append(front_rectangle)
    cardboard_polygons.append(top_flap)
    cardboard_polygons.append(right_flap)
    cardboard_polygons.append(bottom_flap)
    cardboard_polygons.append(left_flap)

    return cardboard_polygons

def get_color_overlay_polygons(width_mm, height_mm, depth_mm, overlap_mm):
    polygons = []
    left_sleeve = ((0, 0),(-overlap_mm-depth_mm, -overlap_mm-depth_mm),(-overlap_mm-depth_mm, height_mm+overlap_mm+depth_mm),(0, height_mm))
    right_sleeve = ((width_mm, 0),(width_mm+overlap_mm+depth_mm, -overlap_mm-depth_mm),(width_mm+overlap_mm+depth_mm, height_mm+overlap_mm+depth_mm),(width_mm, height_mm)) 
    top_sleeve = ((0,0), (0, -overlap_mm-depth_mm), (width_mm, -overlap_mm-depth_mm), (width_mm, 0))
    bottom_sleeve = ((0, height_mm), (0, height_mm + depth_mm + overlap_mm), (width_mm, height_mm + depth_mm + overlap_mm), (width_mm, height_mm))  


    polygons.append(left_sleeve)
    polygons.append(right_sleeve)
    polygons.append(top_sleeve)
    polygons.append(bottom_sleeve)

    return polygons

def create_svg_box_template(
        filename,
        text,
        width_mm,
        height_mm,
        depth_mm,
        overlap_mm,
        paper_size='A3',
        stroke_width='0.5mm'):
    # Create an SVG drawing object
    paper_width = paper_sizes[paper_size][0]
    paper_height = paper_sizes[paper_size][1]
    dwg = svgwrite.Drawing(filename, profile='tiny', size=(f'{paper_width}mm', f'{paper_height}mm'))

    cardboard_polygons = get_cardboard_template_polygons(width_mm, height_mm, depth_mm, overlap_mm)
    color_overlay_polygons = get_color_overlay_polygons(width_mm, height_mm, depth_mm, overlap_mm)

    # join the polygons
    all_polygons = []
    all_polygons += [point for polygon in cardboard_polygons for point in polygon]
    all_polygons += [point for polygon in color_overlay_polygons for point in polygon]

    offset = get_center_polygon_offset(all_polygons, paper_width, paper_height)

    # Create a group element for cardboard template
    cardboard = dwg.g(id='cardboard_template')

    # Add a stroked rectangle to the cardboard template
    cardboard.add(dwg.rect(insert=(f'{offset[0]}mm', f'{offset[1]}mm'), size=(f'{width_mm}mm', f'{height_mm}mm'), fill='none', stroke='black', stroke_width=stroke_width, stroke_dasharray="5,5"))

    for polygon in cardboard_polygons:
        centered_polygon = apply_offset(polygon, offset)
        cardboard.add(dwg.polyline(points=mm_to_px(centered_polygon), fill='none', stroke='black', stroke_width=stroke_width))

    # Add the group to the drawing
    dwg.add(cardboard)

    # Create a group element for the color printed cover
    cover = dwg.g(id='color_overlay')

    for polygon in color_overlay_polygons:
        centered_polygon = apply_offset(polygon, offset)
        cover.add(dwg.polyline(points=mm_to_px(centered_polygon), fill='none', stroke='black', stroke_width=stroke_width))   
   
    # Add the group to the drawing
    dwg.add(cover)

    text_layer = dwg.g(id='text')

    # Add centered text "Outer Box" to the top of the page
    text_layer.add(dwg.text(text, insert=(f'{paper_width / 2}mm', '10mm'), text_anchor="middle", font_size="10mm", fill='black'))

    # Add text containing dimensions of the box to the top of the page
    dimensions_text = f'{width_mm}mm x {height_mm}mm x {depth_mm}mm'
    text_layer.add(dwg.text(dimensions_text, insert=(f'{paper_width / 2}mm', '18mm'), text_anchor="middle", font_size="7mm", fill='black'))


    # Add text to bottom of page telling the user to use svg layers to hide/show the cardboard and color overlay
    text_layer.add(dwg.text('Use SVG layers to hide/show cardboard and color overlay', insert=(f'{paper_width / 2}mm', f'{paper_height - 5}mm'), text_anchor="middle", font_size="5mm", fill='black'))

    # Add the group to the drawing
    dwg.add(text_layer)

    # Save the SVG file
    dwg.save()

box_width_mm = 180
box_height_mm = 230
box_depth_mm = 40
box_cardboard_thickness_mm = 1
box_gap_size_mm = 1
box_overlap_size_mm = 15
stroke_width = '0.25mm'
paper_size = 'A3'


# create_svg_box_template(
#     'generated/box_cardboard_template_outer.svg',
#     'Outer Box Cardboard',
#     box_width_mm,
#     box_height_mm,
#     box_depth_mm,
#     box_overlap_size_mm,
#     paper_size,
#     stroke_width)

# create_svg_box_template(
#     'generated/box_cardboard_template_inner.svg',
#     'Inner Box Cardboard',
#     box_width_mm - box_cardboard_thickness_mm * 2 - box_gap_size_mm * 2,
#     box_height_mm - box_cardboard_thickness_mm * 2 - box_gap_size_mm * 2,
#     box_depth_mm - box_cardboard_thickness_mm,
#     box_overlap_size_mm,
#     paper_size,
#     stroke_width)

# Convert the generated SVG files to PDF
#cairosvg.svg2pdf(url='generated/box_cardboard_template_outer.svg', write_to='generated/box_cardboard_template_outer.pdf')
#cairosvg.svg2pdf(url='generated/box_cardboard_template_inner.svg', write_to='generated/box_cardboard_template_inner.pdf')


def main():
    parser = argparse.ArgumentParser(description='Generate SVG box templates.')
    parser.add_argument('--width', type=int, default=box_width_mm, help='Width of the box in mm')
    parser.add_argument('--height', type=int, default=box_height_mm, help='Height of the box in mm')
    parser.add_argument('--depth', type=int, default=box_depth_mm, help='Depth of the box in mm')
    parser.add_argument('--overlap', type=int, default=box_overlap_size_mm, help='Overlap size in mm')
    parser.add_argument('--thickness', type=int, default=box_cardboard_thickness_mm, help='Cardboard thickness in mm')
    parser.add_argument('--gap', type=int, default=box_gap_size_mm, help='Gap size in mm')
    parser.add_argument('--stroke_width', type=str, default=stroke_width, help='Stroke width for the SVG lines')
    parser.add_argument('--paper_size', type=str, default=paper_size, help='Paper size for the SVG')
    args = parser.parse_args()


    print(f'Creating template with parameters:')
    print(f'  Width: {args.width}mm')
    print(f'  Height: {args.height}mm')
    print(f'  Depth: {args.depth}mm')
    print(f'  Overlap: {args.overlap}mm')
    print(f'  Thickness: {args.thickness}mm')
    print(f'  Gap: {args.gap}mm')
    print(f'  Stroke Width: {args.stroke_width}')
    print(f'  Paper Size: {args.paper_size}')


    create_svg_box_template(
        'generated/box_cardboard_template_outer.svg',
        'Outer Box Cardboard',
        args.width,
        args.height,
        args.depth,
        args.overlap,
        args.paper_size,
        args.stroke_width)

    create_svg_box_template(
        'generated/box_cardboard_template_inner.svg',
        'Inner Box Cardboard',
        args.width - args.thickness * 2 - args.gap * 2,
        args.height - args.thickness * 2 - args.gap * 2,
        args.depth - args.thickness,
        args.overlap,
        args.paper_size,
        args.stroke_width)


    

    # Convert the generated SVG files to PDF
    # cairosvg.svg2pdf(url='generated/box_cardboard_template_outer.svg', write_to='generated/box_cardboard_template_outer.pdf')
    # cairosvg.svg2pdf(url='generated/box_cardboard_template_inner.svg', write_to='generated/box_cardboard_template_inner.pdf')

if __name__ == '__main__':
    main()
