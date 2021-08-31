// primitives
// A set of generators which generate common primitive meshes
#pragma once
#include <vector>
#include "vertex.hpp"
#include "vertexArray.hpp"
#include <spdlog/spdlog.h>

namespace primitive
	{
		template<typename T>
		class generator
			{
				private:
					generator() = default;
					friend class plane;
					friend class sphere;

					[[nodiscard]] static vertexArray generate(vertex::attributes attributes = vertex::attributes::NONE)
						{
							vertexArray vao;

							if (T::ebo == 0 || T::vbo == 0)
								{
									spdlog::error("Cannot generate primitive without a bound VBO/EBO [{}/{}]", T::vbo, T::ebo);
								}

							if (!T::isBound)
								{
									T::bind(vao.vao);
								}

							vao.bindIndices(T::ebo, T::indices.size());
							vao.bindVertices(T::vbo, T::vertices.size());

							vao.use(attributes);

							return vao;
						}
			};

		class plane : public generator<plane>
			{
				private:
					friend class generator<plane>;

					static unsigned int vbo;
					static unsigned int ebo;

					static bool isBound;

					static std::vector<unsigned int> indices;
					static std::vector<vertex> vertices;

					static void bind(unsigned int vao);

				public:
					plane();
					~plane();

					using generator<plane>::generate;
			};

		class sphere : public generator<sphere>
			{
				private:
					friend class generator<sphere>;

					static constexpr int c_resolution = 8;

					static unsigned int vbo;
					static unsigned int ebo;

					static bool isBound;

					static std::vector<unsigned int> indices;
					static std::vector<vertex> vertices;

					static void bind(unsigned int vao);

					// cleans up constructor to be read easier
					void generateVertices() const;
					void generateIndices() const;

				public:
					sphere();
					~sphere();

					using generator<sphere>::generate;
			};
	}