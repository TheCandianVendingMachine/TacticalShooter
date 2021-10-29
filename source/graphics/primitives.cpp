#include "primitives.hpp"
#include "typeDefines.hpp"
#include "glad/glad.h"
#include <glm/glm.hpp>

std::vector<unsigned int> primitive::plane::indices = {};
std::vector<vertex> primitive::plane::vertices = {};

unsigned int primitive::plane::vbo = 0;
unsigned int primitive::plane::ebo = 0;

void primitive::plane::bind(unsigned int vao)
    {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

primitive::plane::plane()
    {
        glm::vec3 p0(-1.f, 0.f, -1.f);
        glm::vec3 p1( 1.f, 0.f, -1.f);
        glm::vec3 p2( 1.f, 0.f,  1.f);
        glm::vec3 p3(-1.f, 0.f,  1.f);

        glm::vec2 uv0(0.f, 0.f);
        glm::vec2 uv1(0.f, 1.f);
        glm::vec2 uv2(1.f, 1.f);
        glm::vec2 uv3(1.f, 0.f);

        glm::vec3 nm(0.f, 1.f, 0.f);

        indices = { 2, 3, 0, 0, 1, 2 };
        vertices = {
            vertex{ p0, nm, { 0.f, 0.f, 1.f }, uv0, { 1.f, 1.f, 1.f } },
            vertex{ p1, nm, { 0.f, 0.f, 1.f }, uv1, { 1.f, 1.f, 1.f } },
            vertex{ p2, nm, { 0.f, 0.f, 1.f }, uv2, { 1.f, 1.f, 1.f } },
            vertex{ p3, nm, { 0.f, 0.f, 1.f }, uv3, { 1.f, 1.f, 1.f } },
        };

        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }

primitive::plane::~plane()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

std::vector<unsigned int> primitive::sphere::indices = {};
std::vector<vertex> primitive::sphere::vertices = {};

unsigned int primitive::sphere::vbo = 0;
unsigned int primitive::sphere::ebo = 0;

void primitive::sphere::bind(unsigned int vao)
    {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

void primitive::sphere::generateVertices() const
    {
        constexpr float positionOffset = 1.f / static_cast<float>(c_resolution);

        vertex v;
        v.position = { 0.f, -1.f, 0.f };
        v.normal = { 0.f, -1.f, 0.f };
        v.textureCoordinate.x = std::asin(v.normal.x) / fe::PI + 0.5f;
        v.textureCoordinate.y = std::asin(v.normal.y) / fe::PI + 0.5f;
        v.colour = (v.normal + glm::vec3(1.f)) * 0.5f;
        vertices.push_back(v);

        // the direction which we offset for each vertex
        constexpr int windDirections[] = {
            0, 1,
            1, 0,
            0, -1,
            -1, 0
        };

        for (int row = 1; row <= c_resolution; row++)
            {
                v.position.y += positionOffset;

                v.position.z -= positionOffset;
                v.position.x -= positionOffset;

                int vertexCounter = 0;
                int index = 2;
                int xDirection = 0;
                int zDirection = 1;
                for (int j = 0; j < row * 4; j++)
                    {
                        v.normal = glm::normalize(v.position);
                        v.colour = (v.normal + glm::vec3(1.f)) * 0.5f;

                        v.textureCoordinate.x = std::asin(v.normal.x) / fe::PI + 0.5f;
                        v.textureCoordinate.y = std::asin(v.normal.y) / fe::PI + 0.5f;

                        vertices.push_back(v);
                        v.position.x += 2.f * positionOffset * xDirection;
                        v.position.z += 2.f * positionOffset * zDirection;

                        if (++vertexCounter == row)
                            {
                                xDirection = windDirections[index + 0];
                                zDirection = windDirections[index + 1];
                                index += 2;

                                vertexCounter = 0;
                            }
                    }
            }
    }

void primitive::sphere::generateIndices() const
    {
        auto createTriangle = [this] (unsigned int i0, unsigned int i1, unsigned int i2) {
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            // generate vertex tangents
            vertex &v1 = vertices[i0];
            vertex &v2 = vertices[i1];
            vertex &v3 = vertices[i2];

            glm::vec3 edge1 = v2.position - v1.position;
            glm::vec3 edge2 = v3.position - v1.position;
            glm::vec2 dUV1 = v2.textureCoordinate - v1.textureCoordinate;
            glm::vec2 dUV2 = v3.textureCoordinate - v1.textureCoordinate;

            float f = 1.f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

            v1.tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
            v1.tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
            v1.tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
        };

        int currentVertexUp = 1;
        int currentVertexDown = 1;
        for (int i = 1; i <= c_resolution; i++)
            {
                // v(i) = 2r(i + 1) + 1
                const int totalVertexCount = (2 * i) * (i + 1) + 1;
                // = v(i) - v(i - 1)
                const int totalVerticesToProcess = (4 * i);
                // = v(i - 1)
                const int wrapVertex = (2 * i) * (i - 1) + 1;
                // = v(i - 2)
                const int previousVertex = (i > 1) ? ((2 * i) * (i - 3) + 5) : 0;

                // add first index to list since it is always the non pattern conforming
                createTriangle(wrapVertex, totalVertexCount - 1, previousVertex);

                // up triangles
                int vertexCounter = 0;
                int increment = 0;
                for (int j = 0; j < totalVerticesToProcess - 1; j++)
                    {
                        createTriangle(currentVertexUp + 1, currentVertexUp, previousVertex + increment);
                        currentVertexUp++;

                        // if we are on a corner we have 2 triangles under us so generate them
                        if (i != 1)
                            {
                                if (++vertexCounter >= i)
                                    {
                                        createTriangle(currentVertexUp + 1, currentVertexUp, previousVertex + increment);

                                        // We are adding another vertex so we want to increment all values pertaining to the +1 vertex
                                        // vertexCounter = 1 since this is a processed vertex
                                        j++;
                                        currentVertexUp++;
                                        vertexCounter = 1;
                                    }
                                increment++;
                            }
                    }
                currentVertexUp++;

                // down triangles
                if (i < 2) { continue; }
                createTriangle(totalVertexCount - 1, wrapVertex - 1, previousVertex);

                vertexCounter = 1;
                for (int j = 1; j < totalVerticesToProcess - 1; j++)
                    {
                        // if we are not on a corner we have down-triangles associated
                        if ((vertexCounter % i) != 0)
                            {
                                createTriangle(wrapVertex + vertexCounter, currentVertexDown, currentVertexDown + 1);

                                // We are adding another vertex so we want to increment all values pertaining to the +1 vertex
                                // vertexCounter = 1 since this is a processed vertex
                                currentVertexDown++;
                            }
                        vertexCounter++;
                    }
                currentVertexDown++;
            }
    }

primitive::sphere::sphere()
    {
        // A procedural sphere shape with alright control over vertex count
        // The sphere is created by first creating an octahedron through triangles subdivided along the edges as vertices
        // The vertices are then transformed to be distance 1 away from the centre

        constexpr int totalVerticesForLastRow = c_resolution * 4;
        constexpr int totalVertexCount = (2 * c_resolution) * (c_resolution + 1) + 1;

        // we generate half the sphere with this function and then mirror the other half
        generateVertices();
        generateIndices();

        std::vector<vertex> mirroredVertices(vertices.begin(), vertices.end() - totalVerticesForLastRow);
        for (auto &vertex : mirroredVertices)
            {
                vertex.position.y = -vertex.position.y;
                vertex.normal.y = -vertex.normal.y;

                vertex.textureCoordinate.y = std::asin(vertex.normal.y) / fe::PI + 0.5f;
            }
        vertices.insert(vertices.end(), mirroredVertices.begin(), mirroredVertices.end());

        std::vector<unsigned int> mirroredIndices(indices);
        for (unsigned int i = 0; i < mirroredIndices.size(); i += 3)
            {
                int tempIndex = mirroredIndices[i];
                mirroredIndices[i] = mirroredIndices[i + 2];
                mirroredIndices[i + 2] = tempIndex;
                for (unsigned int j = 0; j < 3; j++)
                    {
                        unsigned int index = mirroredIndices[i + j];
                        if (static_cast<int>(totalVertexCount - index) > totalVerticesForLastRow)
                            {
                                 mirroredIndices[i + j] += totalVertexCount;
                            }
                    }
            }
        indices.insert(indices.end(), mirroredIndices.begin(), mirroredIndices.end());

        // Ensure all points are at a fixed distance away from origin
        for (auto &vert : vertices)
            {
                float modifier = std::sqrt(1.f / (vert.position.x * vert.position.x + vert.position.y * vert.position.y + vert.position.z * vert.position.z));
                vert.position *= modifier;
            }

        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }

primitive::sphere::~sphere()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
