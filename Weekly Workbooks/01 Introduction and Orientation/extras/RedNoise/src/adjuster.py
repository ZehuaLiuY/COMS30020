import numpy as np
box_path = '../src/files/cornell-box.obj'
logo_path = '../src/files/logo.obj'
sphere_path = '../src/files/sphere.obj'

def read_obj_file(file_path):
    with open(file_path, 'r') as file:
        return file.readlines()

box_content = read_obj_file(box_path)
logo_content = read_obj_file(logo_path)
sphere_content = read_obj_file(sphere_path)

box_content[:10], logo_content[:10], sphere_content[:10]

# Function to extract vertices from the OBJ file content
def extract_vertices(obj_content):
    vertices = []
    for line in obj_content:
        if line.startswith('v '):
            parts = line.split()
            vertices.append([float(parts[1]), float(parts[2]), float(parts[3])])
    return np.array(vertices)

def calculate_centroid(vertices):
    return np.mean(vertices, axis=0)

logo_vertices = extract_vertices(logo_content)
sphere_vertices = extract_vertices(sphere_content)

logo_centroid = calculate_centroid(logo_vertices)
sphere_centroid = calculate_centroid(sphere_vertices)

logo_centroid, sphere_centroid

def translate_to_origin(vertices, centroid):

    return vertices - centroid

logo_vertices_centered = translate_to_origin(logo_vertices, logo_centroid)
sphere_vertices_centered = translate_to_origin(sphere_vertices, sphere_centroid)

def update_obj_content(obj_content, original_vertices, new_vertices):

    vertex_mapping = {tuple(ov): tuple(nv) for ov, nv in zip(original_vertices, new_vertices)}
    new_content = []
    for line in obj_content:
        if line.startswith('v '):
            parts = line.split()
            original_vertex = (float(parts[1]), float(parts[2]), float(parts[3]))
            new_vertex = vertex_mapping[original_vertex]
            new_line = 'v {:.6f} {:.6f} {:.6f}\n'.format(*new_vertex)
            new_content.append(new_line)
        else:
            new_content.append(line)
    return new_content

# Update the OBJ contents with the new, centered vertex positions
logo_content_centered = update_obj_content(logo_content, logo_vertices, logo_vertices_centered)
sphere_content_centered = update_obj_content(sphere_content, sphere_vertices, sphere_vertices_centered)

# Save the new OBJ contents to new files
def save_obj_file(file_path, obj_content):
    with open(file_path, 'w') as file:
        file.writelines(obj_content)


# Function to calculate the bounding box of an object based on its vertex indices
def calculate_bounding_box(vertices, vertex_indices):
    object_vertices = [vertices[i] for i in vertex_indices]
    min_x, min_y, min_z = float('inf'), float('inf'), float('inf')
    max_x, max_y, max_z = float('-inf'), float('-inf'), float('-inf')

    for vertex in object_vertices:
        x, y, z = vertex
        min_x, max_x = min(min_x, x), max(max_x, x)
        min_y, max_y = min(min_y, y), max(max_y, y)
        min_z, max_z = min(min_z, z), max(max_z, z)

    return (min_x, max_x), (min_y, max_y), (min_z, max_z)

# # Define new file paths for the updated OBJ files
# new_logo_path = '../src/files/logo_centered.obj'
# new_sphere_path = '../src/files/sphere_centered.obj'
#
# # Save the updated contents to new OBJ files
# save_obj_file(new_logo_path, logo_content_centered)
# save_obj_file(new_sphere_path, sphere_content_centered)
#
# new_logo_path, new_sphere_path


box_content = read_obj_file(box_path)
logo_content = read_obj_file(logo_path)
sphere_content = read_obj_file(sphere_path)

logo_vertices = extract_vertices(logo_content)
sphere_vertices = extract_vertices(sphere_content)

logo_centroid = calculate_centroid(logo_vertices)
sphere_centroid = calculate_centroid(sphere_vertices)

logo_translation = [2.78098867, 2.74976533, -2.7899683]
sphere_translation = [-1.17901103, -3.142686, 1.2899683]

logo_vertices_translated = logo_vertices + np.array(logo_translation)
sphere_vertices_translated = sphere_vertices + np.array(sphere_translation)

logo_content_updated = update_obj_content(logo_content, logo_vertices, logo_vertices_translated)
sphere_content_updated = update_obj_content(sphere_content, sphere_vertices, sphere_vertices_translated)

new_logo_path = '../src/files/logo_updated.obj'
new_sphere_path = '../src/files/sphere_updated.obj'

save_obj_file(new_logo_path, logo_content_updated)
save_obj_file(new_sphere_path, sphere_content_updated)

new_logo_path, new_sphere_path