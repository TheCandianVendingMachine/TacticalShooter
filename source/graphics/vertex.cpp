#include "vertex.hpp"
#include <type_traits>

vertex::attributes operator|(const vertex::attributes &lhs, const vertex::attributes &rhs)
	{
		using ul = std::underlying_type<vertex::attributes>::type;
		return static_cast<vertex::attributes>(static_cast<const ul>(lhs) & static_cast<const ul>(rhs));
	}

vertex::attributes operator&(const vertex::attributes &lhs, const vertex::attributes &rhs)
	{
		using ul = std::underlying_type<vertex::attributes>::type;
		return static_cast<vertex::attributes>(static_cast<const ul>(lhs) & static_cast<const ul>(rhs));
	}
