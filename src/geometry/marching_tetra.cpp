#include "geometry/marching_tetra.hpp"
#include "geometry/tetron.hpp"
#include <system/log.hpp>
using namespace irmc;

u32 registerScalar(List<f32> &indices, f32 v) {

	auto indexIt = std::find(indices.begin(), indices.end(), v);

	if (indexIt == indices.end()) {

		if (indices.size() == 32)
			throw std::runtime_error("Out of bits");

		indices.push_back(v);
		return u32(indices.size() - 1);
	}

	return u32(indexIt - indices.begin());
}

u16 registerPos(
	const Vec3f32 &pos,
	List<f32> &indices
) {
	return 
		u16(registerScalar(indices, pos.x)) | 
		u16(registerScalar(indices, pos.y) << 5) | 
		u16(registerScalar(indices, pos.z) << 10);
}

Vec4u16 registerTetron(List<f32> &indices, const Tetron &tetron) {

	Vec4u16 index;

	for (u8 i = 0; i < 4; ++i)
		index[i] = registerPos(tetron[i], indices);

	return index;
}

inline void subdiv(
	List<f32> &indices,
	List<Vec4u16> &tetra,
	const Tetron &tetron,
	usz i,
	usz recursion,
	usz j,
	usz offset
) {
	if (i + 1 < recursion) {

		auto split = tetron.splitTetra();

		usz start = (1_usz << i) - 1;
		usz toStart = j - start;

		usz left = (toStart << 1) + (1_usz << (i + 1)) - 1;

		tetra[offset + left] = registerTetron(indices, split[0]);
		tetra[offset + left + 1] = registerTetron(indices, split[1]);

		//printf("Child %zu at recursion %zu has childs: %zu %zu\n", j, i, left, left + 1);

		subdiv(indices, tetra, split[0], i + 1, recursion, left, offset);
		subdiv(indices, tetra, split[1], i + 1, recursion, left + 1, offset);
	}
}

String stringify(u64 v) {

	String str;

	for (u8 i = 0; i < 64; i += 4)
		str = oic::Log::hexChars[(v >> i) & 0xF] + str;

	return "0x" + str;
}

String TetronHelper::compressTetraLut(bool isCpp) {

	//Build up LUT

	static constexpr usz recursions = 8;

	//static_assert(maxLod == recursions, "Compress tetra expects lod = 8");

	List<f32> indices;
	List<Vec4u16> tetra;

	tetra.resize(6 * (1 << recursions));
	indices.reserve(1 << 5);

	for (usz i = 0; i < 6; ++i) {

		auto tetron = Tetron(Tetron::HexonDivision(i));

		usz j = i << recursions;

		//start and end both index to root node
		tetra[j + 0x00] = registerTetron(indices, tetron);
		tetra[j + (1 << recursions) - 1] = registerTetron(indices, tetron);

		subdiv(indices, tetra, tetron, 0, recursions, 0, j);
	}

	//Stringify indices

	String indicesStr = 
		isCpp ? "static constexpr f32 tetraValues[] = {\n\t" : "const float tetraValues[] = {\n\t";

	for (usz i = 0; i < indices.size(); ++i)
		indicesStr += (i == 0 ? "" : ",") + oic::Log::concat(indices[i]);

	indicesStr += "\n};";

	//Stringify tetras

	String tetraStr = 
		isCpp ? "static constexpr u64 tetras[] = {\n\t" : "const uint64_t tetras[] = {\n\t";

	for (usz i = 0; i < tetra.size(); ++i) {

		u64 v = 
			u64(tetra[i].x) | 
			(u64(tetra[i].y) << 16) | 
			(u64(tetra[i].z) << 32) | 
			(u64(tetra[i].w) << 48);

		tetraStr += stringify(v) + ",\t";

		if (i >> 2 != (i + 1) >> 2)
			tetraStr += "\n\t";
	}

	tetraStr += "\n};";

	return indicesStr + "\n\n" + tetraStr;
}

inline Vec3f32 unpackPos(u16 v) {

	static constexpr u64 shifts[] = { 0, 16, 32, 48 };

	return Vec3f32(
		TetronHelper::tetraValues[v & 0x1F],
		TetronHelper::tetraValues[(v >> 5) & 0x1F],
		TetronHelper::tetraValues[v >> 10]
	);
}

Tetron TetronHelper::fromIndex(u16 i) {

	const Vec4u16& v = *(const Vec4u16*) &TetronHelper::tetras[i];

	return Tetron{
		{ unpackPos(v.x), unpackPos(v.y), unpackPos(v.z), unpackPos(v.w) }
	};
}

u16 TetronHelper::indexOfLod(Tetron::HexonDivision div, u8 lod) {
	return (u16(div) << 8) | (sizeOfLod(lod) - 1);
}

Tetron TetronHelper::getTetra(Tetron::HexonDivision div, u8 lod, u8 i) {
	return fromIndex(indexOfLod(div, lod) + i);
}

Tetron TetronHelper::getTetra(Tetron::HexonDivision div, u8 sideId) {
	return fromIndex((u16(div) << 8) | sideId);
}