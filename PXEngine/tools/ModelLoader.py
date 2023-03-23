def listEqual(a,b):
    for i in range(len(a)):
        if a[i] != b[i]:
            return False
    return True


def processVertices(vertices):
    processed_vertices = []
    processed_indices = []
    for vertex in vertices:
        duplicate_vertex = False
        for i in range(len(processed_vertices)):
            if listEqual(processed_vertices[i], vertex):
                duplicate_vertex = True
                processed_indices.append(i)
        if not duplicate_vertex:
            processed_indices.append(len(processed_vertices))
            processed_vertices.append(vertex)
    merged_vertices = []
    for vertex in processed_vertices:
        merged_vertices.extend(vertex)
    print(f"""Vertex Values: {merged_vertices}
Index Values: {processed_indices}
Vertex Count: {len(processed_vertices)}
Vertex Value Count: {len(merged_vertices)}
Index Count: {len(processed_indices)}""")


def loadFromCSV(filename):
    vertices = []
    f = open(filename)
    for line in f:
        vertex = []
        for value in line.split(","):
            stripped_value = value.strip().replace("f", "")
            if stripped_value:
                vertex.append(float(stripped_value))
        if len(vertex):
            vertices.append(vertex)
    return vertices


def loadFromObj(filename):
    f = open(filename)
    vertex_values = []
    uv_values = []
    vertices = []
    for line in f:
        if line.startswith("v "):
            split_values = line.split(" ")
            vertex_values.append([float(split_values[1]), float(split_values[2]), float(split_values[3])])
        elif line.startswith("vt "):
            split_values = line.split(" ")
            uv_values.append([float(split_values[1]), float(split_values[2])])
        elif line.startswith("f "):
            split_values = line.split(" ")
            for face in split_values[1:]:
                face_indices = face.split("/")
                vertex = []
                vertex.extend(vertex_values[int(face_indices[0]) - 1])
                vertex.extend(uv_values[int(face_indices[1]) - 1])
                vertices.append(vertex)
    return vertices


# processVertices(loadFromCSV("vert.csv"))
processVertices(loadFromObj("untitled.obj"))