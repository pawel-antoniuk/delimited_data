#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

namespace client {
	namespace detail {
		template<typename Type>
		struct delimited_data_basic {
			operator Type() const
			{
				return _output;
			}

			Type& data() {
				return _output;
			}

		private:
			Type _output;
		};
	}

	template<typename Type, char delim = ' '>
	struct delimited_data : detail::delimited_data_basic<Type>
	{
		friend std::istream& operator>>(std::istream& in,
			delimited_data<Type, delim>& data)
		{
			std::string buf;
			std::getline(in, buf, delim);
			std::istringstream ss{ std::move(buf) };
			ss >> data.data();
			return in;
		}
	};

	template<typename Type, char... delims>
	struct multiple_delimited_data : detail::delimited_data_basic<Type>
	{
		friend std::istream& operator>>(std::istream& in,
			multiple_delimited_data<Type, delims...>& data)
		{
			auto delims_ = { delims... };
			std::stringstream ss;
			while (true) {
				auto c = in.get();
				auto it = std::find(delims_.begin(), delims_.end(), c);
				if (it != delims_.end() || c == EOF) break;
				else ss << static_cast<char>(c);
			}
			ss >> data.data();
			const auto eofbit = std::istream::eofbit;
			in.setstate(ss.rdstate() & ~eofbit | in.rdstate() & eofbit);
			return in;
		}
	};

	template<typename typename Type, char... delims>
	using lined_multiple_delimited_data = multiple_delimited_data<Type, '\n', delims...>;
}

int main()
{
	using namespace std;
	using namespace client;

	vector<int> v;
	copy(istream_iterator<lined_multiple_delimited_data<int, ','>>(cin),
		istream_iterator<lined_multiple_delimited_data<int, ','>>(),
		ostream_iterator<int>(cout, ", "));
	//for (auto n : v) cout << n;
}