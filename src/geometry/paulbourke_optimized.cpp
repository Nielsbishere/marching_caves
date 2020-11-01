#include "geometry/paulbourke_optimized.hpp"
#include "geometry/paulbourke_polygonise.hpp"
#include <system/log.hpp>

namespace irmc {

	String compressPaulbourke(bool isCpp) {

		//Edge table will be transformed into
		//uint64_t[64], where each edge occupies 12 bits (3 nibbles)
		//First edges are located at 0-0xFFF
		//aka ([lut >> 2] >> (12 * (i & 3))) & 0xFFF

		struct Stringify {

			static String exec(u16 i) {
				return 
					String(1, oic::Log::hexChars[i >> 8]) + 
					oic::Log::hexChars[(i >> 4) & 0xF] + 
					oic::Log::hexChars[i & 0xF];
			}

			static inline c8 looky(u16 i) {
				return oic::Log::hexChars[i];
			}

			static String exec(const u8 (&i)[15]) {

				String res;

				for (u8 j = 0; j < 15; j += 3) {

					res = 
						String(1, looky(i[j])) + 
						looky(i[j + 1]) + 
						looky(i[j + 2]) + 
						res;

				}

				return res;
			}

		};

		String prefix = isCpp ? "static constexpr u64" : "const uint64_t";

		String mcEdgeLut = prefix + " mcEdgeTable[64] = {\n";

		for (usz i = 0; i < 64; ++i) {

			if ((i & 3) == 0)
				mcEdgeLut += '\t';

			String v =
				"0x" +
				Stringify::exec(paulbourke::edgeTable[i * 4 + 3]) +
				Stringify::exec(paulbourke::edgeTable[i * 4 + 2]) +
				Stringify::exec(paulbourke::edgeTable[i * 4 + 1]) +
				Stringify::exec(paulbourke::edgeTable[i * 4]) + (isCpp ? "" : "ul");

			mcEdgeLut += v + ((i & 3) == 3 ? ",\n" : ", ");
		}

		mcEdgeLut += "};";

		String mcTriLut = prefix + " mcTriTable[256] = {\n";

		for (usz i = 0; i < 256; ++i) {

			if ((i & 3) == 0)
				mcTriLut += '\t';

			mcTriLut += 
				"0x" + Stringify::exec(paulbourke::triTable[i]) + (isCpp ? "" : "ul") +
				((i & 3) == 3 ? ",\n" : ", ");
		}

		mcTriLut += "};";

		return "\n" + mcEdgeLut + "\n\n" + mcTriLut;
	}

}