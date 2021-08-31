// message.hpp
// A message sent. Contains up to 8 arguments
#pragma once
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "typeDefines.hpp"

struct message
	{
		struct argument
			{
				union
					{
						int INT;
						float FLOAT;
						double DOUBLE;
						bool BOOL;
						glm::vec2 VEC2;
						glm::vec3 VEC3;
						void *USER_DATA;
					} variable;

				// any metadata about the variable: used, for example, for a string's length in USER_DATA
				void *metaData = nullptr;

				enum class type : unsigned char
					{
						NONE,
						INT,
						FLOAT,
						DOUBLE,
						BOOL,
						VEC2,
						VEC3,
						USER_DATA
					} variableType = type::NONE;

				argument() { variable.INT = 0; };
				argument(int arg)					{ variable.INT = arg;		variableType = type::INT; }
				argument(float arg)					{ variable.FLOAT = arg;		variableType = type::FLOAT; }
				argument(double arg)				{ variable.DOUBLE = arg;	variableType = type::DOUBLE; }
				argument(bool arg)					{ variable.BOOL = arg;		variableType = type::BOOL; }
				argument(glm::vec2 arg)				{ variable.VEC2 = arg;		variableType = type::VEC2; }
				argument(glm::vec3 arg)				{ variable.VEC3 = arg;		variableType = type::VEC3; }
				argument(void *arg)					{ variable.USER_DATA = arg;	variableType = type::USER_DATA; }
				argument(void *arg, void *metaData) { variable.USER_DATA = arg;	variableType = type::USER_DATA; this->metaData = metaData; }

				bool cast(int &in)
					{
						if (variableType == type::INT)
							{
								in = variable.INT;
								return true;
							}
						return false;
					}

				bool cast(float &in)
					{
						if (variableType == type::FLOAT)
							{
								in = variable.FLOAT;
								return true;
							}
						return false;
					}

				bool cast(double &in)
					{
						if (variableType == type::DOUBLE)
							{
								in = variable.DOUBLE;
								return true;
							}
						return false;
					}

				bool cast(bool &in)
					{
						if (variableType == type::BOOL)
							{
								in = variable.BOOL;
								return true;
							}
						return false;
					}

				bool cast(glm::vec2 &in)
					{
						if (variableType == type::VEC2)
							{
								in = variable.VEC2;
								return true;
							}
						return false;
					}

				bool cast(glm::vec3 &in)
					{
						if (variableType == type::VEC3)
							{
								in = variable.VEC3;
								return true;
							}
						return false;
					}

				bool cast(void *&in)
					{
						if (variableType == type::USER_DATA)
							{
								in = variable.USER_DATA;
								return true;
							}
						return false;
					}
			};

		std::array<argument, 8> arguments;
		unsigned int argumentCount = 0;

		fe::str event = 0;

		message() = default;
		message(fe::str event) : event(event) {}
		message(fe::str event, argument arg0) : event(event), argumentCount(1), arguments{ arg0 } {}
		message(fe::str event, argument arg0, argument arg1) : event(event), argumentCount(2), arguments{ arg0, arg1 } {}
		message(fe::str event, argument arg0, argument arg1, argument arg2) : event(event), argumentCount(3), arguments{ arg0, arg1, arg2 } {}
		message(fe::str event, argument arg0, argument arg1, argument arg2, argument arg3) : event(event), argumentCount(4), arguments{ arg0, arg1, arg2, arg3 } {}
		message(fe::str event, argument arg0, argument arg1, argument arg2, argument arg3, argument arg4) : event(event), argumentCount(5), arguments{ arg0, arg1, arg2, arg3, arg4 } {}
		message(fe::str event, argument arg0, argument arg1, argument arg2, argument arg3, argument arg4, argument arg5) : event(event), argumentCount(6), arguments{ arg0, arg1, arg2, arg3, arg4, arg5 } {}
		message(fe::str event, argument arg0, argument arg1, argument arg2, argument arg3, argument arg4, argument arg5, argument arg6) : event(event), argumentCount(7), arguments{ arg0, arg1, arg2, arg3, arg4, arg5, arg6 } {}
		message(fe::str event, argument arg0, argument arg1, argument arg2, argument arg3, argument arg4, argument arg5, argument arg6, argument arg7) : event(event), argumentCount(8), arguments{ arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7 } {}
	};

