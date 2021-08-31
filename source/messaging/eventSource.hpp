// eventSource.hpp
// A base class for any class which can have events sent from
#pragma once
#include <robin_hood.h>
#include <functional>
#include <vector>
#include "typeDefines.hpp"
#include "message.hpp"

class eventSource
	{
		public:
			using handler = std::function<void(message&)>;

		private:
			struct observer
				{
					handler callback;
					int id = 0;
				};

			robin_hood::unordered_map<fe::str, std::vector<observer>> m_observers;
			robin_hood::unordered_map<fe::str, int> m_ids;

		protected:
			void signal(message event) const;

		public:
			int subscribe(fe::str event, handler callback);
			void unsubscribe(fe::str event, int id);

	};
