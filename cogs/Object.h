#ifndef OBJECT_H
#define OBJECT_H

#include <string>

namespace cogs
{
		class Object
		{
		public:
				Object() : m_name("default"), m_destroyed(false) {}
				Object(const std::string& _name) : m_name(_name), m_destroyed(false) {}

				void setName(const std::string& _name)						{ m_name = _name; }
				const std::string& getName() const noexcept { return m_name; }

				void destroy() { m_destroyed = true; }

				virtual ~Object()
				{

				}

		protected:
				std::string m_name{ "default" };
				bool m_destroyed{ false };
		};
}
#endif // !OBJECT_H