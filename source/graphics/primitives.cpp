#include "primitives.hpp"
#include "typeDefines.hpp"
#include "glad/glad.h"
#include <glm/glm.hpp>

std::vector<unsigned int> primitive::plane::indices = {};
std::vector<vertex> primitive::plane::vertices = {};

unsigned int primitive::plane::vbo = 0;
unsigned int primitive::plane::ebo = 0;

bool primitive::plane::isBound = false;

void primitive::plane::bind(unsigned int vao)
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		isBound = true;
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

		indices = { 0, 1, 2, 2, 3, 0 };
		vertices = {
			vertex{ p0, nm, { 1.f, 0.f, 0.f }, uv0, { 1.f, 1.f, 1.f } },
			vertex{ p1, nm, { 1.f, 0.f, 0.f }, uv1, { 1.f, 1.f, 1.f } },
			vertex{ p2, nm, { 1.f, 0.f, 0.f }, uv2, { 1.f, 1.f, 1.f } },
			vertex{ p3, nm, { 1.f, 0.f, 0.f }, uv3, { 1.f, 1.f, 1.f } },
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

bool primitive::sphere::isBound = false;

void primitive::sphere::bind(unsigned int vao)
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		isBound = true;
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
                indices.push_back(previousVertex);
                indices.push_back(totalVertexCount - 1);
                indices.push_back(wrapVertex);

                // up triangles
                int vertexCounter = 0;
                int increment = 0;
                for (int j = 0; j < totalVerticesToProcess - 1; j++)
                    {
                        indices.push_back(previousVertex + increment);
                        indices.push_back(currentVertexUp + 0);
                        indices.push_back(currentVertexUp + 1);
                        currentVertexUp++;

                        // if we are on a corner we have 2 triangles under us so generate them
                        if (i != 1)
                            {
                                if (++vertexCounter >= i)
                                    {
                                        indices.push_back(previousVertex + increment);
                                        indices.push_back(currentVertexUp + 0);
                                        indices.push_back(currentVertexUp + 1);

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
                indices.push_back(totalVertexCount - 1);
                indices.push_back(wrapVertex - 1);
                indices.push_back(previousVertex);

                vertexCounter = 1;
                for (int j = 1; j < totalVerticesToProcess - 1; j++)
                    {
                        // if we are not on a corner we have down-triangles associated
                        if ((vertexCounter % i) != 0)
                            {
                                indices.push_back(wrapVertex + vertexCounter);
                                indices.push_back(currentVertexDown + 0);
                                indices.push_back(currentVertexDown + 1);

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
        for (auto &index : mirroredIndices)
            {
                if (static_cast<int>(totalVertexCount - index) > totalVerticesForLastRow)
                    {
                        index += totalVertexCount;
                    }
            }
        indices.insert(indices.end(), mirroredIndices.begin(), mirroredIndices.end());

        // Ensure all points are at a fixed distance away from origin
        for (auto &vert : vertices)
            {
                float modifier = std::sqrt(1.f / (vert.position.x * vert.position.x + vert.position.y * vert.position.y + vert.position.z * vert.position.z));
                vert.position.x *= modifier;
                vert.position.y *= modifier;
                vert.position.z *= modifier;
            }

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}

primitive::sphere::~sphere()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
