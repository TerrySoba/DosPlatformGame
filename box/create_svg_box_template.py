import svgwrite
import cairosvg

paper_sizes = { "A0": (841, 1189), "A1": (594, 841), "A2": (420, 594), "A3": (297, 420), "A4": (210, 297), "A5": (148, 210), "A6": (105, 148) }

A3_WIDTH_MM = 297
A3_HEIGHT_MM = 420

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

def create_svg_box_template(
        filename,
        text,
        width_mm,
        height_mm,
        depth_mm,
        paper_size='A3'):
    # Create an SVG drawing object
    paper_width = paper_sizes[paper_size][0]
    paper_height = paper_sizes[paper_size][1]
    dwg = svgwrite.Drawing(filename, profile='tiny', size=(f'{paper_width}mm', f'{paper_height}mm'))

    # Add a rectangle without fill
    # dwg.add(dwg.rect(insert=(10, 10), size=(f'{width_mm}mm', f'{height_mm}mm'), fill='none', stroke='black', stroke_width="1mm"))

    front_rectangle = ((0, 0), (width_mm, 0), (width_mm, height_mm), (0, height_mm), (0, 0))
    top_flap = ((0,0), (0, -depth_mm), (width_mm, -depth_mm), (width_mm, 0))
    right_flap = ((width_mm, height_mm), (width_mm + depth_mm, height_mm), (width_mm + depth_mm, 0), (width_mm, 0))  
    bottom_flap = ((0, height_mm), (0, height_mm + depth_mm), (width_mm, height_mm + depth_mm), (width_mm, height_mm))  
    left_flap = ((0, 0), (-depth_mm, 0), (-depth_mm, height_mm), (0, height_mm))    

    # join the two polygons
    all_polygons = front_rectangle + top_flap + right_flap + bottom_flap + left_flap

    offset = get_center_polygon_offset(all_polygons, paper_width, paper_height)

    centered_front_rectangle = apply_offset(front_rectangle, offset)
    centered_top_flap = apply_offset(top_flap, offset)
    centered_right_flap = apply_offset(right_flap, offset)
    centered_bottom_flap = apply_offset(bottom_flap, offset)
    centered_left_flap = apply_offset(left_flap, offset)

    dwg.add(dwg.polyline(points=mm_to_px(centered_front_rectangle), fill='none', stroke='black', stroke_width="0.5mm", stroke_dasharray="5,5"))
    dwg.add(dwg.polyline(points=mm_to_px(centered_top_flap), fill='none', stroke='black', stroke_width="0.5mm"))
    dwg.add(dwg.polyline(points=mm_to_px(centered_right_flap), fill='none', stroke='black', stroke_width="0.5mm"))
    dwg.add(dwg.polyline(points=mm_to_px(centered_bottom_flap), fill='none', stroke='black', stroke_width="0.5mm"))
    dwg.add(dwg.polyline(points=mm_to_px(centered_left_flap), fill='none', stroke='black', stroke_width="0.5mm"))

    # Add centered text "Outer Box"
    dwg.add(dwg.text(text, insert=(f'{paper_width / 2}mm', f'{paper_height / 2}mm'), text_anchor="middle", font_size="10mm", fill='black'))

    # Add text containing dimensions of the box
    dimensions_text = f'{width_mm}mm x {height_mm}mm x {depth_mm}mm'
    dwg.add(dwg.text(dimensions_text, insert=(f'{paper_width / 2}mm', f'{paper_height / 2 + 15}mm'), text_anchor="middle", font_size="7mm", fill='black'))

    # Save the SVG file
    dwg.save()

box_width_mm = 180
box_height_mm = 230
box_depth_mm = 40
box_cardboard_thickness_mm = 1
box_gap_size_mm = 1
paper_size = 'A3'


create_svg_box_template(
    'generated/box_cardboard_template_outer.svg',
    'Outer Box Cardboard',
    box_width_mm,
    box_height_mm,
    box_depth_mm,
    paper_size)

create_svg_box_template(
    'generated/box_cardboard_template_inner.svg',
    'Inner Box Cardboard',
    box_width_mm - box_cardboard_thickness_mm * 2 - box_gap_size_mm * 2,
    box_height_mm - box_cardboard_thickness_mm * 2 - box_gap_size_mm * 2,
    box_depth_mm - box_cardboard_thickness_mm,
    paper_size)

# Convert the generated SVG files to PDF
cairosvg.svg2pdf(url='generated/box_cardboard_template_outer.svg', write_to='generated/box_cardboard_template_outer.pdf')
cairosvg.svg2pdf(url='generated/box_cardboard_template_inner.svg', write_to='generated/box_cardboard_template_inner.pdf')
